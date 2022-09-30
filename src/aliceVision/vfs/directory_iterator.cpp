// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "directory_iterator.hpp"
#include "FilesystemImplUtils.hpp"
#include "BoostDirectoryIteratorImpl.hpp"

namespace aliceVision {
namespace vfs {

directory_iterator::directory_iterator() noexcept {}

directory_iterator::directory_iterator(const directory_iterator& other) : _it{other._it}
{
}

directory_iterator::directory_iterator(const path& p, directory_options opts)
{
    auto cwd = current_path();
    auto *tree = getTreeForPathMaybeRelative(p, cwd);
    if (tree)
    {
        _it = tree->open_directory(p, opts);
    }
    else
    {
        boost::filesystem::directory_iterator it(p.boost_path(), boost::native_value(opts));
        if (it != boost::filesystem::directory_iterator())
        {
            _it = std::make_shared<BoostDirectoryIteratorImpl>(it);
        }
    }
}

directory_iterator::~directory_iterator() = default;

directory_iterator& directory_iterator::operator=(const directory_iterator& other)
{
    _it = other._it;
    return *this;
}

directory_iterator& directory_iterator::operator++()
{
    error_code ec;
    increment(ec);
    if (ec)
    {
        throw boost::filesystem::filesystem_error("directory_iterator::operator++", ec);
    }
    _isCached = false;
    return *this;
}

directory_iterator& directory_iterator::increment(error_code& ec)
{
    _it->increment(ec);
    if (_it->is_end())
    {
        _it.reset();
    }
    return *this;
}

const directory_entry& directory_iterator::operator*() const
{
    if (!_isCached)
    {
        _entry = directory_entry(_it->dereference());
        _isCached = true;
    }
    return _entry;
}

const directory_entry* directory_iterator::operator->() const
{
    return &*(*this);
}

directory_iterator begin(const directory_iterator& iter)
{
    return iter;
}

directory_iterator end(const directory_iterator& iter)
{
    return {};
}

} // namespace vfs
} // namespace aliceVision
