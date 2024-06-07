//
//  FuegoBridge.swift
//  FuegoIteropSample
//
//  Created by Alexander Pototskiy on 04.06.24.
//

import Foundation

public enum FuegoBridgeError: Error {
  case alreadyStarted
  case notStarted
  case unableToStart(String)
  case commandError(String)
}

public class FuegoBridge {
  public var srand: Int = 0
  public var fixedBoardSize: Int = 0
  public var maxGames: Int = -1
  public var useBook: Bool = true
  public var allowHandicap: Bool = true
    
  private var cookie: UnsafeMutableRawPointer?
    
  deinit {
    stopEngine()
  }
    
  public func startEngine() throws {
    guard cookie == nil else {
      throw FuegoBridgeError.alreadyStarted
    }
    let result = fuego_create_engine(nil, nil, Int32(srand), Int32(fixedBoardSize), Int32(maxGames), useBook ? 1 : 0, allowHandicap ? 1 : 0)
    if result.success == 0 {
      cookie = result.result
    } else {
      defer {
        fuego_free_string(result.result)
      }
      throw FuegoBridgeError.unableToStart(handleCString(result.result) ?? "Unknown error")
    }
  }
    
  public func stopEngine() {
    guard let cookie else { return }
    fuego_free_engine(cookie)
    self.cookie = nil
  }
    
  public func submitCommand(_ command: String, completionHandler: (String?, Error?) -> Void) {
    guard let cookie else {
      completionHandler(nil, FuegoBridgeError.notStarted)
      return
    }
    let count = command.utf8.count

    let result = command.withCString { baseAddress in
      fuego_process_command(cookie, baseAddress, UInt64(count))
    }
    defer {
      fuego_free_string(result.result)
    }
    if result.success == 0 {
      completionHandler(handleCString(result.result), nil)
    } else {
      completionHandler(nil, FuegoBridgeError.commandError(handleCString(result.result) ?? "Unknown error"))
    }
  }
    
  func handleCString(_ str: UnsafeRawPointer?) -> String? {
    if let ptr = str {
      let chararr = ptr.assumingMemoryBound(to: CChar.self)
      return String(cString: chararr, encoding: String.Encoding.utf8)
    }
    return nil
  }
}
