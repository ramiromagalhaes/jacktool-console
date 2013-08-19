#ifndef PATCHEXTRACTOR_H
#define PATCHEXTRACTOR_H

#include <vector>
#include <boost/filesystem/path.hpp>

#include <../jacktool-common/markings.h>

class PatchExtractor
{
public:
    PatchExtractor(std::vector<boost::filesystem::path> &outputFolders_);
    bool extract(Markings & markings);

private:
    bool writeImage(const std::string & filename, const cv::Mat & image);
    bool createFolderAsNeeded(boost::filesystem::path &dir);

    bool rotate90,rotate180,rotate270;
    int patchHeight, patchWidth;

    std::vector<boost::filesystem::path>::const_iterator currentDir;
    const std::vector<boost::filesystem::path>::const_iterator end;
    const std::vector<boost::filesystem::path> outputFolders;
};

#endif // PATCHEXTRACTOR_H
