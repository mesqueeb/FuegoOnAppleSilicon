//----------------------------------------------------------------------------
/** @file SgThreadedWorker.h */
//----------------------------------------------------------------------------

#pragma once

#include "SgDebug.h"

#include <thread>
#include <barrier>
#include <mutex>
#include <memory>

//----------------------------------------------------------------------------

template<typename I, typename O, typename W>
class SgThreadedWorker
{
public:
    
    SgThreadedWorker(std::vector<W>& workers);

    ~SgThreadedWorker();

    void DoWork(const std::vector<I>& work, 
                std::vector<std::pair<I,O> >& output);
    
private:

    void StartDoingWork();

    void WaitForThreadsToFinish();

    void TellThreadsToQuit();

    friend class Thread;

    /** Copyable object run in a boost::thread. */
    class Thread
    {
    public:
        Thread(size_t threadId, W& worker, 
               SgThreadedWorker<I,O,W>& threadedWork);

        void operator()();

    private:

        size_t m_id;

        W& m_worker;
            
        SgThreadedWorker<I,O,W>& m_boss;
    };

    /** Flag telling threads to exit. */
    bool m_quit;

    /** Threads must lock this mutex before getting work from list. */
    std::mutex m_workMutex;

    /** Threads must lock this mutex before updating output. */
    std::mutex m_outputMutex;

    /** Threads block on this barrier until told to start. */
    std::barrier<void(*)() noexcept> m_startWork;

    /** Threads block on this barrier until all are finished. */
    std::barrier<void(*)() noexcept> m_workFinished;

    /** Index of next problem to solve. */
    size_t m_workIndex;

    /** Problems to solve. */
    const std::vector<I>* m_workToDo;

    /** Solved problems. */
    std::vector<std::pair<I,O> >* m_output;

    /** The threads. */
    std::vector<std::shared_ptr<std::thread> > m_threads;
};

//----------------------------------------------------------------------------

template<typename I, typename O, typename W>
SgThreadedWorker<I,O,W>::SgThreadedWorker(std::vector<W>& workers)
    : m_quit(false),
    m_startWork(static_cast<unsigned int>(workers.size() + 1), []()noexcept{}),
      m_workFinished(static_cast<unsigned int>(workers.size() + 1), []()noexcept {})
{
    for (size_t i = 0; i < workers.size(); ++i)
    {
        Thread runnable(i, workers[i], *this);
        m_threads.emplace_back(std::make_shared<std::thread>(std::move(runnable)));
    }
}

template<typename I, typename O, typename W>
SgThreadedWorker<I,O,W>::~SgThreadedWorker()
{
    TellThreadsToQuit();
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->join();
        SgDebug() << "SgThreadedWorker: joined " << i << '\n';
    }
}

template<typename I, typename O, typename W>
void SgThreadedWorker<I,O,W>::DoWork(const std::vector<I>& work,
                                   std::vector<std::pair<I,O> >& output)
{
    m_workToDo = &work;
    m_workIndex = 0;
    m_output = &output;
    SgDebug() << "SgThreadedWorker::DoWork(): Processing " 
              << work.size() << " jobs." << '\n';
    StartDoingWork();
    WaitForThreadsToFinish();
}

template<typename I, typename O, typename W>
SgThreadedWorker<I,O,W>::Thread::Thread(size_t threadId, W& worker, 
                                      SgThreadedWorker<I,O,W>& threadedWorker)
    : m_id(threadId),
      m_worker(worker),
      m_boss(threadedWorker)
{ }

template<typename I, typename O, typename W>
void SgThreadedWorker<I,O,W>::Thread::operator()()
{
    while (true)
    {
        m_boss.m_startWork.arrive_and_wait();
        if (m_boss.m_quit) 
            break;
        //SgDebug() << "[" << m_id << "]: starting..."  << '\n';
        while (true)
        {
            bool finished = false;
            const I* currentWork = 0;
            {
                std::scoped_lock lock(m_boss.m_workMutex);
                if (m_boss.m_workIndex < m_boss.m_workToDo->size())
                    currentWork = &(*m_boss.m_workToDo)[m_boss.m_workIndex++];
                else
                    finished = true;
            }
            if (finished)
                break;
            O answer = m_worker(*currentWork);
            {
                std::scoped_lock lock(m_boss.m_outputMutex);
                m_boss.m_output
                    ->push_back(std::make_pair(*currentWork, answer));
            }
        }
        //SgDebug() << "[" << m_id << "]: finished." << '\n';
        m_boss.m_workFinished.arrive_and_wait();
    }
}

template<typename I, typename O, typename W>
void SgThreadedWorker<I,O,W>::StartDoingWork()
{
    m_startWork.arrive_and_wait();
}

template<typename I, typename O, typename W>
void SgThreadedWorker<I,O,W>::WaitForThreadsToFinish()
{
    m_workFinished.arrive_and_wait();
}

template<typename I, typename O, typename W>
void SgThreadedWorker<I,O,W>::TellThreadsToQuit()
{
    m_quit = true;
    m_startWork.arrive_and_wait();
}

//----------------------------------------------------------------------------

