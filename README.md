# Tsunami data plot utilities

Tsunami plot utilities used by the tsunami simulation models (tsunamin2cuda,
tsunamif1cuda, etc.) to generate GMT-based maps and wave diagrams.

## Requirements

- GNU/Linux: build tools (compiler, linker, make)
- Windows: Visual Studio 2022+
- CMake 3.24+ available in PATH
- Generic Mapping Tools (GMT) available in PATH
- GhostScript

## Setup

Open VS Code and wait for the automatic configuration to complete.
If the VSCode Action Buttons extension is installed, there will be several buttons
at the bottom bar: Clean, Clean & Build, etc.

Generate the library in Release mode before building any model that depends on it.

## Manual setup

Linux:

```sh
mkdir build
cd build
cmake -G "Unix Makefiles" ..
```

Windows (Visual Studio 2022+):

```bat
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
```

## Building

Debug configuration:

```sh
cd build
cmake --build . --config Debug
```

Release configuration:

```sh
cd build
cmake --build . --config Release
```

## Cleaning

Clean debug build:

```sh
cd build
cmake --build . --config Debug --target clean
```

Clean release build:

```sh
cd build
cmake --build . --config Release --target clean
```

## Copyright

MIT License

Copyright (c) 2025 Erwin Meza Vega <emezav@unicauca.edu.co> <emezav@gmail.com>

See LICENSE file for full details.
