//----------------------------------------------------------------------------
/** @file GtpEngine.cpp
    See GtpEngine.h */
//----------------------------------------------------------------------------

#include "GtpEngine.h"

#include <iomanip>
#include <cassert>
#include <cctype>
#include <fstream>

#if GTPENGINE_PONDER || GTPENGINE_INTERRUPT
#   include <thread>
#   include <mutex>
#   include <barrier>
#   include <condition_variable>
#endif

#ifdef _MSC_VER
// Don't report Visual C++ warning 4355 ('this' : used in base member
// initializer list) in default warning level 3. The constructors of
// ReadThread and PonderThread store a reference to 'this', which is
// allowed as long as 'this' is not used yet.
#pragma warning(4:4355)
#endif

//----------------------------------------------------------------------------

/** Utility functions. */
namespace {

void Trim(std::string& str);

/** Check, if line contains a command.
    @param line The line to check.
    @return True, if command does not contain only whitespaces and is not a
    comment line. */
bool IsCommandLine(std::string line)
{
    Trim(line);
    return (!line.empty() && line[0] != '#');
}

#if ! GTPENGINE_INTERRUPT

/** Read next command from stream.
    @param in The input stream.
    @param[out] cmd The command (reused for efficiency)
    @return @c false on end-of-stream or read error. */
bool ReadCommand(GtpCommand& cmd, GtpInputStream& in)
{
    std::string line;
    while (in.GetLine(line) && ! IsCommandLine(line))
    {
    }
    if (in.EndOfInput())
        return false;
    Trim(line);
    cmd.Init(line);
    return true;
}
#endif

/** Replace empty lines in a multi-line string by lines containing a single
    space.
    @param text The input string.
    @return The input string with all occurrences of "\n\n" replaced by
    "\n \n". */
std::string ReplaceEmptyLines(const std::string& text)
{
    if (text.find("\n\n") == std::string::npos)
        return text;
    std::istringstream in(text);
    std::ostringstream result;
    bool lastWasNewLine = false;
    char c;
    while (in.get(c))
    {
        bool isNewLine = (c == '\n');
        if (isNewLine && lastWasNewLine)
            result.put(' ');
        result.put(c);
        lastWasNewLine = isNewLine;
    }
    return result.str();
}

/** Remove leading and trailing whitespaces from a string.
    Whitespaces are tab, carriage return and space.
    @param str The input string. */
void Trim(std::string& str)
{
    char const* whiteSpace = " \t\r";
    size_t pos = str.find_first_not_of(whiteSpace);
    str.erase(0, pos);
    pos = str.find_last_not_of(whiteSpace);
    str.erase(pos + 1);
}

} // namespace

//----------------------------------------------------------------------------

#if GTPENGINE_PONDER || GTPENGINE_INTERRUPT

/** Utility functions for Boost.Thread. */
namespace {

void Notify(std::mutex& aMutex, std::condition_variable& aCondition)
{
    std::unique_lock lock(aMutex);
    aCondition.notify_all();
}

} // namespace

#endif // GTPENGINE_PONDER || GTPENGINE_INTERRUPT

//----------------------------------------------------------------------------

#if GTPENGINE_PONDER

namespace {

/** Ponder thread used by GtpEngine::MainLoop().
    This thread calls GtpEngine::Ponder() while the engine is waiting for the
    next command.
    @see GtpEngine::Ponder() */
class PonderThread
{
public:
    PonderThread(GtpEngine& engine);

    void StartPonder();

    void StopPonder();

    void Quit();

private:
    class Function
    {
    public:
        Function(PonderThread& ponderThread);

        void operator()();

    private:
        PonderThread& m_ponderThread;
    };

    friend class PonderThread::Function;

    GtpEngine& m_engine;

    std::barrier<void(*)() noexcept> m_threadReady;

    std::mutex m_startPonderMutex;

    std::mutex m_ponderFinishedMutex;

    std::condition_variable m_startPonder;

    std::condition_variable m_ponderFinished;

    std::unique_lock<std::mutex> m_ponderFinishedLock;

