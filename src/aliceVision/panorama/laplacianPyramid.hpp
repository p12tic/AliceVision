#pragma once

#include "imageOps.hpp"

#include <aliceVision/image/all.hpp>
#include <mutex>

namespace aliceVision
{

class LaplacianPyramid
{
public:
    struct InputInfo 
    {
        aliceVision::image::Image<image::RGBfColor> color; 
        aliceVision::image::Image<float> mask;
        aliceVision::image::Image<float> weights;
        int offsetX;
        int offsetY;
    };

public:
    LaplacianPyramid(size_t base_width, size_t base_height, size_t max_levels);

    virtual ~LaplacianPyramid();

    bool initialize();
    
    bool apply(aliceVision::image::Image<image::RGBfColor>& source,
               aliceVision::image::Image<float>& mask, 
               aliceVision::image::Image<float>& weights,
               const BoundingBox &outputBoundingBox, const BoundingBox &contentBoudingBox);

    bool merge(const aliceVision::image::Image<image::RGBfColor>& oimg, 
               const aliceVision::image::Image<float>& oweight,
               size_t level, int offset_x, int offset_y);

    bool rebuild(image::Image<image::RGBAfColor>& output, const BoundingBox & roi);

private:
    int _baseWidth;
    int _baseHeight;
    int _maxLevels;
    std::mutex _merge_mutex;

    std::vector<image::Image<image::RGBfColor>> _levels;
    std::vector<image::Image<float>> _weights;
    std::vector<InputInfo> _inputInfos;
};

} // namespace aliceVision
