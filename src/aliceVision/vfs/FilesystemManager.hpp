// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "IFilesystemTree.hpp"
#include "boost_common.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <mutex>

namespace aliceVision {
namespace vfs {

class FilesystemManager {
public:
    FilesystemManager();
    ~FilesystemManager();

    /**
     * Installs filesystem tree at the given root name. Root name must be in the format of
     * `//alphanumeric_string/`. Exception will be thrown if a tree at the root name already exists.
     */
    void installTreeAtRoot(const boost::filesystem::path& root_name,
                           std::unique_ptr<IFilesystemTree>&& tree);

    /**
     * Returns a filesystem tree previously installed at a given root, or nullptr if no such
     * tree exists.
     */
    IFilesystemTree* getTreeAtRootIfExists(const boost::filesystem::path& root_name) const;

    /**
     * Returns filesystem tree for the current working directory. If current working directory is
     * in a tree managed by this class, then returns nullptr.
     */
    IFilesystemTree* getCurrentPathTree() const;

    /**
     * Sets the current working directory.
     *
     * If the given path is absolute and outside of any known trees, then after this call:
     *   - getCurrentPathTree() will return nullptr.
     *   - getCurrentPath() will return empty path.
     *
     * If the given path is absolute and inside a known tree, then after this call:
     *   - getCurrentPathTree() will return that tree.
     *   - getCurrentPath() will return the given path.
     *
     * If the given path is relative and there is no current path set (getCurrentPath() is empty
     * and getCurrentPathTree() returns nullptr), then this function has no effect
     *
     * If the given path is relative and there is current path set, then after this call:
     *  - getCurrentPathTree() returns the same value
     *  - getCurrentPath() returns a value equal to canonical(getCurrentPath() / path)
     */
    // TODO: this function may fail via canonical(). Handle errors accordingly.
    void setCurrentPath(const boost::filesystem::path& path);

    /**
     * Returns the current working directory if it is inside a tree, or an empty path if it is
     * outside.
     */
    boost::filesystem::path getCurrentPath() const;

    /**
     * Sets temporary directory path.
     */
    void setTemporaryDirectoryPath(const boost::filesystem::path& path);

    /**
     * Returns temporary directory path. If the path is empty, then the caller should use existing
     * filesystem facilities to retrieve temporary directory path.
     */
    boost::filesystem::path getTemporaryDirectoryPath() const;

    /**
     * Removes all installed trees and reinitializes the manager to default state. Useful in tests
     */
    void clear();

private:

    IFilesystemTree* getTreeAtRootIfExistsNoLock(const boost::filesystem::path& root_name) const;

    // TODO: it makes sense to have a read-write lock here.
    mutable std::mutex _mutex;
    boost::container::flat_map<boost::filesystem::path,
                               std::unique_ptr<IFilesystemTree>> _trees;
    IFilesystemTree* _currentPathTree = nullptr;
    boost::filesystem::path _currentPathInTree;

    mutable std::mutex _tempDirPathMutex;
    boost::filesystem::path _tempDirPath;
};

FilesystemManager& getManager();

} // namespace vfs
} // namespace aliceVision
