//----------------------------------------------------------------------------
/** @file FuegoMain.cpp
    Main function for Fuego */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <iostream>
#include <filesystem>

#include <boost/format.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "FuegoEngine.hpp"

//#include "FuegoMainEngine.h"
#include "FuegoMainUtil.h"
//#include "GoInit.h"
#include "SgDebug.h"
#include "SgException.h"
//#include "SgInit.h"
#include "SgPlatform.h"

namespace fs = std::filesystem;
namespace po = boost::program_options;

//----------------------------------------------------------------------------

namespace {

/** @name Settings from command line options */
// @{



/** Allow handicap games */

bool g_quiet = false;

std::vector<std::string> g_inputFiles;

// @} // @name

/** Get program directory from program path.
    @param programPath Program path taken from @c argv[0] in
    @c main. According to ANSI C, this can be @c 0. */
fs::path GetProgramDir(const char* programPath)
{
    if (!programPath)
        return "";

    return fs::path(programPath).parent_path();
}

fs::path GetTopSourceDir()
{
    #ifdef ABS_TOP_SRCDIR
        return fs::path(ABS_TOP_SRCDIR);
    #else
        return "";
    #endif
}

void Help(po::options_description& desc, std::ostream& out)
{
    out << "Usage: fuego [options] [input files]\n" << desc << "\n";
    exit(0);
}

void ParseOptions(fuego_engine_configuration& cfg, int argc, char** argv)
{
    po::options_description normalOptions("Options");
    normalOptions.add_options()
        ("config", 
            po::value<std::string>(&cfg.configPath)->value_name("_")->default_value(""),
         "execute GTP commands from file before starting main command loop")
        ("help", "Displays this help and exit")
        ("maxgames", 
         po::value<int>(&cfg.maxGames)->default_value(-1),
         "make clear_board fail after n invocations")
        ("nobook", "don't automatically load opening book")
        ("nohandicap", "don't support handicap commands")
        ("quiet", "don't print debug messages")
        ("srand", 
         po::value<int>(&cfg.srand)->default_value(0),
         "set random seed (-1:none, 0:time(0))")
        ("size", 
         po::value<int>(&cfg.fixedBoardSize)->default_value(0),
         "initial (and fixed) board size");
    po::options_description hiddenOptions;
    hiddenOptions.add_options()
        ("input-file", po::value<std::vector<std::string> >(&g_inputFiles),
         "input file");
    po::options_description allOptions;
    allOptions.add(normalOptions).add(hiddenOptions);
    po::positional_options_description positionalOptions;
    positionalOptions.add("input-file", -1);
    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(allOptions).
                                     positional(positionalOptions).run(), vm);
        po::notify(vm);
    }
    catch (...)
    {
        Help(normalOptions, std::cerr);
    }
    if (vm.count("help"))
        Help(normalOptions, std::cout);
    if (vm.count("nobook"))
        cfg.useBook = false;
    if (vm.count("nohandicap"))
        cfg.allowHandicap = false;
    if (vm.count("quiet"))
        g_quiet = true;
}

void PrintStartupMessage()
{
    SgDebug() <<
        "Fuego " << FuegoMainUtil::Version() << "\n"
        "Copyright (C) 2009-2013 by the authors of the Fuego project.\n"
        "This program comes with ABSOLUTELY NO WARRANTY. This is\n"
        "free software and you are welcome to redistribute it under\n"
        "certain conditions. Type `fuego-license' for details.\n\n";
}

} // namespace

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    fuego_engine_configuration engine_cfg;
    if (argc > 0 && argv != 0)
    {
        engine_cfg.programPath = argv[0];
        SgPlatform::SetProgramDir(GetProgramDir(argv[0]));
        SgPlatform::SetTopSourceDir(GetTopSourceDir());
        try
        {
            ParseOptions(engine_cfg, argc, argv);
        }
        catch (const SgException& e)
        {
            SgDebug() << e.what() << "\n";
            return 1;
        }
    }
    if (g_quiet)
        SgDebugToNull();
    try
    {
        PrintStartupMessage();

        FuegoEngine feng{ engine_cfg };

        if (! g_inputFiles.empty())
        {
            for (size_t i = 0; i < g_inputFiles.size(); i++)
            {
                std::string file = g_inputFiles[i];
                std::ifstream fin(file.c_str());
                if (! fin)
                    throw SgException(boost::format("Error file '%1%'") 
                    				  % file);
                GtpInputStream in(fin);
                GtpOutputStream out(std::cout);
                feng.engine().MainLoop(in, out);
            }
        }
        else
        {
            GtpInputStream in(std::cin);
            GtpOutputStream out(std::cout);
            feng.engine().MainLoop(in, out);
        }
    }
    catch (const GtpFailure& e)
    {
        SgDebug() << e.Response() << '\n';
        return 1;
    }
    catch (const std::exception& e)
    {
        SgDebug() << e.what() << '\n';
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------
