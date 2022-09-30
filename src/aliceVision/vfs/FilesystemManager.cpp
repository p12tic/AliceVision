// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "FilesystemManager.hpp"

namespace aliceVision {
namespace vfs {

FilesystemManager::FilesystemManager() = default;
FilesystemManager::~FilesystemManager() = default;

void FilesystemManager::installTreeAtRoot(const boost::filesystem::path &root_name,
                                          std::unique_ptr<IFilesystemTree>&& tree)
{
    std::lock_guard<std::mutex> lock{_mutex};
    if (!_trees.emplace(root_name, std::move(tree)).second)
    {
        throw std::runtime_error("Filesystem tree with given root name already exists");
    }
}

IFilesystemTree*
    FilesystemManager::getTreeAtRootIfExists(const boost::filesystem::path& root_name) const
{
    std::lock_guard<std::mutex> lock{_mutex};
    return getTreeAtRootIfExistsNoLock(root_name);
}

IFilesystemTree*
    FilesystemManager::getTreeAtRootIfExistsNoLock(const boost::filesystem::path& root_name) const
{
    auto it = _trees.find(root_name);
    if (it == _trees.end())
    {
        return nullptr;
    }
    return it->second.get();
}

IFilesystemTree* FilesystemManager::getCurrentPathTree() const
{
    std::lock_guard<std::mutex> lock{_mutex};
    return _currentPathTree;
}

void FilesystemManager::setCurrentPath(const boost::filesystem::path& path)
{
    std::lock_guard<std::mutex> lock{_mutex};
    if (path.is_absolute())
    {
        _currentPathTree = getTreeAtRootIfExistsNoLock(path.root_name());
        if (_currentPathTree == nullptr)
        {
            _currentPathInTree.clear();
        }
        else
        {
            // FIXME: call canonical() here
            _currentPathInTree = path;
        }
    }
    else if (_currentPathTree)
    {
        // FIXME: call canonical() here.
        _currentPathInTree = _currentPathInTree / path;
    }
    else
    {
        throw std::runtime_error("No current path but relative path given");
    }
}

boost::filesystem::path FilesystemManager::getCurrentPath() const
{
    std::lock_guard<std::mutex> lock{_mutex};
    return _currentPathInTree;
}

void FilesystemManager::setTemporaryDirectoryPath(const boost::filesystem::path& path)
{
    std::lock_guard<std::mutex> lock{_tempDirPathMutex};
    _tempDirPath = path;
}


boost::filesystem::path FilesystemManager::getTemporaryDirectoryPath() const
{
    std::lock_guard<std::mutex> lock{_tempDirPathMutex};
    return _tempDirPath;
}

void FilesystemManager::clear()
{
    _currentPathTree = nullptr;
    _currentPathInTree.clear();
    _tempDirPath.clear();
    _trees.clear();
}

FilesystemManager& getManager()
{
    static FilesystemManager manager;
    return manager;
}

} // namespace vfs
} // namespace aliceVision
