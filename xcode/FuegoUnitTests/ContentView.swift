import RealityKit
import SwiftUI

struct ContentView: View {
  @State private var resultText: String = ""

  func runUnitTests() {
    print("start!")
    let arg0 = "test"
    let arg1 = "--no_color_output"
    let arg2 = "--log_level=test_suite"

    let argv = UnsafeMutablePointer<UnsafePointer<Int8>?>.allocate(capacity: 3)

    let result = arg0.withCString { baseAddress in
      argv[0] = baseAddress
      return arg1.withCString { baseAddress in
        argv[1] = baseAddress
        return arg2.withCString { baseAddress in
          argv[2] = baseAddress
          return run_unit_tests(3, argv)
        }
      }
    }
    resultText = (result == 0 ? "Success" : "Failure") + ", see the application log for details."
  }

  var body: some View {
    VStack {
      Button(action: runUnitTests) {
        Text("Run Unit Tests")
      }
      Text("Result: \(resultText)")
        .padding()
    }
    .padding()
  }
}
