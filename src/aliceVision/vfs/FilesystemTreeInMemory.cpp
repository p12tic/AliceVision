// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "FilesystemTreeInMemory.hpp"
#include "filesystem.hpp"
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <algorithm>
#include <memory>
#include <unordered_map>

namespace aliceVision {
namespace vfs {

struct FsTreeFile {
    std::mutex mutex;
    std::vector<char> data;
};

struct FsTreeNode {
    std::mutex mutex;
    file_type type = file_type::status_error;

    // valid only if type == file_type::regular_file
    std::shared_ptr<FsTreeFile> file;

    // valid only if type == file_type::directory_file
    std::unordered_map<std::string, std::shared_ptr<FsTreeNode>> entries;
};

class FilesystemTreeInMemoryFileBuf : public generic_filebuf {
public:
    FilesystemTreeInMemoryFileBuf(const std::shared_ptr<FsTreeFile>& file,
                                  std::ios_base::openmode mode) :
        _file{file}
    {
        if ((mode & std::ios_base::app) != 0)
        {
            _writePos = _file->data.size();
        }
        else if ((mode & std::ios_base::out) != 0)
        {
            _file->data.clear();
        }
    }

    bool is_open() const override
    {
        return _file != nullptr;
    }

    FilesystemTreeInMemoryFileBuf* close() override
    {
        _file = nullptr;
        return this;
    }

protected:
    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which) override
    {
        auto failedRes = static_cast<pos_type>(static_cast<off_type>(-1));
        auto inOut = std::ios_base::in | std::ios_base::out;

        if (dir == std::ios_base::beg)
        {
            if (off < 0)
            {
                return failedRes;
            }
            if (which & std::ios_base::in)
                _readPos = off;
            if (which & std::ios_base::out)
                _writePos = off;
            return 0;
        }

        if (dir == std::ios_base::cur)
        {
            if ((which & inOut) == inOut)
            {
                return failedRes;
            }

            if (which & std::ios_base::in)
            {
                if (off < 0 && -off > _readPos)
                {
                    return failedRes;
                }
                _readPos += off;
                return _readPos;
            }

            if (which & std::ios_base::out)
            {
                if (off < 0 && -off > _writePos)
                {
                    return failedRes;
                }
                _writePos += off;
                return _writePos;
            }
            return failedRes;
        }

        if (dir == std::ios_base::end)
        {
            std::lock_guard<std::mutex> lock{_file->mutex};
            auto size = _file->data.size();
            if (off < 0 && -off > size)
            {
                return failedRes;
            }
            if (which & std::ios_base::in)
                _readPos = size + off;
            if (which & std::ios_base::out)
                _writePos = size + off;
            return size;
        }
        return failedRes;
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode which) override
    {
        if (which & std::ios_base::in)
            _readPos = pos;
        if (which & std::ios_base::out)
            _writePos = pos;
        return pos;
    }

    int_type uflow() override
    {
        std::lock_guard<std::mutex> lock{_file->mutex};
        if (_readPos >= _file->data.size())
            return traits_type::eof();
        return _file->data[_readPos++];
    }

    int_type underflow() override
    {
        std::lock_guard<std::mutex> lock{_file->mutex};
        if (_readPos >= _file->data.size())
            return traits_type::eof();
        return _file->data[_readPos];
    }

