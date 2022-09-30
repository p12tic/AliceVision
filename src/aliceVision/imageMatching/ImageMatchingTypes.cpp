// This file is part of the AliceVision project.
// Copyright (c) 2019 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ImageMatchingTypes.hpp"
#include <aliceVision/voctree/databaseIO.hpp>
#include <aliceVision/system/ParallelFor.hpp>

namespace aliceVision {
namespace imageMatching {

std::ostream& operator<<(std::ostream& os, const PairList& pl)
{
    for (PairList::const_iterator plIter = pl.begin(); plIter != pl.end(); ++plIter)
    {
        os << plIter->first;
        for (ImageID id : plIter->second)
        {
            os << " " << id;
        }
        os << "\n";
    }
    return os;
}

std::string EImageMatchingMethod_enumToString(EImageMatchingMethod m)
{
    switch(m)
    {
        case EImageMatchingMethod::EXHAUSTIVE:
            return "Exhaustive";
        case EImageMatchingMethod::VOCABULARYTREE:
            return "VocabularyTree";
        case EImageMatchingMethod::SEQUENTIAL:
            return "Sequential";
        case EImageMatchingMethod::SEQUENTIAL_AND_VOCABULARYTREE:
            return "SequentialAndVocabularyTree";
        case EImageMatchingMethod::FRUSTUM:
            return "Frustum";
        case EImageMatchingMethod::FRUSTUM_OR_VOCABULARYTREE:
            return "FrustumOrVocabularyTree";
    }
    throw std::out_of_range("Invalid EImageMatchingMethod enum: " + std::to_string(int(m)));
}

EImageMatchingMethod EImageMatchingMethod_stringToEnum(const std::string& m)
{
    std::string mode = m;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

    if (mode == "exhaustive")
        return EImageMatchingMethod::EXHAUSTIVE;
    if (mode == "vocabularytree")
        return EImageMatchingMethod::VOCABULARYTREE;
    if (mode == "sequential")
        return EImageMatchingMethod::SEQUENTIAL;
    if (mode == "sequentialandvocabularytree")
        return EImageMatchingMethod::SEQUENTIAL_AND_VOCABULARYTREE;
    if (mode == "frustum")
        return EImageMatchingMethod::FRUSTUM;
    if (mode == "frustumorvocabularytree")
        return EImageMatchingMethod::FRUSTUM_OR_VOCABULARYTREE;

    throw std::out_of_range("Invalid EImageMatchingMethod: " + m);
}

std::ostream& operator<<(std::ostream& os, EImageMatchingMethod m)
{
    return os << EImageMatchingMethod_enumToString(m);
}

std::istream& operator>>(std::istream& in, EImageMatchingMethod& m)
{
    std::string token;
    in >> token;
    m = EImageMatchingMethod_stringToEnum(token);
    return in;
}

std::string EImageMatchingMode_description()
{
    return "The mode to combine image matching between the input SfMData A and B: \n"
               "* a/a+a/b : A with A + A with B\n"
               "* a/ab    : A with A and B\n"
               "* a/b     : A with B\n"
               "* a/a     : A with A";
}


std::string EImageMatchingMode_enumToString(EImageMatchingMode modeMultiSfM)
{
    switch(modeMultiSfM)
    {
        case EImageMatchingMode::A_A_AND_A_B: return "a/a+a/b";
        case EImageMatchingMode::A_AB:        return "a/ab";
        case EImageMatchingMode::A_B:         return "a/b";
        case EImageMatchingMode::A_A:         return "a/a";
    }
    throw std::out_of_range("Invalid modeMultiSfM enum");
}

EImageMatchingMode EImageMatchingMode_stringToEnum(const std::string& modeMultiSfM)
{
    std::string mode = modeMultiSfM;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower); //tolower

    if (mode == "a/a+a/b") return EImageMatchingMode::A_A_AND_A_B;
    if (mode == "a/ab")    return EImageMatchingMode::A_AB;
    if (mode == "a/b")     return EImageMatchingMode::A_B;
    if (mode == "a/a")     return EImageMatchingMode::A_A;

    throw std::out_of_range("Invalid modeMultiSfM : " + modeMultiSfM);
}

} // namespace imageMatching
} // namespace aliceVision
