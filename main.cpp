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
        ("output-dirs,o", boost::program_options::value< std::vector<std::string> >(), "Where patches will be written to")
        ("input-dirs,i",  boost::program_options::value< std::vector<std::string> >(), "Directories with images and their exclusion data.");
    boost::program_options::variables_map vars;
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
            .options(desc)
            .style(boost::program_options::command_line_style::unix_style)
            .run(),
        vars);
    boost::program_options::notify(vars);

    if (!vars.count(OUTPUT) && !vars.count(INPUT))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    std::vector<boost::filesystem::path> outputFolders;
    std::vector<boost::filesystem::path> inputMarkings;

    {//this is needed because https://svn.boost.org/trac/boost/ticket/8535
        const std::vector<std::string> outputFoldersString = vars[OUTPUT].as< std::vector<std::string> >();
        const std::vector<std::string> inputFoldersString = vars[INPUT].as<  std::vector<std::string> >();
        for(std::vector<std::string>::const_iterator it = outputFoldersString.begin(); it != outputFoldersString.end(); ++it)
        {
            boost::filesystem::path p(*it);
            outputFolders.push_back(p);
        }
        for(std::vector<std::string>::const_iterator it = inputFoldersString.begin(); it != inputFoldersString.end(); ++it)
        {
            boost::filesystem::path p(*it);
            inputMarkings.push_back(p);
        }
    }



    PatchExtractor extractor(outputFolders);

    std::vector<boost::filesystem::path>::const_iterator it = inputMarkings.begin();
    const std::vector<boost::filesystem::path>::const_iterator end = inputMarkings.end();
    for(;it != end; ++it) //for each directory...
    {
        std::cout << "Processing images from " << *it << std::endl;
        Markings markings;
        markings.load(it->native());

        if ( !extractor.extract(markings) )
        {
            break;
        }
    }

    std::cout << "Done processing.\n";

    return 0;
}
