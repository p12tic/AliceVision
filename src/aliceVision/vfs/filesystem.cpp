// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "filesystem.hpp"
#include "FilesystemImplUtils.hpp"

namespace aliceVision {
namespace vfs {

std::unique_ptr<generic_filebuf> open_file(const path& path, std::ios_base::openmode mode)
{
    auto cwd = current_path();
    return openFile(getTreeForPathMaybeRelative(path, cwd), path, mode);
}

bool is_virtual_path(const path& p)
{
    if (p.is_absolute() || p.has_root_path())
    {
        return getTreeForPathAbsolute(p) != nullptr;
    }

    return getCurrentPathTree() != nullptr;
}

path absolute(const path& p)
{
    return absolute(p, current_path());
}

path absolute(const path& p, const path& base)
{
    // This function does not access the filesystem, so can be used without checking for existing
    // virtual file systems.
    return boost::filesystem::absolute(p.boost_path(), base.boost_path());
}

path canonical(const path& p)
{
    return canonical(p, current_path());
}

path canonical(const path& p, error_code& ec)
{
    return canonical(p, current_path(), ec);
}

path canonical(const path& p, const path& base)
{
    error_code ec;
    auto result = canonical(p, base, ec);
    throwIfFailedEc(ec, "canonical", p, base);
    return result;
}

path canonical(const path& p, const path& base, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, base);
    if (!tree)
    {
        return boost::filesystem::canonical(p.boost_path(), base.boost_path(), ec);
    }
    return tree->canonical(p, base, ec);
}

void copy(const path& from, const path& to)
{
    error_code ec;
    copy(from, to, ec);
    throwIfFailedEc(ec, "copy", from, to);
}

void copy(const path& from, const path& to, error_code& ec)
{
    auto* treeFrom = getTreeForPathMaybeRelative(from, current_path());
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    if (!treeFrom && !treeTo)
    {
        boost::filesystem::copy(from.boost_path(), to.boost_path(), ec);
        return;
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(from, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;

    throw std::invalid_argument("copy not supported yet");
}

void copy_directory(const path& from, const path& to)
{
    error_code ec;
    copy_directory(from, to, ec);
    throwIfFailedEc(ec, "copy_directory", from, to);
}

void copy_directory(const path& from, const path& to, error_code& ec)
{
    auto* treeFrom = getTreeForPathMaybeRelative(from, current_path());
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    if (!treeFrom && !treeTo)
    {
        boost::filesystem::copy_directory(from.boost_path(), to.boost_path(), ec);
        return;
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(from, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;


    // copy_directory is poorly named function that performs directory creation with the attributes
    // of the source directory. Virtual filesystem does not support attributes, so just create
    // a directory as usual.
    create_directory(to, ec);
}

void copy_file(const path& from, const path& to)
{
    error_code ec;
    copy_file(from, to, ec);
    throwIfFailedEc(ec, "copy_file", from, to);
}

void copy_file(const path& from, const path& to, error_code& ec)
{
    copy_file(from, to, copy_options::none, ec);
}

void copy_file(const path& from, const path& to, copy_options option)
{
    error_code ec;
    copy_file(from, to, option, ec);
    throwIfFailedEc(ec, "copy_file", from, to);
}

void copy_file(const path& from, const path& to, copy_options options, error_code& ec)
{
    ec.clear();
    auto* treeFrom = getTreeForPathMaybeRelative(from, current_path());
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    if (!treeFrom && !treeTo)
    {
        boost::filesystem::copy_file(from.boost_path(), to.boost_path(),
                                     boost::native_value(options), ec);
        return;
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(from, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;

    auto optionCount = 0;
    optionCount += (options & copy_options::overwrite_existing) != copy_options::none;
    optionCount += (options & copy_options::skip_existing) != copy_options::none;
    optionCount += (options & copy_options::update_existing) != copy_options::none;
    if (optionCount > 1)
    {
        throw std::invalid_argument("Invalid options to copy_file");
    }

    if (!vfs::is_regular_file(from, ec))
    {
        ec.assign(boost::system::errc::function_not_supported, boost::system::generic_category());
        return;
    }
    ec.clear();

    if (vfs::is_regular_file(to, ec))
    {
        ec.clear();
        if ((options & copy_options::skip_existing) != copy_options::none)
        {
            return;
        }
    }

    auto fromFile = openFile(treeFrom, from, std::ios_base::in);
    auto toFile = openFile(treeTo, to, std::ios_base::out);

    if (!fromFile->is_open() || !toFile->is_open())
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    std::size_t bufferSize = 1024 * 128;
    std::vector<char> buffer(bufferSize);

    while (true)
    {
        std::size_t readSize = fromFile->sgetn(buffer.data(), buffer.size());
        if (readSize == 0)
            break;

        std::size_t writeSize = toFile->sputn(buffer.data(), readSize);
        if (writeSize < readSize)
        {
            // FIXME: error code could be better
            ec.assign(boost::system::errc::file_too_large, boost::system::generic_category());
            return;
        }
    }
}

void copy_symlink(const path& existing_symlink, const path& new_symlink)
{
    error_code ec;
    copy_symlink(existing_symlink, new_symlink, ec);
    throwIfFailedEc(ec, "copy_symlink", existing_symlink, new_symlink);
}

void copy_symlink(const path& existing_symlink, const path& new_symlink, error_code& ec)
{
    auto* treeExisting = getTreeForPathMaybeRelative(existing_symlink, current_path());
    auto* treeNew = getTreeForPathMaybeRelative(new_symlink, current_path());
    if (!treeExisting && !treeNew)
    {
        boost::filesystem::copy_symlink(existing_symlink.boost_path(), new_symlink.boost_path(), ec);
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(existing_symlink, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(new_symlink, ec))
        return;

    if (treeExisting != treeNew)
    {
        ec.assign(static_cast<int>(std::errc::cross_device_link),
                  boost::system::generic_category());
    }

    throw std::invalid_argument("copy_symlink not supported yet");
}

bool create_directories(const path& p)
{
    error_code ec;
    auto result = create_directories(p, ec);
    throwIfFailedEc(ec, "create_directories", p);
    return result;
}

bool create_directories(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::create_directories(p.boost_path(), ec);
    }

    if (p.empty())
    {
        ec.assign(boost::system::errc::invalid_argument, boost::system::generic_category());
        return false;
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return false;

    if (p.filename_is_dot() || p.filename_is_dot_dot())
        return create_directories(p.parent_path(), ec);

    auto stat = status(p, ec);
    if (stat.type() == file_type::directory_file)
    {
        // already exists
        ec.clear();
        return false;
    }

    if (stat.type() == file_type::status_error)
    {
        // ec already contains error code
        return false;
    }

    path parentPath = p.parent_path();
    if (parentPath == p)
    {
        throw std::runtime_error("Recursion in create_directories()");
    }
    auto parentStatus = status(parentPath, ec);

    if (parentStatus.type() == file_type::file_not_found)
    {
        create_directories(parentPath, ec);
        if (ec)
        {
            return false;
        }
    }

    return create_directory(p, ec);
}

bool create_directory(const path& p)
{
    error_code ec;
    auto result = create_directory(p, ec);
    throwIfFailedEc(ec, "create_directory", p);
    return result;
}

bool create_directory(const path& p, error_code& ec)
{
    auto cwd = current_path();
    auto* tree = getTreeForPathMaybeRelative(p, cwd);
    if (!tree)
    {
        return boost::filesystem::create_directory(p.boost_path(), ec);
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return false;

    return tree->create_directory(absolute(p, cwd), ec);
}

void create_directory_symlink(const path& to, const path& new_symlink)
{
    error_code ec;
    create_directory_symlink(to, new_symlink, ec);
    throwIfFailedEc(ec, "create_directory_symlink", to, new_symlink);
}

void create_directory_symlink(const path& to, const path& new_symlink, error_code& ec)
{
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    auto* treeNew = getTreeForPathMaybeRelative(new_symlink, current_path());
    if (!treeTo && !treeNew)
    {
        boost::filesystem::create_directory_symlink(to.boost_path(), new_symlink.boost_path(), ec);
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(new_symlink, ec))
        return;

    if (treeTo != treeNew)
    {
        ec.assign(static_cast<int>(std::errc::cross_device_link),
                  boost::system::generic_category());
    }

    throw std::invalid_argument("create_directory_symlink not supported yet");
}

void create_hard_link(const path& to, const path& new_hard_link)
{
    error_code ec;
    create_hard_link(to, new_hard_link, ec);
    throwIfFailedEc(ec, "create_hard_link", to, new_hard_link);
}

void create_hard_link(const path& to, const path& new_hard_link, error_code& ec)
{
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    auto* treeNew = getTreeForPathMaybeRelative(new_hard_link, current_path());
    if (!treeTo && !treeNew)
    {
        boost::filesystem::create_hard_link(to.boost_path(), new_hard_link.boost_path(), ec);
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(new_hard_link, ec))
        return;

    if (treeTo != treeNew)
    {
        ec.assign(static_cast<int>(std::errc::cross_device_link),
                  boost::system::generic_category());
    }

    throw std::invalid_argument("create_hard_link not supported yet");
}

void create_symlink(const path& to, const path& new_symlink)
{
    error_code ec;
    create_symlink(to, new_symlink, ec);
    throwIfFailedEc(ec, "create_symlink", to, new_symlink);
}

void create_symlink(const path& to, const path& new_symlink, error_code& ec)
{
    auto* treeTo = getTreeForPathMaybeRelative(to, current_path());
    auto* treeNew = getTreeForPathMaybeRelative(new_symlink, current_path());
    if (!treeTo && !treeNew)
    {
        boost::filesystem::create_symlink(to.boost_path(), new_symlink.boost_path(), ec);
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(new_symlink, ec))
        return;

    if (treeTo != treeNew)
    {
        ec.assign(static_cast<int>(std::errc::cross_device_link),
                  boost::system::generic_category());
    }

    throw std::invalid_argument("create_symlink not supported yet");
}

path current_path()
{
    error_code ec;
    path result = current_path(ec);
    throwIfFailedEc(ec, "current_path");
    return result;
}

path current_path(error_code& ec)
{
    auto currentVirtualPath = getManager().getCurrentPath();
    if (currentVirtualPath.empty())
        return boost::filesystem::current_path(ec);
    return currentVirtualPath;
}

void current_path(const path& p)
{
    error_code ec;
    current_path(p, ec);
    throwIfFailedEc(ec, "current_path", p);
}

void current_path(const path& p, error_code& ec)
{
    if (p.is_absolute())
    {
        if (getTreeForPathAbsolute(p))
        {
            // absolute path rooted
            getManager().setCurrentPath(p.boost_path());
            return;
        }
        boost::filesystem::current_path(p.boost_path(), ec);
        return;
    }

    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return;

    if (getManager().getCurrentPathTree())
    {
        getManager().setCurrentPath(p.boost_path());
        return;
    }

    boost::filesystem::current_path(p.boost_path(), ec);
}

bool exists(file_status s) noexcept
{
    return boost::filesystem::exists(s);
}

bool exists(const path& p)
{
    return vfs::exists(status(p));
}

bool exists(const path& p, error_code& ec)
{
    return vfs::exists(status(p, ec));
}

bool equivalent(const path& p1, const path& p2)
{
    error_code ec;
    auto result = equivalent(p1, p2, ec);
    throwIfFailedEc(ec, "equivalent", p1, p2);
    return result;
}

bool equivalent(const path& p1, const path& p2, error_code& ec)
{
    return boost::filesystem::equivalent(p1.boost_path(), p2.boost_path(), ec);
}

std::uintmax_t file_size(const path& p)
{
    error_code ec;
    auto result = file_size(p, ec);
    throwIfFailedEc(ec, "file_size", p);
    return result;
}

std::uintmax_t file_size(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::file_size(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return 0;

    return tree->file_size(p, ec);
}

std::uintmax_t hard_link_count(const path& p)
{
    error_code ec;
    auto result = hard_link_count(p, ec);
    throwIfFailedEc(ec, "hard_link_count", p);
    return result;
}

std::uintmax_t hard_link_count(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::hard_link_count(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return 0;

    return tree->hard_link_count(ec);
}

bool is_directory(file_status s) noexcept
{
    return boost::filesystem::is_directory(s);
}

bool is_directory(const path& p)
{
    return vfs::is_directory(status(p));
}

bool is_directory(const path& p, error_code& ec)
{
    return vfs::is_directory(status(p, ec));
}

bool is_empty(const path& p)
{
    error_code ec;
    auto result = is_empty(p, ec);
    throwIfFailedEc(ec, "is_empty", p);
    return result;
}

bool is_empty(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::is_empty(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return true;

    throw std::invalid_argument("is_empty not supported yet");
}

bool is_other(file_status s) noexcept
{
    return boost::filesystem::is_other(s);
}

bool is_other(const path& p)
{
    return vfs::is_other(status(p));
}

bool is_other(const path& p, error_code& ec)
{
    return vfs::is_other(status(p, ec));
}

bool is_regular_file(file_status s) noexcept
{
    return boost::filesystem::is_regular_file(s);
}

bool is_regular_file(const path& p)
{
    return vfs::is_regular_file(status(p));
}

bool is_regular_file(const path& p, error_code& ec)
{
    return vfs::is_regular_file(status(p, ec));
}

bool is_symlink(file_status s)
{
    return boost::filesystem::is_symlink(s);
}

bool is_symlink(const path& p)
{
    return vfs::is_symlink(symlink_status(p));
}

bool is_symlink(const path& p, error_code& ec)
{
    return vfs::is_symlink(symlink_status(p, ec));
}

std::time_t last_write_time(const path& p)
{
    error_code ec;
    auto result = last_write_time(p, ec);
    throwIfFailedEc(ec, "last_write_time", p);
    return result;
}

std::time_t last_write_time(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::last_write_time(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return 0;

    throw std::invalid_argument("last_write_time not supported yet");
}

void last_write_time(const path& p, const std::time_t new_time)
{
    error_code ec;
    last_write_time(p, new_time, ec);
    throwIfFailedEc(ec, "last_write_time", p);
}

void last_write_time(const path& p, const std::time_t new_time, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        boost::filesystem::last_write_time(p.boost_path(), new_time, ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return;

    throw std::invalid_argument("last_write_time not supported yet");
}

path read_symlink(const path& p)
{
    error_code ec;
    auto result = read_symlink(p, ec);
    throwIfFailedEc(ec, "read_symlink", p);
    return result;
}

path read_symlink(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::read_symlink(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return path();

    throw std::invalid_argument("read_symlink not supported yet");
}

path relative(const path& p)
{
    error_code ec;
    auto result = relative(p, current_path(), ec);
    throwIfFailedEc(ec, "relative", p);
    return result;
}

path relative(const path& p, error_code& ec)
{
    return relative(p, current_path(), ec);
}

path relative(const path& p, const path& base)
{
    error_code ec;
    auto result = relative(p, base, ec);
    throwIfFailedEc(ec, "relative", p, base);
    return result;
}

path relative(const path& p, const path& base, error_code& ec)
{
    // This function is defined by the documentation exactly, there's no point in calling boost.
    path canonicalP = weakly_canonical(p, ec);
    if (ec)
        return {};

    path canonicalBase = weakly_canonical(base, ec);
    if (ec)
        return {};

    return canonicalP.lexically_relative(canonicalBase);
}

bool remove(const path& p)
{
    error_code ec;
    auto result = remove(p, ec);
    throwIfFailedEc(ec, "remove", p);
    return result;
}

bool remove(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::remove(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return false;

    return tree->remove(absolute(p, current_path()), ec);
}

std::uintmax_t remove_all(const path& p)
{
    error_code ec;
    auto result = remove_all(p, ec);
    throwIfFailedEc(ec, "remove_all", p);
    return result;
}

std::uintmax_t remove_all(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::remove_all(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return 0;

    return tree->remove_all(p, ec);
}

void rename(const path& from, const path& to)
{
    error_code ec;
    rename(from, to, ec);
    throwIfFailedEc(ec, "rename", from, to);
}

void rename(const path& from, const path& to, error_code& ec)
{
    auto cwd = current_path();
    auto* treeFrom = getTreeForPathMaybeRelative(from, cwd);
    auto* treeTo = getTreeForPathMaybeRelative(to, cwd);
    if (!treeFrom && !treeTo)
    {
        boost::filesystem::rename(from.boost_path(), to.boost_path(), ec);
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(from, ec))
        return;
    if (checkUnsupportedVirtualPathWithoutRootDirectory(to, ec))
        return;

    if (treeFrom != treeTo)
    {
        copy_file(from, to, ec);
        if (ec)
            return;
        remove(from, ec);
        return;
    }

    treeFrom->rename(absolute(from, cwd), absolute(to, cwd), ec);
}

void resize_file(const path& p, std::uintmax_t size)
{
    error_code ec;
    resize_file(p, size, ec);
    throwIfFailedEc(ec, "resize_file", p);
}

void resize_file(const path& p, std::uintmax_t size, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        boost::filesystem::resize_file(p.boost_path(), size, ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return;

    throw std::invalid_argument("resize_file not supported yet");
}

space_info space(const path& p)
{
    error_code ec;
    auto result = space(p, ec);
    throwIfFailedEc(ec, "space", p);
    return result;
}

space_info space(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::space(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return {};

    return tree->space(ec);
}

file_status status(const path& p)
{
    // Note that status works differently than the rest of IO functions in handling the error code
    // data.
    error_code ec;
    auto result = status(p, ec);
    if (result.type() == file_type::status_error)
    {
        throw boost::filesystem::filesystem_error("status", p.boost_path(), ec);
    }
    return result;
}

file_status status(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::status(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return file_status{file_type::status_error};

    return tree->status(p, ec);
}

bool status_known(file_status s) noexcept
{
    return boost::filesystem::status_known(s);
}

file_status symlink_status(const path& p)
{
    error_code ec;
    auto result = symlink_status(p, ec);
    throwIfFailedEc(ec, "symlink_status", p);
    return result;
}

file_status symlink_status(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::symlink_status(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return file_status{file_type::status_error};

    throw std::invalid_argument("symlink_status not supported yet");
}

path system_complete(const path& p)
{
    error_code ec;
    auto result = system_complete(p, ec);
    throwIfFailedEc(ec, "system_complete", p);
    return result;
}

path system_complete(const path& p, error_code& ec)
{
    auto cwd = current_path();
    auto* tree = getTreeForPathMaybeRelative(p, cwd);
    if (!tree)
    {
        return boost::filesystem::system_complete(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return path();

    return (p.empty() || p.is_absolute()) ? p : cwd / p;
}

path temp_directory_path()
{
    error_code ec;
    auto result = temp_directory_path(ec);
    throwIfFailedEc(ec, "temp_directory_path");
    return result;
}

path temp_directory_path(error_code& ec)
{
    auto tempDirPath = getManager().getTemporaryDirectoryPath();
    if (!tempDirPath.empty())
    {
        return tempDirPath;
    }

    return boost::filesystem::temp_directory_path(ec);
}

path unique_path(const path& model)
{
    // this function is not dependent on filesystem functionality, thus we're calling boost directly
    return boost::filesystem::unique_path(model.boost_path());
}

path unique_path(const path& model, error_code& ec)
{
    // this function is not dependent on filesystem functionality, thus we're calling boost directly
    return boost::filesystem::unique_path(model.boost_path(), ec);
}

path weakly_canonical(const path& p)
{
    error_code ec;
    auto result = weakly_canonical(p, ec);
    throwIfFailedEc(ec, "weakly_canonical", p);
    return result;
}

path weakly_canonical(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        return boost::filesystem::weakly_canonical(p.boost_path(), ec);
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return {};

    return tree->weakly_canonical(p, ec);
}

void set_special_data(const path& p, const std::shared_ptr<special_data>& data)
{
    error_code ec;
    set_special_data(p, data, ec);
    throwIfFailedEc(ec, "set_special_data", p);
}

void set_special_data(const path& p, const std::shared_ptr<special_data>& data, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return;

    tree->set_special_data(p, data, ec);
}

std::shared_ptr<special_data> get_special_data(const path& p)
{
    error_code ec;
    auto result = get_special_data(p, ec);
    throwIfFailedEc(ec, "get_special_data", p);
    return result;
}

std::shared_ptr<special_data> get_special_data_if_exists(const path& p)
{
    error_code ec;
    return get_special_data(p, ec);
}

std::shared_ptr<special_data> get_special_data(const path& p, error_code& ec)
{
    auto* tree = getTreeForPathMaybeRelative(p, current_path());
    if (!tree)
    {
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return nullptr;
    }
    if (checkUnsupportedVirtualPathWithoutRootDirectory(p, ec))
        return nullptr;

    return tree->get_special_data(p, ec);
}

} //namespace vfs
} //namespace aliceVision
