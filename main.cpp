#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <QStringList>
#include <QDir>

#include "../jacktool-common/markings.h"
#include "patchextractor.h"



#define INPUT "input-dirs"
#define OUTPUT "output-dirs"



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
    boost::program_options::options_description desc("Usage:");
    desc.add_options()
        ("output-dirs,o", boost::program_options::value< std::vector<boost::filesystem::path> >(), "Where patches will be written to")
        ("input-dirs,i",  boost::program_options::value< std::vector<boost::filesystem::path> >(), "Directories with images and their exclusion data.")
    ;
    boost::program_options::variables_map vars;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vars);
    boost::program_options::notify(vars);

    if (!vars.count(OUTPUT) && !vars.count(INPUT))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    const std::vector<boost::filesystem::path> outputFolders = vars[OUTPUT].as< std::vector<boost::filesystem::path> >();
    const std::vector<boost::filesystem::path> inputFolders = vars[INPUT].as<  std::vector<boost::filesystem::path> >();



    PatchExtractor extractor(outputFolders);

    std::vector<boost::filesystem::path>::const_iterator it = inputFolders.begin();
    const std::vector<boost::filesystem::path>::const_iterator end = inputFolders.end();
    for(;it != end; ++it) //for each directory...
    {
        std::cout << "Processing images from " << *it << std::endl;
        Markings markings(it->native());

        if ( !extractor.extract(markings) )
        {
            break;
        }
    }

    return 0;
}
