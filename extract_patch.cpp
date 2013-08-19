#include "markings.h"
#include "extract_patch.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <iostream>



void processAll(const Markings & markings, const PatchExtractorConfiguration &cfg)
{
    boost::filesystem::path archivePath( markings.getBaseDirectory() );

    for(std::map<std::string, std::vector<Rectangle>*>::const_iterator it = markings.exclusionsBegin(); it != markings.exclusionsEnd(); ++it) {
        const std::map<std::string, std::vector<Rectangle> *>::value_type v = *it;

        const boost::filesystem::path file = archivePath / v.first;

        if ( !extract_patches(file, *v.second, cfg) )
        {
            return;
        }
    }
}



void writeImage(const std::string & filename, const cv::Mat & image)
{
    if ( !cv::imwrite(filename, image) )
    {
        std::cerr << "Failed to write patch at path " << filename << std::endl;
    }
}



bool createFolderAsNeeded(boost::filesystem::path &dir)
{
    if ( !boost::filesystem::exists(dir) )
    {
        try
        {
            boost::filesystem::create_directory(dir);
        }
        catch (boost::filesystem::filesystem_error &)
        {
            std::cerr << "Failed to create a directory " << dir.native() << ". Halting the image processing." << std::endl;
            return false;
        }
    }

    return true;
}



bool varianceNormalize(cv::Mat & patch, cv::Mat & normalized)
{
    if (normalized.type() != CV_64F || patch.cols != normalized.cols || patch.rows != normalized.rows)
    {
        return false;
    }

    cv::Mat integralImage(patch.cols + 1, patch.rows + 1, CV_32S);
    cv::Mat squareIntegralImage(patch.cols + 1, patch.rows + 1, CV_32S);
    cv::integral(patch, integralImage, squareIntegralImage, CV_32S);


    const double area = patch.rows * patch.cols;
    const double subwindowMean = (integralImage.at<int>(0, 0)
                                    - integralImage.at<int>(0, patch.cols)
                                    - integralImage.at<int>(patch.rows, 0)
                                    + integralImage.at<int>(patch.rows, patch.cols)) / area;
    const double subwindowStdDeviation = sqrt(
        subwindowMean * subwindowMean - (squareIntegralImage.at<int>(0, 0)
                                            - squareIntegralImage.at<int>(0, patch.cols)
                                            - squareIntegralImage.at<int>(patch.rows, 0)
                                            + squareIntegralImage.at<int>(patch.rows, patch.cols)
                                         ) / area);

    for(int j = 0; j < patch.rows; j++)
    {
        for (int i = 0; i < patch.cols; i++)
        {
            normalized.at<double>(i, j) = (patch.at<unsigned char>(i, j) - subwindowMean) / subwindowStdDeviation;
        }
    }

    return true;
}



bool extract_patches(const boost::filesystem::path &image_path,
                     const std::vector<Rectangle> &exclusions,
                     const PatchExtractorConfiguration &cfg)
{
    cv::Mat image;
    image = cv::imread(image_path.native());
    if (!image.data)
    {
        std::cerr << "Could not open " << image_path.native() << std::endl;
        return false;
    }

    int patch_counter = 0;

    for (int h = 0; h < image.size().height - cfg.patchHeight; h += cfg.patchHeight)
    {
        for (int w = 0; w < image.size().width - cfg.patchHeight; w += cfg.patchWidth)
        {
            const int h_big = h + cfg.patchHeight;
            const int w_big = w + cfg.patchWidth;

            bool intersects = false;

            for (std::vector<Rectangle>::const_iterator it = exclusions.begin(); it != exclusions.end(); ++it)
            {
                const Rectangle e = *it;
                const int x = e.x;
                const int y = e.y;
                const int x_big = e.x + e.width;
                const int y_big = e.y + e.height;

                if ( ((x <= w && w <= x_big) && (y <= h && h <= y_big)) ||
                     ((w <= x && x <= w_big) && (h <= y && y <= h_big)) )
                {
                    intersects = true;
                    break;
                }
            }

            if (intersects)
            {
                continue;
            }



            //builds the patch path...
            boost::filesystem::path filePatchPath = cfg.destinationFolder / image_path.parent_path().filename();

            //...creates a folder where patches will be saved, if necessary...
            if ( !createFolderAsNeeded(filePatchPath) )
            {
                return false;
            }

            //..sets the basic patch filename
            filePatchPath /= image_path.filename();
            std::stringstream ss;
            ss << filePatchPath.native().c_str() << "-" << patch_counter++;

            //...extract the patch...
            cv::Rect roi(w, h, cfg.patchWidth, cfg.patchHeight);
            cv::Mat patch(image, roi);
            cv::cvtColor(patch, patch, CV_BGR2GRAY);

            //TODO how will I deal with the variance normalization of the patches?
            //See (Viola and Jones, 2004, section 5.4). See also varianceNormalize method in this file

            //...and writes it on file.
            std::string filename = ss.str() + ".pgm";
            writeImage(filename, patch);

            if (cfg.rotate90)
            {
                filename = ss.str() + "-90.pgm";
                cv::Mat transp = patch.clone();
                cv::transpose(patch, transp);
                writeImage(filename, transp);
            }
            if (cfg.rotate180)
            {
                filename = ss.str() + "-180.pgm";
                cv::Mat transp = patch.clone();
                cv::transpose(patch, transp);
                cv::transpose(transp, transp);
                writeImage(filename, transp);
            }
            if (cfg.rotate270)
            {
                filename = ss.str() + "-270.pgm";
                cv::Mat transp = patch.clone();
                cv::transpose(patch, transp);
                cv::transpose(transp, transp);
                cv::transpose(transp, transp);
                writeImage(filename, transp);
            }
        }
    }

    return true;
}
