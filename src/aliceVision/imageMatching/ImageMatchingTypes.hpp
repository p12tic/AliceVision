// This file is part of the AliceVision project.
// Copyright (c) 2019 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/feature/Descriptor.hpp>
#include <map>
#include <set>
#include <vector>

namespace aliceVision {
namespace imageMatching {

static const int DIMENSION = 128;

using DescriptorFloat = feature::Descriptor<float, DIMENSION> ;
using DescriptorUChar = feature::Descriptor<unsigned char, DIMENSION> ;

using ImageID = std::size_t;

// just a list of doc id
using ListOfImageID = std::vector<ImageID>;

// An ordered and unique list of doc id
using OrderedListOfImageID = std::set<ImageID>;

// For each image ID it contains the  list of matching imagess
using PairList = std::map<ImageID, ListOfImageID>;

// For each image ID it contains the ordered list of matching images
using OrderedPairList = std::map<ImageID, OrderedListOfImageID>;


/**
 * @brief Function that prints a PairList
 * @param os The stream on which to print
 * @param pl The pair list
 * @return the stream
 */
std::ostream& operator<<(std::ostream& os, const PairList& pl);

/**
 * @brief Mode to select the type of image matching
 */
enum class EImageMatchingMethod
{
    EXHAUSTIVE = 0,
    VOCABULARYTREE = 1,
    SEQUENTIAL = 2,
    SEQUENTIAL_AND_VOCABULARYTREE = 3,
    FRUSTUM = 4,
    FRUSTUM_OR_VOCABULARYTREE = 5
};

/**
 *@brief convert an enum EImageMatchingMethod to its corresponding string
 */
std::string EImageMatchingMethod_enumToString(EImageMatchingMethod m);

/**
 * @brief convert a string treeMode to its corresponding enum treeMode
 * @param String
 * @return EImageMatchingMethod
 */
EImageMatchingMethod EImageMatchingMethod_stringToEnum(const std::string& m);

std::ostream& operator<<(std::ostream& os, EImageMatchingMethod m);
std::istream& operator>>(std::istream& in, EImageMatchingMethod& m);

/**
 * @brief Mode to combine image matching between two SfMDatas
 */
enum class EImageMatchingMode
{
    A_A_AND_A_B,
    A_AB,
    A_B,
    A_A
};

/**
 * @brief get informations about each EImageMatchingMode
 * @return String
 */
std::string EImageMatchingMode_description();

/**
 * @brief convert an enum EImageMatchingMode to its corresponding string
 * @param modeMultiSfM
 * @return String
 */
std::string EImageMatchingMode_enumToString(EImageMatchingMode modeMultiSfM);

/**
 * @brief convert a string modeMultiSfM to its corresponding enum modeMultiSfM
 * @param String
 * @return EImageMatchingMode
 */
EImageMatchingMode EImageMatchingMode_stringToEnum(const std::string& modeMultiSfM);

} // namespace imageMatching
} // namespace aliceVision
