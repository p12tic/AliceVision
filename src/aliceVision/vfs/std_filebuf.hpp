// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "generic_filebuf.hpp"
#include <fstream>

namespace aliceVision {
namespace vfs {

/// Wraps std::filebuf interface to generic_filebuf
class std_filebuf : public generic_filebuf {
public:
    std_filebuf();
    ~std_filebuf() override;

    bool is_open() const override;
    std_filebuf* open(const char* s, std::ios_base::openmode mode) override;
    std_filebuf* open(const std::string& str, std::ios_base::openmode mode) override;
    std_filebuf* close() override;

protected:
    void imbue(const std::locale& loc) override;
    std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which) override;
    pos_type seekpos(pos_type pos,
                     std::ios_base::openmode which) override;
    int sync() override;
    std::streamsize showmanyc() override;
    int_type underflow() override;
    int_type uflow() override;
    std::streamsize xsgetn(char_type* s, std::streamsize n) override;

    std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    int_type overflow(int_type c) override;
    int_type pbackfail(int_type c) override;
private:
    class public_std_filebuf_wrapper : public std::filebuf {
    public:
        friend class std_filebuf;
    };

    public_std_filebuf_wrapper _filebuf;
};

} // namespace vfs
} // namespace aliceVision
