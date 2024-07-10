let BOARDSIZE = 19

/// `nil` when there's no stone, or the color
typealias BoardState = [TileIndexes: GoStoneColor?]

/// Represents the indexes of a tile
struct TileIndexes: Equatable, Hashable {
  let ri: Int
  let ci: Int

  init(_ ri: Int, _ ci: Int) {
    self.ri = ri
    self.ci = ci
  }
}

/// Helper function to convert TileIndexes to GoBoardCoordinate
func tileToGoBoardCoordinate(_ tile: TileIndexes) -> GoBoardCoordinate {
  let colChar = Character(UnicodeScalar(tile.ci + (tile.ci >= 8 ? 66 : 65))!)
  return GoBoardCoordinate(rawValue: "\(colChar)\(tile.ri + 1)")!
}

/// Helper function to convert GoBoardCoordinate to TileIndexes
func goBoardCoordinateToTileIndexes(_ coordinate: GoBoardCoordinate) -> TileIndexes {
  let ri = Int(coordinate.rawValue.dropFirst().prefix(2))! - 1
  let colChar = coordinate.rawValue.prefix(1).utf8.first!
  let col = colChar > Character("I").utf8.first! ? Int(colChar - Character("A").utf8.first!) - 1 : Int(colChar - Character("A").utf8.first!)
  return TileIndexes(ri, col)
}

/// Helper function to check if a position is within bounds
func inBounds(_ ri: Int, _ col: Int) -> Bool {
  return ri >= 0 && ri < BOARDSIZE && col >= 0 && col < BOARDSIZE
}

/// Helper function to perform flood-fill and find connected groups
func findGroup(
  _ tile: TileIndexes,
  _ boardState: BoardState,
  visited: inout Set<TileIndexes>
) -> (group: [TileIndexes], hasLiberty: Bool) {
  var group: [TileIndexes] = []
  var hasLiberty = false

  if !inBounds(tile.ri, tile.ci) { return (group: group, hasLiberty: hasLiberty) }
  if visited.contains(tile) { return (group: group, hasLiberty: hasLiberty) }

  let directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
  visited.insert(tile)

  guard let tileColor = boardState[tile] else { return (group: group, hasLiberty: hasLiberty) }

  group.append(tile)

  for (rowDirection, colDirection) in directions {
    let nextRi = tile.ri + rowDirection
    let nextCi = tile.ci + colDirection
    let nextT = TileIndexes(nextRi, nextCi)

    if let nextColor = boardState[nextT] {
      if nextColor == tileColor {
        let nestedResults = findGroup(nextT, boardState, visited: &visited)
        group.append(contentsOf: nestedResults.group)
        if nestedResults.hasLiberty { hasLiberty = true }
      } else {
        // black stone
        continue
      }
    } else {
      // no stone
      hasLiberty = true
    }
  }

  return (group: group, hasLiberty: hasLiberty)
}

/// Given an array of Go Stones and their coordinates, determine the stones that are dead.
public func determineDeadStones(board: [(GoStoneColor, GoBoardCoordinate)], lastStonePlaced: GoBoardCoordinate?) -> [(GoStoneColor, GoBoardCoordinate)] {
  // Initialize the board
  var boardState: BoardState = [:]

  // Populate the board with the given stones
  for (color, coordinate) in board {
    let t = goBoardCoordinateToTileIndexes(coordinate)
    boardState[t] = color
  }

  var visited: Set<TileIndexes> = Set()
  var dead: [TileIndexes] = []

  // Traverse the board to find all groups and check their liberties
  for tile in boardState.keys {
    if !visited.contains(tile) {
      let (tileGroup, hasLiberty) = findGroup(tile, boardState, visited: &visited)
      if !hasLiberty {
        dead.append(contentsOf: tileGroup)
      }
    }
  }

  var result: [(GoStoneColor, GoBoardCoordinate)] = []
  for deadTile in dead {
    let coordinate = tileToGoBoardCoordinate(deadTile)
    if coordinate == lastStonePlaced { continue }
    guard let color = boardState[deadTile] ?? nil else { continue }
    result.append((color, coordinate))
  }
  return result
}
