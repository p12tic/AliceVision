// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// Copyright (c) 2012 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "cameraCommon.hpp"
#include <boost/detail/bitmask.hpp>
#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <algorithm>
#include <vector>

namespace aliceVision {
namespace camera {

std::string EINTRINSIC_enumToString(EINTRINSIC intrinsic)
{
    switch (intrinsic)
    {
        case EINTRINSIC::PINHOLE_CAMERA: return "pinhole";
        case EINTRINSIC::PINHOLE_CAMERA_RADIAL1: return "radial1";
        case EINTRINSIC::PINHOLE_CAMERA_RADIAL3: return "radial3";
        case EINTRINSIC::PINHOLE_CAMERA_3DERADIAL4: return "3deradial4";
        case EINTRINSIC::PINHOLE_CAMERA_BROWN: return "brown";
        case EINTRINSIC::PINHOLE_CAMERA_FISHEYE: return "fisheye4";
        case EINTRINSIC::PINHOLE_CAMERA_FISHEYE1: return "fisheye1";
        case EINTRINSIC::PINHOLE_CAMERA_3DEANAMORPHIC4: return "3deanamorphic4";
        case EINTRINSIC::PINHOLE_CAMERA_3DECLASSICLD: return "3declassicld";
        case EINTRINSIC::EQUIDISTANT_CAMERA: return "equidistant";
        case EINTRINSIC::EQUIDISTANT_CAMERA_RADIAL3: return "equidistant_r3";
        case EINTRINSIC::UNKNOWN:
        case EINTRINSIC::VALID_PINHOLE:
        case EINTRINSIC::VALID_EQUIDISTANT:
        case EINTRINSIC::VALID_CAMERA_MODEL:
            break;
    }
    throw std::out_of_range("Invalid Intrinsic Enum");
}

EINTRINSIC EINTRINSIC_stringToEnum(const std::string& intrinsic)
{
    std::string type = intrinsic;
    std::transform(type.begin(), type.end(), type.begin(), ::tolower); //tolower

    if (type == "pinhole") return EINTRINSIC::PINHOLE_CAMERA;
    if (type == "radial1") return EINTRINSIC::PINHOLE_CAMERA_RADIAL1;
    if (type == "radial3") return EINTRINSIC::PINHOLE_CAMERA_RADIAL3;
    if (type == "3deradial4") return EINTRINSIC::PINHOLE_CAMERA_3DERADIAL4;
    if (type == "brown") return EINTRINSIC::PINHOLE_CAMERA_BROWN;
    if (type == "fisheye4") return EINTRINSIC::PINHOLE_CAMERA_FISHEYE;
    if (type == "fisheye1") return EINTRINSIC::PINHOLE_CAMERA_FISHEYE1;
    if (type == "3deanamorphic4") return EINTRINSIC::PINHOLE_CAMERA_3DEANAMORPHIC4;
    if (type == "3declassicld") return EINTRINSIC::PINHOLE_CAMERA_3DECLASSICLD;
    if (type == "equidistant") return EINTRINSIC::EQUIDISTANT_CAMERA;
    if (type == "equidistant_r3") return EINTRINSIC::EQUIDISTANT_CAMERA_RADIAL3;

    throw std::out_of_range(intrinsic);
}

std::ostream& operator<<(std::ostream& os, EINTRINSIC e)
{
    return os << EINTRINSIC_enumToString(e);
}

std::istream& operator>>(std::istream& in, EINTRINSIC& e)
{
    std::string token;
    in >> token;
    e = EINTRINSIC_stringToEnum(token);
    return in;
}

EINTRINSIC EINTRINSIC_parseStringToBitmask(const std::string& str, const std::string& joinChar)
{
    std::vector<std::string> intrinsicsVec;
    boost::split(intrinsicsVec, str, boost::is_any_of(joinChar));
    if (!intrinsicsVec.size())
    {
        throw std::invalid_argument("'" + str + "'can't be parsed to EINTRINSIC Bitmask.");
    }
    EINTRINSIC mask = EINTRINSIC_stringToEnum(intrinsicsVec[0]);
    for (const std::string& intrinsic : intrinsicsVec)
    {
        mask |= EINTRINSIC_stringToEnum(intrinsic);
    }
    return mask;
}

} // namespace camera
} // namespace aliceVision
