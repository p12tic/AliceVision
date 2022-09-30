// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "std_filebuf.hpp"

namespace aliceVision {
namespace vfs {

std_filebuf::std_filebuf(const std::string& str, std::ios_base::openmode mode)
{
    _filebuf.open(str, mode);
}

std_filebuf::~std_filebuf() = default;

bool std_filebuf::is_open() const
{
    return _filebuf.is_open();
}

std_filebuf* std_filebuf::close()
{
    if (_filebuf.close())
        return this;
    return nullptr;
}

void std_filebuf::imbue(const std::locale& loc)
{
    _filebuf.imbue(loc);
}

std::streambuf* std_filebuf::setbuf(char_type* s, std::streamsize n)
{
    _filebuf.setbuf(s, n);
    return this;
}

std::filebuf::pos_type std_filebuf::seekoff(off_type off, std::ios_base::seekdir dir,
                                            std::ios_base::openmode which)
{
    return _filebuf.seekoff(off, dir, which);
}

std_filebuf::pos_type std_filebuf::seekpos(pos_type pos,
                 std::ios_base::openmode which)
{
    return _filebuf.seekpos(pos, which);
}

int std_filebuf::sync()
{
    return _filebuf.sync();
}

std::streamsize std_filebuf::showmanyc()
{
    return _filebuf.showmanyc();
}

std_filebuf::int_type std_filebuf::underflow()
{
    return _filebuf.underflow();
}

std_filebuf::int_type std_filebuf::uflow()
{
    return _filebuf.uflow();
}

std::streamsize std_filebuf::xsgetn(char_type* s, std::streamsize n)
{
    return _filebuf.xsgetn(s, n);
}

std::streamsize std_filebuf::xsputn(const char_type* s, std::streamsize n)
{
    return _filebuf.xsputn(s, n);
}

std_filebuf::int_type std_filebuf::overflow(int_type c)
{
    return _filebuf.overflow(c);
}

std_filebuf::int_type std_filebuf::pbackfail(int_type c)
{
    return _filebuf.pbackfail(c);
}

} // namespace vfs
} // namespace aliceVision
