//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPriorKnowledge.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledge::GoUctDefaultPriorKnowledge(const GoBoard& bd,
                              const GoUctDefaultPlayoutPolicyParam& param)
    : m_bd(bd),
      m_policy(bd, param)
{
}

void GoUctDefaultPriorKnowledge::Initialize(SgPoint p, float value,
                                            std::size_t count)
{
    m_values[p] = value;
    m_counts[p] = count;
}

void GoUctDefaultPriorKnowledge::ProcessPosition(bool& deepenTree)
{
    SG_UNUSED(deepenTree);
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctDefaultPlayoutPolicyType type = m_policy.MoveType();
    const GoUctPatterns<GoBoard>& patterns = m_policy.Patterns();
    SgPointSet patternMatch;
    bool anyPatternMatch = false;
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it) && patterns.MatchAny(*it))
        {
            patternMatch.Include(*it);
            anyPatternMatch = true;
        }


    if (type == GOUCT_RANDOM && ! anyPatternMatch)
    {
        Initialize(SG_PASS, 0.0, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.0, 3);
            else
                m_counts[*it] = 0; // Don't initialize
        }
    }
    else if (type == GOUCT_RANDOM && anyPatternMatch)
    {
        Initialize(SG_PASS, 0.0, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.0, 3);
            else if (patternMatch[*it])
                Initialize(*it, 1.0, 3);
            else
                Initialize(*it, 0.5, 3);
        }
    }
    else
    {
        Initialize(SG_PASS, 0.0, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.0, 9);
            else if (patterns.MatchAny(*it))
                Initialize(*it, 0.6, 9);
            else
                Initialize(*it, 0.4, 9);
        }
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            Initialize(*it, 1.0, 9);
    }

    m_policy.EndPlayout();
}

void GoUctDefaultPriorKnowledge::InitializeMove(SgMove move, float& value,
                                                std::size_t& count)
{
    value = m_values[move];
    count = m_counts[move];
}

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledgeFactory
::GoUctDefaultPriorKnowledgeFactory(const GoUctDefaultPlayoutPolicyParam&
                                    param)
    : m_param(param)
{
}

SgUctPriorKnowledge*
GoUctDefaultPriorKnowledgeFactory::Create(SgUctThreadState& state)
{
    GoUctGlobalSearchState& globalSearchState
        = dynamic_cast<GoUctGlobalSearchState&>(state);
    return new GoUctDefaultPriorKnowledge(globalSearchState.Board(),
                                          m_param);
}

//----------------------------------------------------------------------------
