#ifndef EXTRACT_PATCH_H
#define EXTRACT_PATCH_H

#include <vector>
#include <boost/filesystem.hpp>

#include "../jacktool-common/Rectangle.h"

bool extract_patches(const boost::filesystem::path &image_path,
                     const std::vector<Rectangle> &exclusions,
                     const PatchExtractorConfiguration &cfg);

#endif // EXTRACT_PATCH_H
