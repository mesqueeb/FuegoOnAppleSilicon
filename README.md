# Fuego On Apple Silicon ⚫️⚪️

[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dswift-versions)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)
[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2Fmesqueeb%2FFuegoOnAppleSilicon%2Fbadge%3Ftype%3Dplatforms)](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon)

```
.package(url: "https://github.com/mesqueeb/FuegoOnAppleSilicon", from: "0.3.2")
```

Fuego On Apple Silicon is the Fuego C++ Go engine to play and analyse Go games. Built as multi-platform XCframework for iOS, macOS and visionOS. Wrapped as a modernised Swift Package that can be included in any Swift project and can build on all Apple platforms.

## Improvements over original Fuego source code

This project
- Modernised the original C++ source code to compile with modern (20+) C++ standard
- No longer requires the entire Boost library to be built, but now relies only on the Boost header libraries and is compatible with the latest Boost version (1.85)
- Migrated to a CMake-based build system so the project can be supported with a more modern IDEs
- Created a new [SH build script](./build-xcframework.sh) that builds XCframeworks for iOS, macOS, visionOS (xrOS), and related simulators
- Wrapped everything nicely into a Swift Package that includes the pre-compiled binaries, has a Swift bridge class so that it can be easily used in any Swift project

It builds upon a fork of both the original [Fuego C++ code on Source Forge](http://fuego.sourceforge.net/) and the [Fuego on iOS](https://github.com/herzbube/fuego-on-ios) repository. (Fuego on iOS only compiles to iOS and relies on GNU++98 & Boost 1.75.0)

## Installation Issues

When installing the package via Xcode 14.x you might [run into the error:](https://stackoverflow.com/questions/76556005/swift-package-manager-failed-with-invalid-archive-returned-from-xy-which-is-req/78592676)

> invalid archive returned from https://github.com/...

The fix is to remove the dependency again, quit Xcode, and execute:

```bash
rm -rf $HOME/Library/Caches/org.swift.swiftpm/
```

Then re-open Xcode, re-add the package again and it should install correctly. Sadly you'll need to do this _every time_ you need to update this package, (until Xcode fixes this issue with packages that uses binary targets).

## Usage

```swift
import FuegoOnAppleSilicon

let bridge = FuegoBridge()

do {
  try await bridge.startEngine()
  
  // You need to feed the engine GTP (Go Text Protocol) strings to be able to request moves
  try await bridge.submitCommand("boardsize 19")
  try await bridge.submitCommand("clear_board")
  try await bridge.submitCommand("komi 6.5")
  try await bridge.submitCommand("play b D16")
  
  if let whiteMove = try await bridge.submitCommand("genmove w") {
    print("Fuego plays white:", whiteMove) // Eg. "Q4"
  }
  if let boardState = try await bridge.submitCommand("showboard") {
    print("Fuego shows the board:", boardState)
  }

  // Stop the engine when you're done:
  bridge.stopEngine()
} catch {
  print("Something went wrong... error:", error)
}
```

The library comes with some useful types and helper functions, be sure to check out the [Swift wrapper's source code here](./FuegoOnAppleSilicon/SwiftBridge/)

### Sample Project

There is a sample Xcode project provided as part of the repo that you reference here: [FuegoTestApp](./xcode/FuegoTestApp/)

### Documentation

See the [documentation](https://swiftpackageindex.com/mesqueeb/FuegoOnAppleSilicon/documentation/fuegoonapplesilicon) for more info.

## Development

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

### Running Unit Tests

`build-xcframework.sh` is a multipurpose build script. It also creates another `FuegoTest.xcframework` by merging the `simpleplayers` and `unittestmain` subprojects. While `unittestmain` is intended to be an executable, it's built as a static library where the `main` method is renamed to `run_unit_tests` with the same signature. So any iOS/macOS/visionOS application is able to run Fuego unit tests just by embedding this `FuegoTest.xcframework` (in addition to `Fuego.xcframework`) and calling the `run_unit_tests` method. An example of such an application is xcode\FuegoTest\FuegoTest.xcodeproj, which represents a simple SwitUI application where tests are started when a button is clicked.

## Attributions

The [Fuego on iOS](https://github.com/herzbube/fuego-on-ios) repository was a source of inspiration which gave birth to this library.

Modernising the C++ code and build scripts was done with the help of [Alexander Pototskiy](https://github.com/apotocki).

Thank you, everybody!

# Other Projects

Also check out Micro-Max on Apple Silicon ♟️, the µ-Max Chess engine wrapped for Apple Silicon.

→ [github.com/mesqueeb/MicroMaxOnAppleSilicon](https://github.com/mesqueeb/MicroMaxOnAppleSilicon)

# Original Fuego C++ README

```md
Description
===========

Fuego is a collection of C++ libraries for developing software for the
game of Go. It includes a Go player using Monte-Carlo tree search.

The initial version of the code was released by the Computer Go group at
the University of Alberta and is based in parts on the previous projects
Smart Game Board and Explorer.

Contact
=======

Fuego project page: http://sourceforge.net/projects/fuego/

Fuego Trac and Wiki: http://sourceforge.net/apps/trac/fuego/

Copyright
=========

See AUTHORS for the list of copyright holders on Fuego.

License
=======

Fuego is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fuego is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Fuego.  If not, see <http://www.gnu.org/licenses/>.

Compilation
===========

See INSTALL for generic compilation instructions from GNU Automake.
The file doc/doxygen/general/pages/autotools.cpp contains additional and
Fuego specific documentation.

Documentation
=============

The developer documentation for the libraries and applications can be
created with Doxygen (http://www.doxygen.org). There is a makefile in
doc/doxygen/Makefile.

The user manual for the main Go player is in doc/manual/

AUTHORS
=======

Fuego authors
=============

Martin Mueller
Markus Enzenberger
Fan Xie

Previous authors
================

Anders Kierulf (original Smart Game Board code)

Contributors
============

David Silver (SpDumbTactic player, improvements to SgHash)
Xiaozhen Niu (parts of GoRegionUtil, GoSafetyUtil)
Broderick Arneson (autobook; many enhancements)
Richard Segal (improvements to parallel search)
Chris Rosin (additive knowledge, Greenpeep-style patterns)
Timothy Yau (Greenpeep-style patterns)
Saradha Sankaran (deterministic mode; build improvements)
Aja Huang
Jakub Pawlewicz (better hash table; df-pn code)
```
