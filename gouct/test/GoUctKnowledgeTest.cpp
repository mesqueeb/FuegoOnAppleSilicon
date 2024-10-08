//----------------------------------------------------------------------------
/** @file GoUctKnowledgeTest.cpp
    Unit tests for GoUctKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "GoUctKnowledge.h"
#include "SgPoint.h"
#include "SgUctSearch.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

class GoUctKnowledgeTester : public GoUctKnowledge
{
public:
    GoUctKnowledgeTester(const GoBoard& bd);

    /** Compute knowledge for the current position */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

GoUctKnowledgeTester::GoUctKnowledgeTester(const GoBoard& bd)
	: GoUctKnowledge(bd)
{ }

void GoUctKnowledgeTester::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    Add(Pt(3, 1), SgUctValue(0.5), SgUctValue(10));
    Add(Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
    TransferValues(moves);
}

void GoUctKnowledgeTestInit(std::vector<SgUctMoveInfo>& moves, const GoBoard& bd)
{
    for (GoBoard::Iterator it(bd); it; ++it)
        moves.push_back(SgUctMoveInfo(*it));
}

void GoUctKnowledgeTestCheckValue(const SgUctMoveInfo& moveInfo, SgPoint p,
                SgUctValue value, SgUctValue count)
{
    BOOST_CHECK_EQUAL(moveInfo.m_move, p);
    BOOST_CHECK_CLOSE(moveInfo.m_value, SgUctSearch::InverseEstimate(value),
                      0.00001);
    BOOST_CHECK_EQUAL(moveInfo.m_count, count);
    BOOST_CHECK_CLOSE(moveInfo.m_raveValue, value,
                      0.00001);
    BOOST_CHECK_EQUAL(moveInfo.m_raveCount, count);
}

void GoUctKnowledgeTestCheckUndefined(const SgUctMoveInfo& moveInfo, SgPoint p)
{
    BOOST_CHECK_EQUAL(moveInfo.m_move, p);
    BOOST_CHECK_EQUAL(moveInfo.m_value, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_count, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_raveValue, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_raveCount, SgUctValue(0));
}

const SgUctMoveInfo* FindMoveInfo(const std::vector<SgUctMoveInfo>& moves, 
                            SgPoint p)
{
    /* @todo write it using a find template.
    std::vector<SgUctMoveInfo>::iterator it = find(moves, p);
    BOOST_REQUIRE_NE(it, moves.end());
	GoUctKnowledgeTestCheckValue(*it, p, value, count);
    */
    std::vector<SgUctMoveInfo>::const_iterator it = moves.begin();
    for (; it != moves.end(); ++it)
        if (it->m_move == p)
            return &(*it);
    BOOST_ERROR("move not found");
    return 0;
}

void GoUctKnowledgeTestCheckValue(const std::vector<SgUctMoveInfo>& moves, SgPoint p,
                SgUctValue value, SgUctValue count)
{
    const SgUctMoveInfo* m = FindMoveInfo(moves, p);
    if (m)
    	GoUctKnowledgeTestCheckValue(*m, p, value, count);
}

void GoUctKnowledgeTestCheckUndefined(const std::vector<SgUctMoveInfo>& moves, SgPoint p)
{
    const SgUctMoveInfo* m = FindMoveInfo(moves, p);
    if (m)
		GoUctKnowledgeTestCheckUndefined(*m, p);
}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoUctKnowledgeTest_ProcessPosition)
{
    GoBoard bd(9);
    GoUctKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    GoUctKnowledgeTestInit(moves, bd);
    tester.TransferValues(moves);
    GoUctKnowledgeTestCheckUndefined(moves, Pt(3, 1));
    GoUctKnowledgeTestCheckUndefined(moves, Pt(3, 2));
    GoUctKnowledgeTestCheckUndefined(moves, Pt(5, 5));
    tester.ProcessPosition(moves);
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 1), SgUctValue(0.5), SgUctValue(10));
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
    GoUctKnowledgeTestCheckUndefined(moves, Pt(5, 5));
}

BOOST_AUTO_TEST_CASE(GoUctKnowledgeTest_AddValuesTo)
{
    GoBoard bd(9);
    GoUctKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    GoUctKnowledgeTestInit(moves, bd);
    tester.Add(Pt(3, 1), SgUctValue(0.6), SgUctValue(10));
    tester.Add(Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
    tester.AddValuesTo(moves);
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 1), SgUctValue(0.6), SgUctValue(10));
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
    GoUctKnowledgeTester tester2(bd);
    tester2.Add(Pt(3, 1), SgUctValue(0.3), SgUctValue(20));
    tester2.Add(Pt(3, 2), SgUctValue(0.7), SgUctValue(20));
    tester2.AddValuesTo(moves);
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 1), SgUctValue(0.4), SgUctValue(30));
    GoUctKnowledgeTestCheckValue(moves, Pt(3, 2), SgUctValue(0.4), SgUctValue(40));
}
    
//----------------------------------------------------------------------------

} // namespace
