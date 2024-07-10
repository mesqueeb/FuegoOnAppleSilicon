// swift-tools-version: 5.10
// The swift-tools-version declares the minimum version of Swift required to build this package.
import PackageDescription

/// This `Package.swift` exists to be able to import the package in an Xcode project in the same repository.
///
/// Compared to the repository root `Package.swift`, everything is the same but the target paths.
let package = Package(
  name: "FuegoOnAppleSilicon",
  platforms: [.iOS(.v14), .macOS(.v13)],
  products: [
    .library(
      name: "FuegoOnAppleSilicon",
      targets: ["FuegoOnAppleSilicon", "FuegoCBridge"]
    ),
  ],
  targets: [
    .binaryTarget(
      name: "FuegoXCFramework",
      url: "https://github.com/mesqueeb/FuegoOnAppleSilicon/releases/download/v0.3.1/Fuego-0.3.1.xcframework.zip",
      checksum: "94eda59dc7c819c4a9a69618d62232da410bda9b4576085ed25bce7bdc26bf2b"
    ),
    .target(
      name: "FuegoCBridge",
      dependencies: ["FuegoXCFramework"],
      path: "Sources/CBridge"
    ),
    .target(
      name: "FuegoOnAppleSilicon",
      dependencies: ["FuegoCBridge"],
      path: "Sources/SwiftBridge"
    ),
    .testTarget(
      name: "FuegoOnAppleSiliconTests",
      dependencies: ["FuegoOnAppleSilicon"],
      path: "Tests"
    ),
  ]
)
