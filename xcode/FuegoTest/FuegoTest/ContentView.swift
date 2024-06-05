//
//  ContentView.swift
//  FuegoTest
//
//  Created by Alexander Pototskiy on 05.06.24.
//

import SwiftUI
import RealityKit
import RealityKitContent

struct ContentView: View {
    @State private var resultText: String = ""
    
    var body: some View {
        VStack {
            Model3D(named: "Scene", bundle: realityKitContentBundle)
                .padding(.bottom, 50)

            Button("Run Unit Tests") {
                let arg0 = "test"
                let arg1 = "--no_color_output"
                let arg2 = "--log_level=test_suite"
                
                let argv = UnsafeMutablePointer<UnsafePointer<Int8>?>.allocate(capacity: 3)

                let result = arg0.withCString { (baseAddress) in
                    argv[0] = baseAddress
                    return arg1.withCString { (baseAddress) in
                        argv[1] = baseAddress
                        return arg2.withCString { (baseAddress) in
                            argv[2] = baseAddress
                            return run_unit_tests(3, argv)
                        }
                    }
                }
                resultText = (result == 0 ? "Success": "Failure") + ", see the application log for details."
            }
            TextField("Enter text here", text: $resultText)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .padding()
        }
        .padding()
    }
}

#Preview(windowStyle: .automatic) {
    ContentView()
}
