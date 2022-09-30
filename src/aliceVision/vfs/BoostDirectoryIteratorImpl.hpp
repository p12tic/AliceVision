// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "boost_common.hpp"
#include "IDirectoryIteratorImpl.hpp"

namespace aliceVision {
namespace vfs {

class BoostDirectoryIteratorImpl : public IDirectoryIteratorImpl {
public:
    BoostDirectoryIteratorImpl(const boost::filesystem::directory_iterator& it);
    ~BoostDirectoryIteratorImpl() override;

    void increment(error_code& ec) override;
    directory_entry dereference() override;
    bool is_end() override;

private:
    boost::filesystem::directory_iterator _it;
};

} // namespace vfs
} // namespace aliceVision
