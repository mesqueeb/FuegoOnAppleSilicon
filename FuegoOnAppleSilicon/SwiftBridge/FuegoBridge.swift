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

/// The Go Board Coordinates from A1 until T19
///
/// When a Coordinate is supposed to be returned but something is returned instead, it will be case `Other(String?)`
public enum GoBoardCoordinate: String, CustomStringConvertible, Sendable {
  case A19, B19, C19, D19, E19, F19, G19, H19, J19, K19, L19, M19, N19, O19, P19, Q19, R19, S19, T19,
       A18, B18, C18, D18, E18, F18, G18, H18, J18, K18, L18, M18, N18, O18, P18, Q18, R18, S18, T18,
       A17, B17, C17, D17, E17, F17, G17, H17, J17, K17, L17, M17, N17, O17, P17, Q17, R17, S17, T17,
       A16, B16, C16, D16, E16, F16, G16, H16, J16, K16, L16, M16, N16, O16, P16, Q16, R16, S16, T16,
       A15, B15, C15, D15, E15, F15, G15, H15, J15, K15, L15, M15, N15, O15, P15, Q15, R15, S15, T15,
       A14, B14, C14, D14, E14, F14, G14, H14, J14, K14, L14, M14, N14, O14, P14, Q14, R14, S14, T14,
       A13, B13, C13, D13, E13, F13, G13, H13, J13, K13, L13, M13, N13, O13, P13, Q13, R13, S13, T13,
       A12, B12, C12, D12, E12, F12, G12, H12, J12, K12, L12, M12, N12, O12, P12, Q12, R12, S12, T12,
       A11, B11, C11, D11, E11, F11, G11, H11, J11, K11, L11, M11, N11, O11, P11, Q11, R11, S11, T11,
       A10, B10, C10, D10, E10, F10, G10, H10, J10, K10, L10, M10, N10, O10, P10, Q10, R10, S10, T10,
       A9, B9, C9, D9, E9, F9, G9, H9, J9, K9, L9, M9, N9, O9, P9, Q9, R9, S9, T9,
       A8, B8, C8, D8, E8, F8, G8, H8, J8, K8, L8, M8, N8, O8, P8, Q8, R8, S8, T8,
       A7, B7, C7, D7, E7, F7, G7, H7, J7, K7, L7, M7, N7, O7, P7, Q7, R7, S7, T7,
       A6, B6, C6, D6, E6, F6, G6, H6, J6, K6, L6, M6, N6, O6, P6, Q6, R6, S6, T6,
       A5, B5, C5, D5, E5, F5, G5, H5, J5, K5, L5, M5, N5, O5, P5, Q5, R5, S5, T5,
       A4, B4, C4, D4, E4, F4, G4, H4, J4, K4, L4, M4, N4, O4, P4, Q4, R4, S4, T4,
       A3, B3, C3, D3, E3, F3, G3, H3, J3, K3, L3, M3, N3, O3, P3, Q3, R3, S3, T3,
       A2, B2, C2, D2, E2, F2, G2, H2, J2, K2, L2, M2, N2, O2, P2, Q2, R2, S2, T2,
       A1, B1, C1, D1, E1, F1, G1, H1, J1, K1, L1, M1, N1, O1, P1, Q1, R1, S1, T1

  /// The string used when this enum is interpolated
  public var description: String {
    return self.rawValue
  }
}

/// The Go Stone color
public enum GoStoneColor: String, CustomStringConvertible, Sendable {
  case black, white

  /// The string used when this enum is interpolated
  public var description: String {
    switch self {
    case .black: return "b"
    case .white: return "w"
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