    /** The thread to run the ponder function.
        Order dependency: must be constructed as the last member, because the
        constructor starts the thread. */
    std::thread m_thread;
};

PonderThread::Function::Function(PonderThread& ponderThread)
    : m_ponderThread(ponderThread)
{ }

void PonderThread::Function::operator()()
{
    std::unique_lock lock(m_ponderThread.m_startPonderMutex);
    m_ponderThread.m_threadReady.arrive_and_wait();
    while (true)
    {
        m_ponderThread.m_startPonder.wait(lock);
        GtpEngine& engine = m_ponderThread.m_engine;
        if (engine.IsQuitSet())
            return;
        engine.Ponder();
        Notify(m_ponderThread.m_ponderFinishedMutex,
               m_ponderThread.m_ponderFinished);
    }
}

PonderThread::PonderThread(GtpEngine& engine)
    : m_engine(engine),
    m_threadReady(2, []()noexcept{}),
      m_ponderFinishedLock(m_ponderFinishedMutex),
      m_thread(Function(*this))
{
    m_threadReady.arrive_and_wait();
}

void PonderThread::StartPonder()
{
    m_engine.InitPonder();
    Notify(m_startPonderMutex, m_startPonder);
}

void PonderThread::StopPonder()
{
    m_engine.StopPonder();
    m_ponderFinished.wait(m_ponderFinishedLock);
}

void PonderThread::Quit()
{
    Notify(m_startPonderMutex, m_startPonder);
    m_thread.join();
}

} // namespace

#endif // GTPENGINE_PONDER

//----------------------------------------------------------------------------

#if GTPENGINE_INTERRUPT

namespace {

/** Thread for reading the next command line.
    This thread is used instead of the simple function
    ReadCommand(GtpCommand&), if GtpEngine is compiled with interrupt
    support.
    @see GtpEngine::Interrupt() */
class ReadThread
{
public:
    ReadThread(GtpInputStream& in, GtpEngine& engine);

    bool ReadCommand(GtpCommand& cmd);

    void JoinThread();

private:
    class Function
    {
    public:
        Function(ReadThread& readThread);

        void operator()();

    private:
        ReadThread& m_readThread;

        void ExecuteSleepLine(const std::string& line);
    };

    friend class ReadThread::Function;

    GtpInputStream& m_in;

    GtpEngine& m_engine;

    std::string m_line;

    bool m_isStreamGood;

    std::barrier<void(*)() noexcept> m_threadReady;

    std::mutex m_waitCommandMutex;

    std::condition_variable m_waitCommand;

    std::mutex m_commandReceivedMutex;

    std::condition_variable m_commandReceived;

    std::unique_lock<std::mutex> m_commandReceivedLock;

    /** The thread to run the read command function.
        Order dependency: must be constructed as the last member, because the
        constructor starts the thread. */
    std::thread m_thread;
};

ReadThread::Function::Function(ReadThread& readThread)
    : m_readThread(readThread)
{ }

void ReadThread::Function::operator()()
{
    std::unique_lock lock(m_readThread.m_waitCommandMutex);
    m_readThread.m_threadReady.arrive_and_wait();
    GtpEngine& engine = m_readThread.m_engine;
    GtpInputStream& in = m_readThread.m_in;
    std::string line;
    while (true)
    {
        while (in.GetLine(line))
        {
            Trim(line);
            if (line == "# interrupt")
                engine.Interrupt();
            else if (line.find("# gtpengine-sleep ") == 0)
                ExecuteSleepLine(line);
            else if (IsCommandLine(line))
                break;
        }
        m_readThread.m_waitCommand.wait(lock);
        m_readThread.m_isStreamGood = ! in.EndOfInput();
        m_readThread.m_line = line;
        Notify(m_readThread.m_commandReceivedMutex,
               m_readThread.m_commandReceived);
        if (in.EndOfInput())
            return;
        // See comment at GtpEngine::SetQuit
        GtpCommand cmd(line);
        if (cmd.Name() == "quit")
            return;
    }
}

void ReadThread::Function::ExecuteSleepLine(const std::string& line)
{
    std::istringstream buffer(line);
    std::string s;
    buffer >> s;
    assert(s == "#");
    buffer >> s;
    assert(s == "gtpengine-sleep");
    int seconds;
    buffer >> seconds;
    if (seconds > 0)
    {
        std::cerr << "GtpEngine: sleep " << seconds << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        std::cerr << "GtpEngine: sleep done\n";
    }
}

void ReadThread::JoinThread()
{
    m_thread.join();
}

ReadThread::ReadThread(GtpInputStream& in, GtpEngine& engine)
    : m_in(in),
      m_engine(engine),
      m_threadReady(2, []()noexcept{}),
      m_commandReceivedLock(m_commandReceivedMutex),
      m_thread(Function(*this))
{
    m_threadReady.arrive_and_wait();
}

bool ReadThread::ReadCommand(GtpCommand& cmd)
{
    Notify(m_waitCommandMutex, m_waitCommand);
    m_commandReceived.wait(m_commandReceivedLock);
    if (! m_isStreamGood)
        return false;
    cmd.Init(m_line);
    return true;
}

} // namespace

