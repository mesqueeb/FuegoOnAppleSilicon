//
// Created by Alexander Pototskiy on 03.06.24.
//

#include "SgSystem.h"
#include "FuegoEngine.hpp"

#include <filesystem>
#include <fstream>

#include "GoInit.h"
#include "SgInit.h"

#include "GoUctCommands.h"
#include "GoSafetyCommands.h"
#include "GoUctBookBuilderCommands.h"
#include "GoGtpCommandUtil.h"
#include "GoUctPlayer.h"
#include "SgStringUtil.h"
#include "SgDebug.h"
#include "SgPlatform.h"

namespace fs = std::filesystem;

class FuegoEngineImpl : public GoGtpEngine
{
    using PlayerType = GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
        GoUctPlayoutPolicyFactory<GoUctBoard>>,
        GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard>>>;

    GoUctCommands m_uctCommands;
    GoSafetyCommands m_safetyCommands;
    GoUctBookBuilderCommands<PlayerType> m_autoBookCommands;

public:
    FuegoEngineImpl(fuego_engine_configuration const& cfg)
        : GoGtpEngine(cfg.fixedBoardSize, cfg.programPath.c_str(), false, !cfg.allowHandicap)
        , m_uctCommands(Board(), m_player, Game())
        , m_autoBookCommands(Board(), m_player, m_autoBook)
        , m_safetyCommands(Board())
    {
        m_uctCommands.Register(*this);
        m_safetyCommands.Register(*this);
        m_autoBookCommands.Register(*this);
        Register("fuego-license", &FuegoEngineImpl::CmdLicense, this);
        SetPlayer(new PlayerType(Board()));

        if (cfg.maxGames >= 0)
            SetMaxClearBoard(cfg.maxGames);

        if (cfg.useBook) {
            LoadBook(Book(), SgPlatform::GetProgramDir());
        }

        if (!cfg.configPath.empty()) {
            ExecuteFile(cfg.configPath);
        }
    }

    void LoadBook(GoBook& book, const fs::path& programDir)
    {
        const std::string fileName = "book.dat";
#ifdef ABS_TOP_SRCDIR
        if (LoadBookFile(book, path(ABS_TOP_SRCDIR) / "book" / fileName))
            return;
#endif
        if (LoadBookFile(book, programDir / fileName))
            return;
#if defined(DATADIR) && defined(PACKAGE)
        if (LoadBookFile(book, path(DATADIR) / PACKAGE / fileName))
            return;
#endif
        throw SgException("Could not find opening book.");
    }

    static bool LoadBookFile(GoBook& book, const fs::path& file)
    {
        std::string nativeFile = SgStringUtil::GetNativeFileName(file);
        SgDebug() << "Loading opening book from '" << nativeFile << "'... ";
        std::ifstream in(nativeFile.c_str());
        if (!in)
        {
            SgDebug() << "not found\n";
            return false;
        }
        try
        {
            book.Read(in);
        }
        catch (const SgException& e)
        {
            SgDebug() << "error: " << e.what() << '\n';
            return false;
        }
        SgDebug() << "ok\n";
        return true;
    }

    void CmdAnalyzeCommands(GtpCommand& cmd) override
    {
        GoGtpEngine::CmdAnalyzeCommands(cmd);
        m_uctCommands.AddGoGuiAnalyzeCommands(cmd);
        m_safetyCommands.AddGoGuiAnalyzeCommands(cmd);
        m_autoBookCommands.AddGoGuiAnalyzeCommands(cmd);
        cmd << "string/Fuego License/fuego-license\n";
        std::string response = cmd.Response();
        cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
    }

    void CmdName(GtpCommand& cmd) override
    {
        cmd << "Fuego";
    }

    void CmdVersion(GtpCommand& cmd) override
    {
        cmd << Version();
    }

    void CmdLicense(GtpCommand& cmd)
    {
        cmd << "\n" <<
            "Fuego " << Version() << "\n" <<
            "Copyright (C) 2009-2013 by the authors of the Fuego project.\n"
            "See http://fuego.sf.net for information about Fuego. Fuego comes\n"
            "with NO WARRANTY to the extent permitted by law. This program is\n"
            "free software; you can redistribute it and/or modify it under the\n"
            "terms of the GNU Lesser General Public License as published by the\n"
            "Free Software Foundation - version 3. For more information about\n"
            "these matters, see the files named COPYING and COPYING.LESSER\n";
    }

    static std::string Version()
    {
        std::ostringstream s;
#ifdef VERSION
        s << VERSION;
#else
        s << "(" __DATE__ ")";
#endif
#ifdef SVNREV
        s << "(" SVNREV ")";
#endif
#ifndef NDEBUG
        s << " (dbg)";
#endif
        return s.str();
    }
};

FuegoEngine::FuegoEngine(fuego_engine_configuration const& cfg)
{
    SgInit();
    try {
        GoInit();
        try {
            SgRandom::SetSeed(cfg.srand);
            impl_ = std::make_unique<FuegoEngineImpl>(cfg);
            assertionHandler_.emplace(*impl_); // noexcept
        } catch (...) {
            GoFini();
            throw;
        }
    } catch (...) {
        SgFini();
        throw;
    }
}

FuegoEngine::~FuegoEngine()
{
    assertionHandler_ = std::nullopt;
    impl_.reset();
    try {
        GoFini();
        SgFini();
    } catch (std::exception const& e) {
        SgDebug() << "finalization error: " << e.what();
    } catch (...) {
        SgDebug() << "unknown finalization error";
    }
}

std::pair<bool, std::string> FuegoEngine::ProcessCommand(std::string_view command) noexcept
{
    return impl_->ExecuteCommand(command);
}

#include <cstring>
#if !WIN32
#   define _strdup strdup
#endif
extern "C" {

fuego_result_descriptor fuego_create_engine(
    const char* program_path,
    const char* config_path,
    int srand,
    int fixed_board_size,
    int max_games,
    int use_book,
    int allow_handicap)
{
    FuegoEngine* peng = nullptr;
    try {
        fuego_engine_configuration engine_cfg;
        engine_cfg.programPath = program_path ? program_path : "";
        engine_cfg.configPath = config_path ? config_path : "";
        engine_cfg.srand = srand;
        engine_cfg.fixedBoardSize = fixed_board_size;
        engine_cfg.maxGames = max_games;
        engine_cfg.useBook = !!use_book;
        engine_cfg.allowHandicap = allow_handicap;
        peng = new FuegoEngine(engine_cfg);
        return { peng, 0 };
    } catch (std::exception const& e) {
        if (peng) delete peng;
        return { _strdup(e.what()), 1 };
    } catch (...) {
        if (peng) delete peng;
        return { _strdup("fatal error, unknown exception"), 1 };
    }
}

void fuego_free_string(void* cookie)
{
    free(cookie);
}

void fuego_free_engine(void* cookie)
{
    FuegoEngine * peng = reinterpret_cast<FuegoEngine*>(cookie);
    delete peng;
}

fuego_result_descriptor fuego_process_command(void* cookie, const char* cmd, size_t cmdlen)
{
    FuegoEngine* peng = reinterpret_cast<FuegoEngine*>(cookie);
    try {
        auto [suc, result] = peng->ProcessCommand(std::string_view{ cmd, cmdlen });
        return { _strdup(result.c_str()), suc ? 0 : 1 };
    } catch (std::exception const& e) {
        return { _strdup(e.what()), 1 };
    } catch (...) {
        return { _strdup("fatal error, unknown exception"), 1 };
    }
}

}
