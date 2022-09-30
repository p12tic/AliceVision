// This file is part of the AliceVision project.
// Copyright (c) 2016 AliceVision contributors.
// Copyright (c) 2012 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/camera/cameraCommon.hpp>
#include <aliceVision/camera/IntrinsicBase.hpp>
#include <memory>

namespace aliceVision {
namespace camera {

std::shared_ptr<IntrinsicBase> createIntrinsic(EINTRINSIC intrinsicType,
    unsigned int w = 0, unsigned int h = 0,
    double focalLengthPixX = 0.0, double focalLengthPixY = 0.0,
    double offsetX = 0.0, double offsetY = 0.0);

} // namespace camera
} // namespace aliceVision
