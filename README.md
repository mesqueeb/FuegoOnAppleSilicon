# Fuego Apple Silicon ⚫️⚪️

[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dswift-versions)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)
[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dplatforms)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)

```
.package(url: "https://github.com/mesqueeb/FuegoOnAppleSilicon", from: "0.0.1")
```

Fuego On Apple Silicon is a Swift Package that provides the Fuego Go engine C++ code packaged as a modern Swift package that is buildable on all Apple platforms.

It builds upon a fork of both the original [Fuego C++ code on Source Forge](http://fuego.sourceforge.net/) and the [Fuego on iOS](https://github.com/herzbube/fuego-on-ios) repository. (Fuego on iOS only compiles to iOS and relies on GNU++98 & Boost 1.75.0)

This project
- Modernised the original C++ source code to compile with modern (20+) C++ standard and the latest (1.85) Boost version
- Migrated to a CMake-based build system so the project can be supported with a more modern IDEs
- Created a new [SH build script](./build-xcframework.sh) that builds XCframeworks for iOS, visionOS (xrOS), and related simulators
- Wrapped everything nicely into a Swift Package that includes the pre-compiled binaries, has a Swift bridge class so that it can be easily used in any Swift project

## Attributions

The original "[Fuego on iOS](https://github.com/herzbube/fuego-on-ios)" was the inspiration that gave birth to this library.

Modernising everything was done with the help of [Apotocki](https://github.com/apotocki).

Thank you, everybody!
