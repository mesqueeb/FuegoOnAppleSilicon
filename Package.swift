// swift-tools-version: 5.10
// The swift-tools-version declares the minimum version of Swift required to build this package.

// =================================================================
// || Do not edit! This file is generated by node/zipFramework.js ||
// =================================================================

import PackageDescription

let package = Package(
  name: "FuegoOnAppleSilicon",
  platforms: [.iOS(.v14), .macOS(.v13), .visionOS(.v1)],
  products: [
    .library(
      name: "FuegoOnAppleSilicon",
      targets: ["FuegoOnAppleSilicon", "FuegoCBridge"]
    ),
  ],
  targets: [
    .binaryTarget(
      name: "FuegoXCFramework",
      url: "https://github.com/mesqueeb/FuegoOnAppleSilicon/releases/download/v0.3.3/Fuego-0.3.3.xcframework.zip",
      checksum: "94eda59dc7c819c4a9a69618d62232da410bda9b4576085ed25bce7bdc26bf2b"
    ),
    .target(
      name: "FuegoCBridge",
      dependencies: ["FuegoXCFramework"],
      path: "FuegoOnAppleSilicon/Sources/CBridge"
    ),
    .target(
      name: "FuegoOnAppleSilicon",
      dependencies: ["FuegoCBridge"],
      path: "FuegoOnAppleSilicon/Sources/SwiftBridge"
    ),
    .testTarget(
      name: "FuegoOnAppleSiliconTests",
      dependencies: ["FuegoOnAppleSilicon"],
      path: "FuegoOnAppleSilicon/Tests"
    ),
  ]
)
