
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
    // Products define the executables and libraries a package produces, making them visible to other packages.
    .library(
      name: "FuegoOnAppleSilicon",
      targets: ["FuegoSwiftBridge", "FuegoCBridge", "Fuego"]
    ),
  ],
  targets: [
    // Targets are the basic building blocks of a package, defining a module or a test suite.
    // Targets can depend on other targets in this package and products from dependencies.
    .binaryTarget(
      name: "Fuego",
      url: "https://github.com/mesqueeb/FuegoOnAppleSilicon/releases/download/v0.0.5/Fuego.xcframework.zip",
      checksum: "f443e963a9dbd7febc7ddc6159c3133a374e900b7b08f0c17275025b6242c084"
    ),
    .target(
      name: "FuegoCBridge",
      dependencies: ["Fuego"],
      path: "FuegoOnAppleSilicon/CBridge"
    ),
    .target(
      name: "FuegoSwiftBridge",
      dependencies: ["FuegoCBridge", "Fuego"],
      path: "FuegoOnAppleSilicon/SwiftBridge"
    ),
  ]
)
