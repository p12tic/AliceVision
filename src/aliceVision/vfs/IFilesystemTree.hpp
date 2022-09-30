// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "boost_common.hpp"
#include "generic_filebuf.hpp"
#include "path.hpp"
#include "special_data.hpp"
#include "IDirectoryIteratorImpl.hpp"

namespace aliceVision {
namespace vfs {

class IFilesystemTree
{
public:
    virtual ~IFilesystemTree();

    virtual std::unique_ptr<generic_filebuf> open(const path& p, std::ios_base::openmode mode) = 0;
    virtual std::shared_ptr<IDirectoryIteratorImpl> open_directory(const path& p,
                                                                   directory_options opts) = 0;
    virtual bool create_directory(const path& p, error_code& ec) = 0;
    virtual void rename(const path& from, const path& to, error_code& ec) = 0;

    virtual path canonical(const path& p, const path& base, error_code& ec) = 0;
    virtual path weakly_canonical(const path& p, error_code& ec) = 0;
    virtual std::uintmax_t file_size(const path& p, error_code& ec) = 0;
    virtual file_status status(const path& p, error_code& ec) = 0;
    virtual bool remove(const path& p, error_code& ec) = 0;
    virtual std::uintmax_t remove_all(const path& p, error_code& ec) = 0;

    virtual std::uintmax_t hard_link_count(error_code& ec) = 0;
    virtual space_info space(error_code& ec) = 0;

    virtual void set_special_data(const path& p, const std::shared_ptr<special_data>& data,
                                  error_code& ec) = 0;
    virtual std::shared_ptr<special_data> get_special_data(const path& p, error_code& ec) = 0;
};

} // namespace vfs
} // namespace aliceVision
