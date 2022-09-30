// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <streambuf>

namespace aliceVision {
namespace vfs {

/**
 * Enhances std::streambuf with an interface that can be implemented with std::streambuf and
 * similar classes.
 */
class generic_filebuf : public std::streambuf {
public:

    virtual bool is_open() const = 0;
    virtual generic_filebuf* open(const char* s, std::ios_base::openmode mode) = 0;
    virtual generic_filebuf* open(const std::string& str, std::ios_base::openmode mode) = 0;
    virtual generic_filebuf* close() = 0;
};

} // namespace vfs
} // namespace aliceVision
