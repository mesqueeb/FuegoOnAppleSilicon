# Fuego On Apple Silicon ⚫️⚪️

[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dswift-versions)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)
[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dplatforms)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)

```
.package(url: "https://github.com/mesqueeb/FuegoOnAppleSilicon", from: "0.1.7")
```

Fuego On Apple Silicon is the Fuego C++ Go engine to play and analyse Go games. Built as multi-platform XCframework for iOS and visionOS. Wrapped as a modernised Swift Package that can be included in any Swift project and can build on all Apple platforms.

## Improvements over original Fuego source code

This project
- Modernised the original C++ source code to compile with modern (20+) C++ standard and the latest (1.85) Boost version
- Migrated to a CMake-based build system so the project can be supported with a more modern IDEs
- Created a new [SH build script](./build-xcframework.sh) that builds XCframeworks for iOS, visionOS (xrOS), macOS, and related simulators
- Wrapped everything nicely into a Swift Package that includes the pre-compiled binaries, has a Swift bridge class so that it can be easily used in any Swift project

It builds upon a fork of both the original [Fuego C++ code on Source Forge](http://fuego.sourceforge.net/) and the [Fuego on iOS](https://github.com/herzbube/fuego-on-ios) repository. (Fuego on iOS only compiles to iOS and relies on GNU++98 & Boost 1.75.0)

## Development

### Running Unit Tests

TODO

### Building the XCframework

```
./build-xcframework.sh
```

This script builds `gtpengine`, `smartgame`, `go` and `gouct` libraries, merges them into a single lib (per platform), and packs libs from different platforms into `./build/Fuego.xcframework`.

This script also downloads Boost (before building), however, now there is no need to build Boost libraries because the Boost libraries that need to be built are no longer used for these fuego subprojects. Only `fuegomain` and `fuegotest` depend on the `boost.program_options` library that needs to be built. But they are not part of `Fuego.xcframework`.

You can check this build file and enable / disable output by toggling `if true; then` to `if false; then`. Currently it is set to build:

- xros-simulator arm64,x86_64
- xros arm64
- ios arm64
- ios simulator arm64,x86_64
- osx arm64,x86_64

## Attributions

The [Fuego on iOS](https://github.com/herzbube/fuego-on-ios) repository was a source of inspiration which gave birth to this library.

Modernising the C++ code and build scripts was done with the help of [Apotocki](https://github.com/apotocki).

Thank you, everybody!
