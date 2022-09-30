// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/numeric/numeric.hpp>
#include <aliceVision/image/pixelTypes.hpp>
#include <aliceVision/image/Image.hpp>

namespace aliceVision
{
namespace image
{

/// A container for unowned image data. Supports only a small subset of algorithms.
template<class T>
class ImageSpan
{
    // TODO: investigate whether to reuse Eigen::Map type, just like Image wraps Eigen::Matrix
public:
    using Tpixel = T;

    ImageSpan() = default;
    ImageSpan(const ImageSpan& other) = default;
    ImageSpan(const Image<T>& other) :
        _data{const_cast<T*>(other.data())},
        _width{other.Width()},
        _height{other.Height()},
        _strideBytes{static_cast<int>(other.Width() * sizeof(T))}
    {}

    ImageSpan(T* data, int width, int height) :
        _data{data},
        _width{width},
        _height{height},
        _strideBytes{static_cast<int>(width * sizeof(T))}
    {
    }

    ImageSpan(T* data, int width, int height, int strideBytes) :
        _data{data},
        _width{width},
        _height{height},
        _strideBytes{strideBytes}
    {
    }

    ImageSpan& operator=(const ImageSpan& other) = default;
    ImageSpan& operator=(const Image<T>& other)
    {
        _data = other.data();
        _width = other.Width();
        _height = other.Height();
        _strideBytes = other.Width() * sizeof(T);
    }

    const T* data() const { return _data; }
    T* data() { return _data; }

    int Width() const { return _width; }
    int Height() const { return _height; }
    int Depth() const { return sizeof(Tpixel); }
    int Channels() const { return NbChannels<Tpixel>::size; }
    int StrideBytes() const { return _strideBytes; }
    std::size_t size() const { return _width * _height; }

    const T& operator()(int i) const { return _data[i]; }
    const T& operator()(int y, int x) const
    {
        const char* rowPtr = reinterpret_cast<const char*>(_data) + _strideBytes * y;
        return reinterpret_cast<const T*>(rowPtr)[x];
    }

    T& operator()(int i) { return _data[i]; }
    T& operator()(int y, int x)
    {
        const char* rowPtr = reinterpret_cast<char*>(_data) + _strideBytes * y;
        return reinterpret_cast<T*>(rowPtr)[x];
    }

private:
    T* _data = nullptr;
    int _width = 0;
    int _height = 0;
    int _strideBytes = 0;
};

} // namespace image
} // namespace aliceVision