#endif // GTPENGINE_INTERRUPT

//----------------------------------------------------------------------------

GtpFailure::GtpFailure()
{ }

GtpFailure::GtpFailure(const GtpFailure& failure)
{
    m_response << failure.Response();
    m_response.copyfmt(failure.m_response);
}

GtpFailure::GtpFailure(const std::string& response)
{
    m_response << response;
}

GtpFailure::~GtpFailure() throw()
{ }

//----------------------------------------------------------------------------

GtpCommand::Argument::Argument(const std::string& value, size_t end)
    : m_value(value),
      m_end(end)
{ }

//----------------------------------------------------------------------------

std::ostringstream GtpCommand::s_dummy;

const std::string& GtpCommand::Arg(size_t number) const
{
    size_t index = number + 1;
    if (number >= NuArg())
        throw GtpFailure() << "missing argument " << index;
    return m_arguments[index].m_value;
}

const std::string& GtpCommand::Arg() const
{
    CheckNuArg(1);
    return Arg(0);
}

template<>
size_t GtpCommand::Arg<size_t>(size_t i) const
{
    // See the function declaration in GtpEngine.h for the rationale why this
    // template specialization is necessary.
    std::string arg = Arg(i);
    bool fail = (! arg.empty() && arg[0] == '-');
    size_t result;
    if (! fail)
    {
        std::istringstream in(arg);
        in >> result;
        fail = ! in;
    }
    if (fail)
        throw GtpFailure() << "argument " << (i + 1) << " (" << arg
                           << ") must be of type size_t";
    return result;
}

std::string GtpCommand::ArgLine() const
{
    std::string result = m_line.substr(m_arguments[0].m_end);
    Trim(result);
    return result;
}

std::string GtpCommand::ArgToLower(size_t number) const
{
    std::string value = Arg(number);
    for (size_t i = 0; i < value.length(); ++i)
        value[i] = char(tolower(value[i]));
    return value;
}

bool GtpCommand::BoolArg(size_t number) const
{
    return Arg<bool>(number);
}

void GtpCommand::CheckNuArg(size_t number) const
{
    if (NuArg() == number)
        return;
    if (number == 0)
        throw GtpFailure() << "no arguments allowed";
    else if (number == 1)
        throw GtpFailure() << "command needs one argument";
    else
        throw GtpFailure() << "command needs " << number << " arguments";
}

void GtpCommand::CheckNuArgLessEqual(size_t number) const
{
    if (NuArg() <= number)
        return;
    if (number == 1)
        throw GtpFailure() << "command needs at most one argument";
    else
    throw GtpFailure() << "command needs at most " << number << " arguments";
}

double GtpCommand::FloatArg(size_t number) const
{
    return Arg<double>(number);
}

int GtpCommand::IntArg(size_t number) const
{
    return Arg<int>(number);
}

int GtpCommand::IntArg(size_t number, int min) const
{
    return ArgMin<int>(number, min);
}

int GtpCommand::IntArg(size_t number, int min, int max) const
{
    return ArgMinMax<int>(number, min, max);
}

void GtpCommand::Init(std::string_view line)
{
    m_line = line;
    Trim(m_line);
    SplitLine(m_line);
    assert(m_arguments.size() > 0);
    ParseCommandId();
    assert(m_arguments.size() > 0);
    m_response.str("");
    m_response.copyfmt(s_dummy);
}

void GtpCommand::ParseCommandId()
{
    m_id = "";
    if (m_arguments.size() < 2)
        return;
    std::istringstream in(m_arguments[0].m_value);
    int id;
    in >> id;
    if (in)
    {
        m_id = m_arguments[0].m_value;
        m_arguments.erase(m_arguments.begin());
    }
}

std::string GtpCommand::RemainingLine(size_t number) const
{
    size_t index = number + 1;
    if (number >= NuArg())
        throw GtpFailure() << "missing argument " << index;
    std::string result = m_line.substr(m_arguments[index].m_end);
    Trim(result);
    return result;
}

void GtpCommand::SetResponse(const std::string& response)
{
    m_response.str(response);
}

void GtpCommand::SetResponseBool(bool value)
{
    m_response.str(value ? "true" : "false");
}

size_t GtpCommand::SizeTypeArg(size_t number) const
{
    return Arg<size_t>(number);
}

