// This file is part of the AliceVision project.
// Copyright (c) 2019 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "ImageMatchingTypes.hpp"
#include <aliceVision/feature/Descriptor.hpp>
#include <aliceVision/sfmData/SfMData.hpp>
#include <aliceVision/voctree/Database.hpp>
#include <aliceVision/voctree/VocabularyTree.hpp>

namespace aliceVision {
namespace imageMatching {

/**
 * It processes a pairlist containing all the matching images for each image ID and return
 * a similar list limited to a numMatches number of matching images and such that
 * there is no repetitions: eg if the image 1 matches with 2 in the list of image 2
 * there won't be the image 1
 *
 * @param[in] allMatches A pairlist containing all the matching images for each image of the dataset
 * @param[in] numMatches The maximum number of matching images to consider for each image (if 0, consider all matches)
 * @param[out] matches A processed version of allMatches that consider only the first numMatches without repetitions
 */
void convertAllMatchesToPairList(const PairList &allMatches, std::size_t numMatches,
                                 OrderedPairList &outPairList);

void generateSequentialMatches(const sfmData::SfMData& sfmData, size_t nbMatches,
                               OrderedPairList& outPairList);
void generateAllMatchesInOneMap(const std::set<IndexT>& viewIds, OrderedPairList& outPairList);
void generateAllMatchesBetweenTwoMap(const std::set<IndexT>& viewIdsA,
                                     const std::set<IndexT>& viewIdsB,
                                     OrderedPairList& outPairList);

void generateFromVoctree(PairList& allMatches,
                         const std::map<IndexT, std::string>& descriptorsFiles,
                         const voctree::Database& db,
                         const voctree::VocabularyTree<DescriptorFloat>& tree,
                         EImageMatchingMode modeMultiSfM,
                         std::size_t nbMaxDescriptors,
                         std::size_t numImageQuery);

void conditionVocTree(const std::string& treeName, bool withWeights,
                      const std::string& weightsName,
                      const EImageMatchingMode matchingMode,
                      const std::vector<std::string>& featuresFolders,
                      const sfmData::SfMData& sfmDataA,
                      std::size_t nbMaxDescriptors,
                      const std::string& sfmDataFilenameA,
                      const sfmData::SfMData& sfmDataB,
                      const std::string& sfmDataFilenameB,
                      bool useMultiSfM,
                      const std::map<IndexT, std::string>& descriptorsFilesA,
                      std::size_t numImageQuery,
                      OrderedPairList& selectedPairs);

EImageMatchingMethod selectImageMatchingMethod(EImageMatchingMethod method,
                                               const sfmData::SfMData& sfmDataA,
                                               const sfmData::SfMData& sfmDataB,
                                               std::size_t minNbImages);

} // namespace imageMatching
} // namespace aliceVision
