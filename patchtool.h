#ifndef PATCHTOOL_H
#define PATCHTOOL_H

#include <boost/filesystem.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

#define DEFAULT_PATCH_SIZE 20
#define DEFAULT_SIZE cv::Size(DEFAULT_PATCH_SIZE * 4000 * 4000, DEFAULT_PATCH_SIZE)

class PatchTool
{
public:

    /**
     * @brief Creates a new one.
     * @param size
     * @param patchFile
     */
    PatchTool(std::string patchFile_, cv::Size size_);

    /**
     * @brief Loads an existing patch data file.
     * @param patchFile
     */
    PatchTool(std::string patchFile_);

    /**
     * @brief begin Resets the internal iterator
     */
    void begin();

    /**
     * @brief Gets the next patch.
     */
    cv::Mat next();

    /**
     * @brief Appends a new patch to the end of the patch file. Returns false if it fails.
     */
    bool append(cv::Mat patch);

    /**
     * @brief Writes things on disk.
     * @return
     */
    bool save();

private:
    bool loadImage();
    void updatePatchCount();
    void initializePatchExtractor();

    boost::filesystem::path patchFile;
    boost::filesystem::path imageFile;

    cv::Size patchSize;
    int patchCount;
    cv::Mat image;

    //used by iterator methods such as begin() and next()
    cv::Rect patchExtractor;
};

#endif // PATCHTOOL_H
