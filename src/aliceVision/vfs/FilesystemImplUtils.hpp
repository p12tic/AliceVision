// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "filesystem.hpp"
#include "FilesystemManager.hpp"
#include "std_filebuf.hpp"

namespace aliceVision {
namespace vfs {

inline IFilesystemTree* getTreeForPathAbsolute(const path& p)
{
    return getManager().getTreeAtRootIfExists(p.boost_path().root_name());
}

// base must be absolute path
inline IFilesystemTree* getTreeForPathMaybeRelative(const path& p, const path& base)
{
    return getTreeForPathAbsolute((p.is_absolute() || p.has_root_path()) ? p : base);
}

inline IFilesystemTree* getCurrentPathTree()
{
    return getManager().getCurrentPathTree();
}

inline bool checkUnsupportedVirtualPathWithoutRootDirectory(const path& p, error_code& ec)
{
    if (!p.is_absolute() && p.has_root_name() &&
        getManager().getTreeAtRootIfExists(p.boost_path().root_name()))
    {
        ec.assign(boost::system::errc::no_such_file_or_directory,
                  boost::system::generic_category());
        return true;
    }
    return false;
}

inline void throwIfFailedEc(error_code ec, const char* msg)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, ec);
    }
}

inline void throwIfFailedEc(error_code ec, const char* msg, const path& path)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, path.boost_path(), ec);
    }
}

inline void throwIfFailedEc(error_code ec, const char* msg, const path& path1, const path& path2)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, path1.boost_path(), path2.boost_path(), ec);
    }
}

inline std::unique_ptr<generic_filebuf> openFile(IFilesystemTree* tree, const path& path,
                                                 std::ios_base::openmode mode)
{
    if (tree)
        return tree->open(path, mode);
    return std::make_unique<std_filebuf>(path.string(), mode);
}

} // namespace vfs
} // namespace aliceVision
