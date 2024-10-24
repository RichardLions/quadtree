# Quadtree

This was inspired by [Quadtree](https://en.wikipedia.org/wiki/Quadtree).

## Setup

This repository uses the .sln/.proj files created by Visual Studio 2022 Community Edition.
Using MSVC compiler, Preview version(C++23 Preview). 

### SDL3
Running the visualisation project will show the Quadtree running.

Inputs:
1 - Render Quadtree on/off
2 - Render mouse position Quadtree test on/off
Space - Pause the simulation on/off
Enter - Switch between brute force collision tests and using Quadtree
ESC - Shutdown

### Catch2
The examples for how to use the Quadtree are written as Unit Tests.

Launching the test project in Debug or Release will run the Unit Tests.

Alternative:
Installing the Test Adapter for Catch2 Visual Studio extension enables running the Unit Tests via the Test Explorer Window. Setup the Test Explorer to use the project's .runsettings file.

### vcpkg
This repository uses vcpkg in manifest mode for it's dependencies. To interact with vcpkg, open a Developer PowerShell (View -> Terminal).

To setup vcpkg, install it via the Visual Studio installer. To enable/disable it run these commands from the Developer PowerShell:
```
vcpkg integrate install
vcpkg integrate remove
```

To add additional dependencies run:
```
vcpkg add port [dependency name]
```

To update the version of a dependency modify the overrides section of vcpkg.json. 

To create a clean vcpkg.json and vcpkg-configuration.json file run:
```
vcpkg new --application
```

### TODO
- [x] Implementation
- [ ] Unit Tests
- [x] Benchmarks
- [x] Visualisation
