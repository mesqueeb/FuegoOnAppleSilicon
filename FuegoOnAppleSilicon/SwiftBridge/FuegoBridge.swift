import Foundation

public enum FuegoBridgeError: Error {
  case notStarted
  case unableToStart(String)
  case commandError(String)
}

public actor FuegoBridge {
  private var cookie: UnsafeMutableRawPointer?
  
  public init() {}
  
  deinit {
    stopEngine()
  }
  
  public func startEngine(
    srand: Int = 0,
    fixedBoardSize: Int = 0,
    maxGames: Int = -1,
    useBook: Bool = true,
    allowHandicap: Bool = true
  ) throws {
    guard cookie == nil else { return }
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
      cookie = result.result
      return
    }
    
    defer {
      fuego_free_string(result.result)
    }
    throw FuegoBridgeError.unableToStart(handleCString(result.result) ?? "Unknown error")
  }
  
  public func stopEngine() {
    guard let cookie else { return }
    fuego_free_engine(cookie)
    self.cookie = nil
  }
  
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
      return handleCString(result.result)
    } else {
      throw FuegoBridgeError.commandError(handleCString(result.result) ?? "Unknown error")
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