size_t GtpCommand::SizeTypeArg(size_t number, size_t min) const
{
    return ArgMin<size_t>(number, min);
}

/** Split line into arguments.
    Arguments are words separated by whitespaces.
    Arguments with whitespaces can be quoted with quotation marks ('"').
    Characters can be escaped with a backslash ('\').
    @param line The line to split. */
void GtpCommand::SplitLine(const std::string& line)
{
    m_arguments.clear();
    bool escape = false;
    bool inString = false;
    std::ostringstream element;
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"' && ! escape)
        {
            if (inString)
            {
                m_arguments.push_back(Argument(element.str(), i + 1));
                element.str("");
            }
            inString = ! inString;
        }
        else if (isspace(c) && ! inString)
        {
            if (! element.str().empty())
            {
                m_arguments.push_back(Argument(element.str(), i + 1));
                element.str("");
            }
        }
        else
            element << c;
        escape = (c == '\\' && ! escape);
    }
    if (! element.str().empty())
        m_arguments.push_back(Argument(element.str(), line.size()));
}

//----------------------------------------------------------------------------

GtpCallbackBase::~GtpCallbackBase() throw()
{ }

//----------------------------------------------------------------------------

GtpEngine::GtpEngine()
    : m_quit(false)
{
    Register("known_command", &GtpEngine::CmdKnownCommand, this);
    Register("list_commands", &GtpEngine::CmdListCommands, this);
    Register("name", &GtpEngine::CmdName, this);
    Register("protocol_version", &GtpEngine::CmdProtocolVersion, this);
    Register("quit", &GtpEngine::CmdQuit, this);
    Register("version", &GtpEngine::CmdVersion, this);
}

GtpEngine::~GtpEngine()
{
    typedef CallbackMap::iterator Iterator;
    for (Iterator i = m_callbacks.begin(); i != m_callbacks.end(); ++i)
    {
        delete i->second;
#ifndef NDEBUG
        i->second = 0;
#endif
    }
}

void GtpEngine::BeforeHandleCommand()
{
    // Default implementation does nothing
}

void GtpEngine::BeforeWritingResponse()
{
    // Default implementation does nothing
}

/** Return @c true if command is known, @c false otherwise. */
void GtpEngine::CmdKnownCommand(GtpCommand& cmd)
{
    cmd.SetResponseBool(IsRegistered(cmd.Arg()));
}

/** List all known commands. */
void GtpEngine::CmdListCommands(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    typedef CallbackMap::const_iterator Iterator;
    for (Iterator i = m_callbacks.begin(); i != m_callbacks.end(); ++i)
        cmd << i->first << '\n';
}

/** Return name. */
void GtpEngine::CmdName(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << "Unknown";
}

/** Return protocol version. */
void GtpEngine::CmdProtocolVersion(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << "2";
}

/** Quit command loop. */
void GtpEngine::CmdQuit(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SetQuit();
}

/** Return empty version string.
    The GTP standard says to return empty string, if no meaningful reponse
    is available. */
void GtpEngine::CmdVersion(GtpCommand& cmd)
{
    cmd.CheckArgNone();
}

std::pair<bool, std::string> GtpEngine::ExecuteCommand(std::string_view cmdline) noexcept
{
    using namespace std::string_view_literals;

    try {
        if (!IsCommandLine(std::string{cmdline}))
            return { false, (std::ostringstream{} << "Bad command: "sv << cmdline).str() };
        GtpCommand cmd;
        cmd.Init(cmdline);
    
        std::ostringstream rss;
        GtpOutputStream gtpLog(rss);
        bool status = HandleCommand(cmd, gtpLog);
        return { status, cmd.Response() };
    } catch (std::exception const& e) {
        return { false, e.what() };
    } catch (...) {
        return { false, "fatal error, unknown exception" };
    }
}

std::string GtpEngine::ExecuteCommand(const std::string& cmdline, std::ostream& log)
{
    if (! IsCommandLine(cmdline))
        throw GtpFailure() << "Bad command: " << cmdline;
    GtpCommand cmd;
    cmd.Init(cmdline);
    log << cmd.Line() << '\n';
    GtpOutputStream gtpLog(log);
    bool status = HandleCommand(cmd, gtpLog);
    std::string response = cmd.Response();
    if (! status)
        throw GtpFailure() << "Executing " << cmd.Line() << " failed";
    return response;
}

