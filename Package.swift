
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
      targets: ["FuegoSwiftBridge", "FuegoCBridge"]
		),
  ],
  targets: [
    // Targets are the basic building blocks of a package, defining a module or a test suite.
    // Targets can depend on other targets in this package and products from dependencies.
    .binaryTarget(
      name: "FuegoXCFramework",
      url: "https://github.com/mesqueeb/FuegoOnAppleSilicon/releases/download/v0.0.1/Fuego-0.0.1.xcframework.zip",
      checksum: "5c49ee6ea96a9152485092c4d88253edc2fad403ffb83837be979accc063519b"
    ),
    .target(
      name: "FuegoCBridge",
      dependencies: ["FuegoXCFramework"],
      path: "FuegoOnAppleSilicon/CBridge"
		),
    .target(
      name: "FuegoSwiftBridge",
      dependencies: ["FuegoCBridge"],
      path: "FuegoOnAppleSilicon/SwiftBridge"
		),
  ]
)