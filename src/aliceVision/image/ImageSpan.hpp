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
        _height{other.Height()}
    {}

    ImageSpan& operator=(const ImageSpan& other) = default;
    ImageSpan& operator=(const Image<T>& other)
    {
        _data = other.data();
        _width = other.Width();
        _height = other.Height();
    }

    const T* data() const { return _data; }
    T* data() { return _data; }

    int Width() const { return _width; }
    int Height() const { return _height; }
    int Depth() const { return sizeof(Tpixel); }
    int Channels() const { return NbChannels<Tpixel>::size; }
    std::size_t size() const { return _width * _height; }

    const T& operator()(int i) const { return _data[i]; }
    const T& operator()(int y, int x) const { return _data[y * _width + x]; }
    T& operator()(int i) { return _data[i]; }
    T& operator()(int y, int x) { return _data[y * _width + x]; }


private:
    T* _data = nullptr;
    int _width = 0;
    int _height = 0;
};

} // namespace image
} // namespace aliceVision
