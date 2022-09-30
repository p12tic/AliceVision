// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "boost_common.hpp"
#include "directory_entry.hpp"

namespace aliceVision {
namespace vfs {

class IDirectoryIteratorImpl {
public:
    virtual ~IDirectoryIteratorImpl();

    virtual void increment(error_code& ec) = 0;
    virtual directory_entry dereference() = 0;
    virtual bool is_end() = 0;
};

} // namespace vfs
} // namespace aliceVision
