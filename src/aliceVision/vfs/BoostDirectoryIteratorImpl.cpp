// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "BoostDirectoryIteratorImpl.hpp"

namespace aliceVision {
namespace vfs {

BoostDirectoryIteratorImpl::BoostDirectoryIteratorImpl(
        const boost::filesystem::directory_iterator& it) : _it{it}
{}

BoostDirectoryIteratorImpl::~BoostDirectoryIteratorImpl() = default;

void BoostDirectoryIteratorImpl::increment(error_code& ec)
{
    ++_it;
};

directory_entry BoostDirectoryIteratorImpl::dereference()
{
    return *_it;
}

bool BoostDirectoryIteratorImpl::is_end()
{
    return _it == boost::filesystem::directory_iterator();
}

} // namespace vfs
} // namespace aliceVision
