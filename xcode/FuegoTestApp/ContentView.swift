import SwiftUI

let testCommands = [
  "boardsize 19",
  "clear_board",
  "komi 6.5",
  "play b D4",
  "genmove w",
  "play b E4",
  "genmove w",
  // "set_free_handicap d16 q16 d4 q4",
  // "genmove w"
]

struct ContentView: View {
  @State private var sentCommands: [String] = []
  @State private var testCommandIndex: Int = 0
  @State private var inputText: String = testCommands[0]
  @State private var fuegoResponse: String = ""
  @State private var fuegoError: String = ""
    
  @State var fuegoBridge: FuegoBridge? = nil

  init() {
    guard let bundleBookUrl = Bundle.main.url(forResource: "book", withExtension: "dat") else {
      fatalError("can't retrieve bundle path")
    }
        
    // Just example. Much better to copy book into working dir
    let bookDir = bundleBookUrl.deletingLastPathComponent()
    if !FileManager.default.changeCurrentDirectoryPath(bookDir.path) {
      fatalError("can't set book directory directory")
    }
  }
    
  func handleResponse(_ response: String) {
    print("response →", response)
    fuegoResponse = response
        
    testCommandIndex += 1
    if testCommandIndex == testCommands.count {
      inputText = ""
    } else {
      inputText = testCommands[testCommandIndex]
    }
        
    fuegoError = ""
  }
    
  // current implementation using a callback function:
  func submitCommand() {
    Task.detached {
      guard let fuegoBridge else { return }
      sentCommands.append(inputText.lowercased())
      do {
        if let response = try await fuegoBridge.submitCommand(inputText.lowercased()) {
          handleResponse(response)
        }
      } catch {
        fuegoResponse = "error"
        fuegoError = "\(error)" // error.localizedDescription
      }
    }
  }
    
  var body: some View {
    VStack(spacing: 16) {
      TextField("Enter text here", text: $inputText)
        .textFieldStyle(RoundedBorderTextFieldStyle())
        .padding()
            
      Button("Submit") { submitCommand() }
        .padding()
        .foregroundColor(.white)
        .background(Color.blue)
        .cornerRadius(10)
            
      VStack {
        ForEach(sentCommands, id: \.self) { command in
          Text(command).font(.caption)
        }
      }
            
      Text("Fuego Response: \(fuegoResponse)")
            
      Text("Error: \(fuegoError)")
    }
    .padding()
    .task {
      if fuegoBridge == nil {
        fuegoBridge = FuegoBridge()
        print("AI connecting...")
        do {
          try await fuegoBridge!.startEngine()
          print("AI connected")
        } catch {
          print("AI didn't connect, error: \(error)")
          fuegoError = "\(error)"
        }
      }
    }
    .onDisappear {
      Task {
        await fuegoBridge?.stopEngine()
      }
    }
  }
}