    std::streamsize xsgetn(char_type* s, std::streamsize n) override
    {
        std::lock_guard<std::mutex> lock{_file->mutex};
        if (_readPos >= _file->data.size())
        {
            return 0;
        }
        auto available = _file->data.size() - _readPos;
        auto toRead = std::min<std::size_t>(available, n);
        std::copy(_file->data.data() + _readPos, _file->data.data() + _readPos + toRead, s);
        _readPos += toRead;
        return toRead;
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override
    {
        std::lock_guard<std::mutex> lock{_file->mutex};
        if (_writePos == _file->data.size())
        {
            _file->data.insert(_file->data.end(), s, s + n);
            _writePos += n;
            return n;
        }

        if (_writePos > _file->data.size())
        {
            auto toAppend = _writePos - _file->data.size();
            _file->data.insert(_file->data.end(), toAppend, '\0');
            _file->data.insert(_file->data.end(), s, s + n);
            _writePos += n;
            return n;
        }

        auto available = _file->data.size() - _writePos;
        auto toReuse = std::min<std::size_t>(available, n);
        std::copy(s, s + toReuse, _file->data.data() + _writePos);
        _writePos += toReuse;
        n -= toReuse;
        s += toReuse;

        if (n > 0)
        {
            _file->data.insert(_file->data.end(), s, s + n);
            _writePos += n;
        }
        return n;
    }

    int_type overflow(int_type c) override
    {
        if (!traits_type::eq_int_type(c, traits_type::eof()))
        {
            std::lock_guard<std::mutex> lock{_file->mutex};
            if (_writePos < _file->data.size())
            {
                _file->data[_writePos++] = c;
            }
            else
            {
                if (_writePos == _file->data.size())
                {
                    _file->data.push_back(c);
                }
                else
                {
                    auto toInsert = _file->data.size() - _writePos;
                    _file->data.insert(_file->data.end(), toInsert - 1, '\0');
                    _file->data.push_back(c);
                }
            }
        }
        return traits_type::eof() + 1;
    }

private:
    std::shared_ptr<FsTreeFile> _file;
    std::size_t _readPos = 0;
    std::size_t _writePos = 0;

};

namespace {

std::vector<std::string> splitPath(const path& p)
{
    std::vector<std::string> parts;
    boost::split(parts, p.lexically_normal().relative_path().string(), boost::is_any_of("\\/"));
    if (!parts.empty() && parts.back() == ".")
    {
        parts.pop_back();
    }
    return parts;
}

std::shared_ptr<FsTreeNode> createFileNode()
{
    auto result = std::make_shared<FsTreeNode>();
    result->type = file_type::regular_file;
    result->file = std::make_shared<FsTreeFile>();
    return result;
}

std::shared_ptr<FsTreeNode> createDirectoryNode()
{
    auto result = std::make_shared<FsTreeNode>();
    result->type = file_type::directory_file;
    return result;
}

std::shared_ptr<FsTreeNode> findTreeNode(std::shared_ptr<FsTreeNode> nodePtr,
                                         const std::vector<std::string>& parts)
{
    for (std::size_t i = 0; i < parts.size(); ++i)
    {
        if (nodePtr->type != file_type::directory_file)
            return nullptr;
        std::shared_ptr<FsTreeNode> tempPtr;
        {
            std::lock_guard<std::mutex> lock{nodePtr->mutex};
            auto it = nodePtr->entries.find(parts[i]);
            if (it == nodePtr->entries.end())
            {
                return nullptr;
            }
            tempPtr = it->second;
        }
        // lock is now unlocked, nothing refers to parent nodePtr, so we can release the pointer
        std::swap(nodePtr, tempPtr);
    }
    return nodePtr;
}

} // namespace

struct FilesystemTreeInMemory::Data
{
    // we don't strictly need root as a shared_ptr, but it makes the rest of code much simplier
    std::shared_ptr<FsTreeNode> root;
};

FilesystemTreeInMemory::FilesystemTreeInMemory() : _d{std::make_unique<Data>()}
{
    _d->root = createDirectoryNode();
}

FilesystemTreeInMemory::~FilesystemTreeInMemory() = default;

std::unique_ptr<generic_filebuf> FilesystemTreeInMemory::open(const path& p,
                                                              std::ios_base::openmode mode)
{
    if (!p.is_absolute())
        return nullptr;

    auto parts = splitPath(p);
    if (parts.empty())
        return nullptr;

    auto filename = parts.back();
    parts.pop_back();

    auto directoryNode = findTreeNode(_d->root, parts);
    if (!directoryNode || directoryNode->type != file_type::directory_file)
        return nullptr;

    std::lock_guard<std::mutex> lock{directoryNode->mutex};
    auto it = directoryNode->entries.find(filename);
    if (it == directoryNode->entries.end())
    {
        if (mode & std::ios_base::out)
        {
            // create leaf file
            auto newNode = createFileNode();
            directoryNode->entries.emplace(filename, newNode);
            return std::make_unique<FilesystemTreeInMemoryFileBuf>(newNode->file, mode);
        }
        return nullptr;
    }

    if (it->second->type != file_type::regular_file)
        return nullptr;

    // found existing file
    return std::make_unique<FilesystemTreeInMemoryFileBuf>(it->second->file, mode);
}

bool FilesystemTreeInMemory::create_directory(const path& p, error_code& ec)
{
    ec.clear();
    if (!p.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return false;
    }

    auto parts = splitPath(p);
    if (parts.empty())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return false;
    }

    auto dirname = parts.back();
    parts.pop_back();

    auto directoryNode = findTreeNode(_d->root, parts);
    if (!directoryNode || directoryNode->type != file_type::directory_file)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return false;
    }

    std::lock_guard<std::mutex> lock{directoryNode->mutex};
    auto it = directoryNode->entries.find(dirname);
    if (it == directoryNode->entries.end())
    {
        directoryNode->entries.emplace(dirname, createDirectoryNode());
        return true;
    }

    if (it->second->type != file_type::directory_file)
    {
        ec.assign(boost::system::errc::file_exists, boost::system::generic_category());
    }
    return false;
}

