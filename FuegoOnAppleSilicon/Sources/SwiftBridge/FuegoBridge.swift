import Foundation
import FuegoCBridge

public enum FuegoBridgeError: Error {
  case notStarted
  case unableToStart(String)
  case commandError(String)
}

public enum GenMoveResponse: Sendable {
  case Coordinate(GoBoardCoordinate)
  case Other(String?)

  public var description: String {
    return switch self {
    case .Coordinate(let value): "\(value)"
    case .Other(let value): "Other(\(value ?? "nil")"
    }
  }
}

public actor FuegoBridge {
  private var cookie: UnsafeMutableRawPointer?

  public init() {}

  public func startEngine(
    srand: Int = 0,
    fixedBoardSize: Int = 0,
    maxGames: Int = -1,
    useBook: Bool = true,
    allowHandicap: Bool = true
  ) throws {
    guard self.cookie == nil else { return }
    let result = fuego_create_engine(
      nil,
      nil,
      Int32(srand),
      Int32(fixedBoardSize),
      Int32(maxGames),
      useBook ? 1 : 0,
      allowHandicap ? 1 : 0
    )

    if result.success == 0 {
      self.cookie = result.result
      return
    }

    defer {
      fuego_free_string(result.result)
    }
    throw FuegoBridgeError.unableToStart(self.handleCString(result.result) ?? "Unknown error")
  }

  public func stopEngine() {
    guard let cookie else { return }
    fuego_free_engine(cookie)
    self.cookie = nil
  }

  @discardableResult
  public func submitCommand(_ command: String) throws -> String? {
    guard let cookie else {
      throw FuegoBridgeError.notStarted
    }
    let count = command.utf8.count

    let result = command.withCString { baseAddress in
      fuego_process_command(cookie, baseAddress, UInt64(count))
    }
    defer {
      fuego_free_string(result.result)
    }
    if result.success == 0 {
      return self.handleCString(result.result)
    } else {
      throw FuegoBridgeError.commandError(self.handleCString(result.result) ?? "Unknown error")
    }
  }

  private func handleCString(_ str: UnsafeRawPointer?) -> String? {
    if let ptr = str {
      let chararr = ptr.assumingMemoryBound(to: CChar.self)
      return String(cString: chararr, encoding: String.Encoding.utf8)
    }
    return nil
  }
}

// MARK: - Common Commands

public extension FuegoBridge {
  func genmove(_ color: GoStoneColor) throws -> GenMoveResponse {
    guard let response = try self.submitCommand("genmove \(color)") else {
      return GenMoveResponse.Other(nil)
    }
    guard let coordinate = GoBoardCoordinate(rawValue: response) else {
      return GenMoveResponse.Other(response)
    }
    return GenMoveResponse.Coordinate(coordinate)
  }

  func play(_ color: GoStoneColor, _ coordinate: GoBoardCoordinate) throws {
    try self.submitCommand("play \(color) \(coordinate.rawValue)")
  }

  /// Use this to clear the Go board
  func clearboard() throws {
    try self.submitCommand("clear_board")
  }

  /// Eg. boardsize(19)
  func boardsize(_ size: Int) throws {
    try self.submitCommand("boardsize \(size)")
  }

  /// Eg. komi(6.5)
  func komi(_ value: Float) throws {
    try self.submitCommand("komi \(value)")
  }

  /// Use this to clear the Go board
  func showboard() throws -> String? {
    return try self.submitCommand("showboard")
  }
}
