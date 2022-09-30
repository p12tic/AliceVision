// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "IFilesystemTree.hpp"

namespace aliceVision {
namespace vfs {

class FilesystemTreeInMemory : public IFilesystemTree
{
public:
    FilesystemTreeInMemory();
    ~FilesystemTreeInMemory() override;

    std::unique_ptr<generic_filebuf> open(const path& p, std::ios_base::openmode mode) override;
    bool create_directory(const path& p, error_code& ec) override;

    path canonical(const path& p, const path& base, error_code& ec) override;
    path weakly_canonical(const path& p, error_code& ec) override;
    void rename(const path& from, const path& to, error_code& ec) override;

    std::uintmax_t file_size(const path& p, error_code& ec) override;
    file_status status(const path& p, error_code& ec) override;
    bool remove(const path& p, error_code& ec) override;
    std::uintmax_t remove_all(const path& p, error_code& ec) override;

    std::uintmax_t hard_link_count(error_code& ec) override;
    space_info space(error_code& ec) override;

    void set_special_data(const path& p, const std::shared_ptr<special_data>& data,
                          error_code& ec) override;
    std::shared_ptr<special_data> get_special_data(const path& p, error_code& ec) override;
private:
    struct Data;
    std::unique_ptr<Data> _d;
};


} // namespace vfs
} // namespace aliceVision