void GtpEngine::ExecuteFile(const std::string& name, std::ostream& log)
{
    std::ifstream in(name.c_str());
    if (! in)
        throw GtpFailure() << "Cannot read " << name;
    std::string line;
    GtpCommand cmd;
    GtpOutputStream gtpLog(log);
    while (getline(in, line))
    {
        if (! IsCommandLine(line))
            continue;
        cmd.Init(line);
        log << cmd.Line() << '\n';

        bool status = HandleCommand(cmd, gtpLog);
        if (! status)
            throw GtpFailure() << "Executing " << cmd.Line() << " failed";
    }
}

bool GtpEngine::HandleCommand(GtpCommand& cmd, GtpOutputStream& out)
{
    BeforeHandleCommand();
    bool status = true;
    std::string response;
    try
    {
        CallbackMap::const_iterator pos = m_callbacks.find(cmd.Name());
        if (pos == m_callbacks.end())
        {
            status = false;
            response = "unknown command: " + cmd.Name();
        }
        else
        {
            GtpCallbackBase* callback = pos->second;
            (*callback)(cmd);
            response = cmd.Response();
        }
    }
    catch (const GtpFailure& failure)
    {
        status = false;
        response = failure.Response();
    }
    response = ReplaceEmptyLines(response);
    BeforeWritingResponse();
    std::ostringstream ostr;
    ostr << (status ? '=' : '?') << cmd.ID() << ' ' << response;
    size_t size = response.size();
    if (size == 0 || response[size - 1] != '\n')
        ostr << '\n';
    ostr << '\n' << std::flush;
    out.Write(ostr.str());
    out.Flush();
    return status;
}

bool GtpEngine::IsRegistered(const std::string& command) const
{
    return (m_callbacks.find(command) != m_callbacks.end());
}

void GtpEngine::MainLoop(GtpInputStream& in, GtpOutputStream& out)
{
    m_quit = false;
#if GTPENGINE_PONDER
    PonderThread ponderThread(*this);
#endif
#if GTPENGINE_INTERRUPT
    ReadThread readThread(in, *this);
#endif
    GtpCommand cmd;
    while (true)
    {
#if GTPENGINE_PONDER
        ponderThread.StartPonder();
#endif
#if GTPENGINE_INTERRUPT
        bool isStreamGood = readThread.ReadCommand(cmd);
#else
        bool isStreamGood = ReadCommand(cmd, in);
#endif
#if GTPENGINE_PONDER
        ponderThread.StopPonder();
#endif
        if (isStreamGood)
            HandleCommand(cmd, out);
        else
            SetQuit();
        if (m_quit)
        {
#if GTPENGINE_PONDER
            ponderThread.Quit();
#endif
#if GTPENGINE_INTERRUPT
            readThread.JoinThread();
#endif
            break;
        }
    }
}

void GtpEngine::Register(const std::string& command, GtpCallbackBase* callback)
{
    CallbackMap::iterator pos = m_callbacks.find(command);
    if (pos != m_callbacks.end())
    {
        delete pos->second;
#ifndef NDEBUG
        pos->second = 0;
#endif
        m_callbacks.erase(pos);
    }
    m_callbacks.insert(make_pair(command, callback));
}

void GtpEngine::SetQuit()
{
    m_quit = true;
}

bool GtpEngine::IsQuitSet() const
{
    return m_quit;
}


#if GTPENGINE_PONDER

void GtpEngine::Ponder()
{
    // Default implementation does nothing
#ifdef GTPENGINE_TEST
    std::cerr << "GtpEngine::Ponder()\n";
#endif
}

void GtpEngine::StopPonder()
{
    // Default implementation does nothing
#ifdef GTPENGINE_TEST
    std::cerr << "GtpEngine::StopPonder()\n";
#endif
}

void GtpEngine::InitPonder()
{
    // Default implementation does nothing
#ifdef GTPENGINE_TEST
    std::cerr << "GtpEngine::InitPonder()\n";
#endif
}

#endif // GTPENGINE_PONDER


#if GTPENGINE_INTERRUPT

void GtpEngine::Interrupt()
{
    // Default implementation does nothing
#ifdef GTPENGINE_TEST
    std::cerr << "GtpEngine::Interrupt()\n";
#endif
}

#endif // GTPENGINE_INTERRUPT

//----------------------------------------------------------------------------

#ifdef GTPENGINE_MAIN

/** Main routine for testing and standalone compilation. */
int main()
{
    try
    {
        GtpEngine engine(cin, cout);
        engine.MainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

#endif // GTPENGINE_MAIN

//----------------------------------------------------------------------------
