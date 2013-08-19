#include "patchextractor.h"
#include <boost/filesystem.hpp>



PatchExtractor::PatchExtractor(const std::vector<boost::filesystem::path> &outputFolders_) : rotate90(false), rotate180(false), rotate270(false),
                                                                                        patchHeight(20), //Pavani et al; 2010 defaults
                                                                                        patchWidth(20),  //Viola, Jones; 2004 is 24x24
                                                                                        outputFolders(outputFolders_),
                                                                                        currentOutputDir(outputFolders.begin()),
                                                                                        endOutputDir(outputFolders.end())
{
}

bool PatchExtractor::extract(Markings & markings)
{
    const boost::filesystem::path archivePath( markings.getBaseDirectory() );

    for(std::map<std::string, std::vector<Rectangle>*>::const_iterator map_iter = markings.imagesExclusionsMapBegin(); map_iter != markings.imagesExclusionsMapEnd(); ++map_iter) {
        const std::map<std::string, std::vector<Rectangle> *>::value_type v = *map_iter;
        const boost::filesystem::path imagePath = archivePath / v.first;
        const std::vector<Rectangle> * exclusions = v.second;



        const cv::Mat image = cv::imread(imagePath.native());
        if (!image.data)
        {
            std::cerr << "Could not open " << imagePath.native() << ".\n";
            return false;
        }



        int patch_counter = 0;

        //loops over the image trying to take patches from it
        for (int h = 0; h < image.size().height - patchHeight; h += patchHeight)
        {
            for (int w = 0; w < image.size().width - patchHeight; w += patchWidth)
            {
                const int h_big = h + patchHeight;
                const int w_big = w + patchWidth;

                bool intersects = false;

                for (std::vector<Rectangle>::const_iterator it = exclusions->begin(); it != exclusions->end(); ++it)
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

                //Extract the patch.
                const cv::Rect roi(w, h, patchWidth, patchHeight);
                cv::Mat patch(image, roi);
                cv::cvtColor(patch, patch, CV_BGR2GRAY);


                //Sets the patch base filename. The final name will depend on other processing options.
                std::stringstream patchBaseFilename;
                patchBaseFilename << imagePath.filename().native().c_str() << "-" << patch_counter;


                //Write the file somewhere
                if ( !createPathsAndWriteFile(imagePath.parent_path().filename(), patchBaseFilename.str(), patch) )
                {
                    return false;
                }

                if (rotate90)
                {
                    cv::Mat transp = patch.clone();
                    cv::transpose(patch, transp);
                    if ( !createPathsAndWriteFile(imagePath.parent_path().filename(), patchBaseFilename.str() + "-90", transp) )
                    {
                        return false;
                    }
                }
                if (rotate180)
                {
                    cv::Mat transp = patch.clone();
                    cv::transpose(patch, transp);
                    cv::transpose(transp, transp);
                    if ( !createPathsAndWriteFile(imagePath.parent_path().filename(), patchBaseFilename.str() + "-180", transp) )
                    {
                        return false;
                    }
                }
                if (rotate270)
                {
                    cv::Mat transp = patch.clone();
                    cv::transpose(patch, transp);
                    cv::transpose(transp, transp);
                    cv::transpose(transp, transp);
                    if ( !createPathsAndWriteFile(imagePath.parent_path().filename(), patchBaseFilename.str() + "-270", transp) )
                    {
                        return false;
                    }
                }

                patch_counter++;
            }
        }
    }

    return true;
}



bool PatchExtractor::createPathsAndWriteFile(const boost::filesystem::path & subdirectory, const std::string & fileName, const cv::Mat & patch)
{
    const std::string patchFileName = fileName + ".pgm";

    for (;currentOutputDir != endOutputDir; ++currentOutputDir)
    {
        const boost::filesystem::path dir = (*currentOutputDir) / subdirectory;

        if ( !boost::filesystem::exists(dir) )
        {
            try
            {
                boost::filesystem::create_directories(dir);
            }
            catch (boost::filesystem::filesystem_error & e)
            {
                std::cerr << "Failed to create directories " << dir.native() << ". Will try another.\n";
                continue;
            }
        }

        const boost::filesystem::path patchPath = dir / patchFileName;

        if ( cv::imwrite(patchPath.native(), patch) )
        {
            return true;
        }
        else
        {
            std::cerr << "Failed to write patch at path " << patchPath.native() << ". Will try another.\n";
            continue;
        }
    }

    std::cerr << "No more output directories to try to write the patch.\n";
    return false;
}
