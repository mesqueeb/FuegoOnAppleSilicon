import { execSync } from 'node:child_process'
import { writeFileSync } from 'node:fs'
import { join, basename } from 'node:path'
import { createRequire } from 'node:module'
import semver from 'semver'
import { replaceRegex } from 'replace-regex'
import select from'@inquirer/select'

const require = createRequire(import.meta.url)
const { version } = require('../package.json')

const bump = await select({
  message: 'Select the version bump:',
  choices: ['patch','minor', 'major'].map((value) => ({ name: value, value })),
})

const PATH_ROOT = join(process.cwd())
const PATH_BUILD = join(PATH_ROOT, './build')

const nextVersion = semver.inc(version, bump)
const frameworkPath = join(PATH_BUILD, './Fuego.xcframework')
const zipFilename = `Fuego-${nextVersion}.xcframework.zip`
const zipFullPath = join(PATH_BUILD, `./${zipFilename}`)

// Change directory to the build folder to create the zip
const frameworkName = basename(frameworkPath)

execSync(`cd ${PATH_BUILD} && zip -r ${zipFilename} ${frameworkName}`, { stdio: 'inherit' })

const zipChecksum = execSync(`swift package compute-checksum ${zipFullPath}`).toString().trim()

// Write the updated Package.swift
writeFileSync(join(PATH_ROOT, 'Package.swift'), `
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
      targets: ["FuegoOnAppleSilicon", "FuegoCBridge"]
    ),
  ],
  targets: [
    .binaryTarget(
      name: "FuegoXCFramework",
      url: "https://github.com/mesqueeb/FuegoOnAppleSilicon/releases/download/v${nextVersion}/${zipFilename}",
      checksum: "${zipChecksum}"
    ),
    .target(
      name: "FuegoCBridge",
      dependencies: ["FuegoXCFramework"],
      path: "FuegoOnAppleSilicon/CBridge"
    ),
    .target(
      name: "FuegoOnAppleSilicon",
      dependencies: ["FuegoCBridge"],
      path: "FuegoOnAppleSilicon/SwiftBridge"
    ),
    .testTarget(
      name: "FuegoOnAppleSiliconTests",
      dependencies: ["FuegoOnAppleSilicon"]
    ),
  ]
)
`)

await replaceRegex({ 
  files: join(PATH_ROOT, 'README.md'),
  from: /from: "\d+\.\d+\.\d+"/g,
  to: `from: "${nextVersion}"`,
})

execSync(`git add .`, { stdio: 'inherit' })
execSync(`git commit -m "chore: 🎉 Release version ${nextVersion}"`, { stdio: 'inherit' })

execSync(`np ${bump} --no-cleanup --no-tests --no-publish`)
execSync(`open ${PATH_BUILD}`)
