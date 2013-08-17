#include <string>

#include <boost/filesystem.hpp>

#include <QStringList>
#include <QDir>

#include "../jacktool-common/markings.h"



QStringList imagesInSourceFolder(std::string folder)
{
    QDir sourceFolder = QDir(folder.c_str());

    QStringList filter;
    filter.append("*.jpg");
    filter.append("*.png");
    filter.append("*.bmp");
    filter.append("*.pgm");
    filter.append("*.JPG");
    filter.append("*.PNG");
    filter.append("*.BMP");
    filter.append("*.PGM");

    return sourceFolder.entryList(filter);
}



int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        return 1;
    }


    boost::filesystem::path destinationFolder = argv[1];



    std::vector<std::string> directories;
    for (int i = 2; i < argc; ++i)
    {
        directories.push_back(argv[i]);
    }



    const PatchExtractorConfiguration cfg(destinationFolder);



    std::vector<std::string>::const_iterator it = directories.begin();
    const std::vector<std::string>::const_iterator end = directories.end();
    for(;it != end; ++it) //for each directory...
    {
        std::cout << "Processing images from " << *it << std::endl;
        Markings markings(*it);
        markings.processAll(cfg);
    }

    return 0;
}
