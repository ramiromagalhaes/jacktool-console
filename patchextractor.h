#ifndef PATCHEXTRACTOR_H
#define PATCHEXTRACTOR_H

#include <vector>
#include <boost/filesystem/path.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <../jacktool-common/markings.h>

class PatchExtractor
{
public:
    PatchExtractor(const std::vector<boost::filesystem::path> &outputFolders_);
    bool extract(Markings & markings);

private:
    bool createPathsAndWriteFile(const boost::filesystem::path &subdirectory, const std::string &fileName, const cv::Mat &patch);

    const bool rotate90,rotate180,rotate270;
    const int patchHeight, patchWidth;

    const std::vector<boost::filesystem::path> outputFolders;
    std::vector<boost::filesystem::path>::const_iterator currentOutputDir;
    const std::vector<boost::filesystem::path>::const_iterator endOutputDir;
};

#endif // PATCHEXTRACTOR_H
