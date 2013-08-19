#include "patchextractor.h"



PatchExtractor::PatchExtractor(std::vector<boost::filesystem::path> & outputFolders_) : rotate90(false), rotate180(false), rotate270(false),
                                                                                        patchHeight(20), //Pavani et al; 2010 defaults
                                                                                        patchWidth(20),  //Viola, Jones; 2004 is 24x24
                                                                                        outputFolders(outputFolders_),
                                                                                        currentDir(outputFolders.begin()),
                                                                                        end(outputFolders.end())
{
}

bool PatchExtractor::extract(Markings &markings)
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



bool PatchExtractor::writeImage(const std::string &filename, const cv::Mat &image)
{
    if ( !cv::imwrite(filename, image) )
    {
        std::cerr << "Failed to write patch at path " << filename << ".\n";
        return false;
    }

    return true;
}



bool PatchExtractor::createFolderAsNeeded(boost::filesystem::path &dir)
{
    if ( !boost::filesystem::exists(dir) )
    {
        try
        {
            boost::filesystem::create_directory(dir);
        }
        catch (boost::filesystem::filesystem_error &)
        {
            std::cerr << "Failed to create a directory " << dir.native() << ".\n";
            return false;
        }
    }

    return true;
}
