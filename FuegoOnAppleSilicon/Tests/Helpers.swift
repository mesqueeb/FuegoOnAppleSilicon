@testable import FuegoOnAppleSilicon
import XCTest

final class FuegoOnAppleSiliconTests: XCTestCase {
  func testTileIndexesToGoBoardCoordinate() {
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(0, 0)).rawValue, "A1", "Expected A1 for tile (0, 0)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(18, 18)).rawValue, "T19", "Expected T19 for tile (18, 18)")
    // Test tiles around 'I'
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(0, 7)).rawValue, "H1", "Expected H1 for tile (0, 7)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(0, 8)).rawValue, "J1", "Expected J1 for tile (0, 8)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(1, 7)).rawValue, "H2", "Expected H2 for tile (1, 7)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(1, 8)).rawValue, "J2", "Expected J2 for tile (1, 8)")
    // Additional tests to ensure surrounding tiles are correctly converted
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(0, 6)).rawValue, "G1", "Expected G1 for tile (0, 6)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(0, 9)).rawValue, "K1", "Expected K1 for tile (0, 9)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(1, 6)).rawValue, "G2", "Expected G2 for tile (1, 6)")
    XCTAssertEqual(tileToGoBoardCoordinate(TileIndexes(1, 9)).rawValue, "K2", "Expected K2 for tile (1, 9)")
  }

  func testGoBoardCoordinateToTileIndexes() {
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.A1), TileIndexes(0, 0), "Expected tile (0, 0) for coordinate A1")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.T19), TileIndexes(18, 18), "Expected tile (18, 18) for coordinate T19")
    // Test coordinates around 'I'
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.H1), TileIndexes(0, 7), "Expected tile (0, 7) for coordinate H1")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.J1), TileIndexes(0, 8), "Expected tile (0, 8) for coordinate J1")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.H2), TileIndexes(1, 7), "Expected tile (1, 7) for coordinate H2")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.J2), TileIndexes(1, 8), "Expected tile (1, 8) for coordinate J2")
    // Additional tests to ensure surrounding coordinates are correctly converted
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.G1), TileIndexes(0, 6), "Expected tile (0, 6) for coordinate G1")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.K1), TileIndexes(0, 9), "Expected tile (0, 9) for coordinate K1")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.G2), TileIndexes(1, 6), "Expected tile (1, 6) for coordinate G2")
    XCTAssertEqual(goBoardCoordinateToTileIndexes(.K2), TileIndexes(1, 9), "Expected tile (1, 9) for coordinate K2")
  }

  func testSingleBlackStoneSurroundedByWhite() {
    let deadStones = determineDeadStones(board: [
      /*          */ (.white, .E4),
      (.white, .D5), (.black, .E5), (.white, .F5),
      /*          */ (.white, .E6)
    ], lastStonePlaced: .E6)

    let expectedDeadStones: [(GoStoneColor, GoBoardCoordinate)] = [
      (.black, .E5)
    ]

    XCTAssertEqual(deadStones.count, expectedDeadStones.count, "The number of dead stones is not as expected.")
    for deadStone in deadStones {
      XCTAssertTrue(expectedDeadStones.contains(where: { $0 == deadStone }), "Unexpected dead stone found: \(deadStone)")
    }
  }

  func testTwoBlackStonesSurroundedByWhite() {
    let deadStones = determineDeadStones(board: [
      /*          */ (.white, .E3),
      (.white, .D4), (.black, .E4), (.white, .F4),
      (.white, .D5), (.black, .E5), (.white, .F5),
      /*          */ (.white, .E6)
    ], lastStonePlaced: .E6)

    let expectedDeadStones: [(GoStoneColor, GoBoardCoordinate)] = [
      (.black, .E4), (.black, .E5)
    ]

    XCTAssertEqual(deadStones.count, expectedDeadStones.count, "The number of dead stones is not as expected.")
    for deadStone in deadStones {
      XCTAssertTrue(expectedDeadStones.contains(where: { $0 == deadStone }), "Unexpected dead stone found: \(deadStone)")
    }
  }

  func testThreeBlackStonesWithOneFreeSpotSurroundedByWhite() {
    let deadStones = determineDeadStones(board: [
      /*          */ (.white, .D3), (.white, .E3),
      (.white, .C4), (.black, .D4), (.black, .E4), (.white, .F4),
      (.white, .C5), /*          */ (.black, .E5), (.white, .F5),
      /*          */ (.white, .D6), (.white, .E6)
    ], lastStonePlaced: .E6)

    let expectedDeadStones: [(GoStoneColor, GoBoardCoordinate)] = []

    XCTAssertEqual(deadStones.count, expectedDeadStones.count, "The number of dead stones is not as expected.")
  }

  func testBlackStonesInCircleWithOneFreeSpotNotDead() {
    let deadStones = determineDeadStones(board: [
      /*          */ (.white, .D3), (.white, .E3), (.white, .F3),
      (.white, .C4), (.black, .D4), (.black, .E4), (.black, .F4), (.white, .G4),
      (.white, .C5), (.black, .D5), /*          */ (.black, .F5), (.white, .G5),
      (.white, .C6), (.black, .D6), (.black, .E6), (.black, .F6), (.white, .G6),
      /*          */ (.white, .D7), (.white, .E7), (.white, .F7)
    ], lastStonePlaced: .E7)

    let expectedDeadStones: [(GoStoneColor, GoBoardCoordinate)] = []

    XCTAssertEqual(deadStones.count, expectedDeadStones.count, "The number of dead stones is not as expected.")
  }

  func testBlackStonesInCircleWithOneWhiteStoneInMiddleDead() {
    let deadStones = determineDeadStones(board: [
      /*          */ (.white, .D3), (.white, .E3), (.white, .F3),
      (.white, .C4), (.black, .D4), (.black, .E4), (.black, .F4), (.white, .G4),
      (.white, .C5), (.black, .D5), (.white, .E5), (.black, .F5), (.white, .G5),
      (.white, .C6), (.black, .D6), (.black, .E6), (.black, .F6), (.white, .G6),
      /*          */ (.white, .D7), (.white, .E7), (.white, .F7)
    ], lastStonePlaced: .E5)

    let expectedDeadStones: [(GoStoneColor, GoBoardCoordinate)] = [
      (.black, .D4), (.black, .E4), (.black, .F4),
      (.black, .D5), /*          */ (.black, .F5),
      (.black, .D6), (.black, .E6), (.black, .F6)
    ]

    XCTAssertEqual(deadStones.count, expectedDeadStones.count, "The number of dead stones is not as expected.")
    for deadStone in deadStones {
      XCTAssertTrue(expectedDeadStones.contains(where: { $0 == deadStone }), "Unexpected dead stone found: \(deadStone)")
    }
  }
}
