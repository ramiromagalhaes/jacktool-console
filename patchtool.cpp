#include "patchtool.h"
#include <iostream>
#include <fstream>



PatchTool::PatchTool(std::string patchFile_, cv::Size size_ = DEFAULT_SIZE) : patchFile(patchFile_),
                                                                              patchSize(DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE),
                                                                              image(size_, CV_8UC1)
{
    imageFile = patchFile.parent_path() / (patchFile.filename() / ".pgm");
}



PatchTool::PatchTool(std::string patchFile_) : patchFile(patchFile_)
{
    {
        std::ifstream ifs;
        ifs.open(patchFile.native().c_str(), std::ifstream::in);

        if ( !ifs.is_open() )
        {
            throw "Fail: patch files does not exist!"; //TODO throw something
        }

        std::string imageFileAsString;
        ifs >> imageFileAsString
            >> patchSize.width
            >> patchSize.height
            >> patchCount;

        imageFile = imageFileAsString;

        ifs.close();
    }

    if ( !boost::filesystem::exists(imageFile) )
    {
        throw "Fail: image does not exist!"; //TODO throw something appropriate
    }

    if ( patchSize.height != image.rows ) //assertions
    {
        throw "Fail assertion!"; //TODO throw something appropriate
    }

    initializePatchExtractor();
}



void PatchTool::begin()
{
    patchExtractor.x = 0;
}



cv::Mat PatchTool::next()
{
    patchExtractor.x += patchSize.width;

    return image(patchExtractor);
}



bool PatchTool::append(cv::Mat patch)
{
    if (patch.cols != patchSize.width || patch.rows != patchSize.height || patch.type() != image.type())
    {
        return false;
    }

    if (patchCount >= image.cols / patchSize.width)
    {
        return false;
    }

    //adiciona o patch
    cv::Mat append_here(image,
                        cv::Rect(patchSize.width * patchCount,
                                 patchSize.height,
                                 patchSize.width,
                                 patchSize.height)
                        );
    patch.copyTo(append_here);

    patchCount++;

    return true;
}



bool PatchTool::save()
{
    std::ofstream ofs;
    ofs.open(patchFile.native().c_str(), std::ofstream::out | std::ofstream::trunc);

    if ( !ofs.is_open() )
    {
        return false;
    }

    ofs << imageFile.native()
        << patchSize.width
        << patchSize.height
        << patchCount;

    ofs.close();

    cv::imwrite(imageFile.native(), image);

    return true;
}



bool PatchTool::loadImage()
{
    image = cv::imread(imageFile.native(), CV_LOAD_IMAGE_GRAYSCALE);
    return image.data != 0;
}



void PatchTool::initializePatchExtractor()
{
    patchExtractor.x = 0;
    patchExtractor.y = 0;
    patchExtractor.width = patchSize.width;
    patchExtractor.height = patchSize.height;
}