void FilesystemTreeInMemory::rename(const path& from, const path& to, error_code& ec)
{
    ec.clear();
    if (!from.is_absolute() || !to.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    auto from2to = from.lexically_normal().lexically_relative(to.lexically_normal()).string();
    auto to2from = to.lexically_normal().lexically_relative(from.lexically_normal()).string();
    if (from2to == ".")
        return; // same path

    if (from2to.substr(0, 2) != ".." && to2from.substr(0, 2) != "..")
    {
        // one path is child of another
        ec.assign(boost::system::errc::invalid_argument, boost::system::generic_category());
        return;
    }

    auto fromParts = splitPath(from);
    auto toParts = splitPath(to);
    if (fromParts.empty() || toParts.empty())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    auto fromFilename = fromParts.back();
    fromParts.pop_back();
    auto toFilename = toParts.back();
    toParts.pop_back();

    auto fromNode = findTreeNode(_d->root, fromParts);
    auto toNode = findTreeNode(_d->root, toParts);
    if (!fromNode || fromNode->type != file_type::directory_file ||
        !toNode || toNode->type != file_type::directory_file)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    std::lock_guard<std::mutex> lockFrom{fromNode->mutex};
    std::unique_lock<std::mutex> lockTo;
    if (fromNode != toNode)
    {
        lockTo = std::unique_lock<std::mutex>{toNode->mutex};
    }

    auto itFrom = fromNode->entries.find(fromFilename);
    if (itFrom == fromNode->entries.end())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    // remove destination if needed
    auto itTo = toNode->entries.find(toFilename);
    if (itTo != toNode->entries.end())
    {
        if (itTo->second->type != file_type::directory_file)
        {
            toNode->entries.erase(itTo);
        }
        else
        {
            std::lock_guard<std::mutex> toChildLock{itTo->second->mutex};
            if (!itTo->second->entries.empty())
            {
                ec.assign(boost::system::errc::directory_not_empty, boost::system::generic_category());
                return;
            }
        }
    }

    auto renamedNode = itFrom->second;
    fromNode->entries.erase(itFrom);
    toNode->entries.emplace(toFilename, renamedNode);
}

path FilesystemTreeInMemory::canonical(const path& p, const path& base, error_code& ec)
{
    auto absolute = vfs::absolute(p, base);
    if (!vfs::exists(status(absolute, ec)))
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return path();
    }

    return absolute.lexically_normal();
}

path FilesystemTreeInMemory::weakly_canonical(const path& p, error_code& ec)
{
    // The filesystem does not support symlinks, so it's enough to lexically normalize the path
    return p.lexically_normal();
}

std::uintmax_t FilesystemTreeInMemory::file_size(const path& p, error_code& ec)
{
    ec.clear();
    if (!p.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return -1;
    }

    auto parts = splitPath(p);
    auto fileNode = findTreeNode(_d->root, parts);
    if (!fileNode)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return -1;
    }
    if (fileNode->type != file_type::regular_file)
    {
        return -1;
    }
    std::lock_guard<std::mutex> lock{fileNode->file->mutex};
    return fileNode->file->data.size();
}

file_status FilesystemTreeInMemory::status(const path& p, error_code& ec)
{
    ec.clear();
    if (!p.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return file_status{file_type::file_not_found};
    }

    auto parts = splitPath(p);
    auto treeNode = findTreeNode(_d->root, parts);
    if (!treeNode)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return file_status{file_type::file_not_found};
    }

    return file_status{treeNode->type};
}

bool FilesystemTreeInMemory::remove(const path& p, error_code& ec)
{
    ec.clear();
    if (!p.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return false;
    }

    error_code status_ec;
    if (!vfs::exists(status(p, status_ec)))
    {
        if (status_ec.value() != boost::system::errc::no_such_file_or_directory) {
            ec = status_ec;
        }
        return false;
    }

    auto parts = splitPath(p);
    auto filename = parts.back();
    parts.pop_back();
    auto treeNode = findTreeNode(_d->root, parts);
    if (!treeNode || treeNode->type != file_type::directory_file)
    {
        // shouldn't usually happen because we've already checked the path for existence
        return false;
    }

    std::lock_guard<std::mutex> lock{treeNode->mutex};
    auto it = treeNode->entries.find(filename);
    if (it == treeNode->entries.end())
    {
        // shouldn't usually happen because we've already checked the path for existence
        return false;
    }

    if (it->second->type != file_type::directory_file)
    {
        treeNode->entries.erase(it);
        return true;
    }

    {
        std::lock_guard<std::mutex> childLock{it->second->mutex};
        if (!it->second->entries.empty())
        {
            ec.assign(boost::system::errc::directory_not_empty, boost::system::generic_category());
            return true;
        }
    }
    treeNode->entries.erase(it);
    return true;
}

std::uintmax_t FilesystemTreeInMemory::remove_all(const path& p, error_code& ec)
{
    // FIXME: fix the return value of this function
    ec.clear();
    if (!p.is_absolute())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return 0;
    }

    if (!vfs::exists(status(p, ec)))
    {
        return 0;
    }
    ec.clear();

    auto parts = splitPath(p);
    auto filename = parts.back();
    parts.pop_back();
    auto treeNode = findTreeNode(_d->root, parts);
    if (!treeNode || treeNode->type != file_type::directory_file)
    {
        // shouldn't usually happen because we've already checked the path for existence
        return 0;
    }

    std::lock_guard<std::mutex> lock{treeNode->mutex};
    auto it = treeNode->entries.find(filename);
    if (it == treeNode->entries.end())
    {
        // shouldn't usually happen because we've already checked the path for existence
        return 0;
    }
    treeNode->entries.erase(it);
    return 1;
}

std::uintmax_t FilesystemTreeInMemory::hard_link_count(error_code& ec)
{
    return 0;
}

space_info FilesystemTreeInMemory::space(error_code& ec)
{
    return space_info{0, 0, 0};
}

} // namespace vfs
} // namespace aliceVision
