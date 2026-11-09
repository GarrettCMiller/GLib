# GLib — Garrett's C++ game framework

GLib is a historical C++ framework by Garrett Miller, developed alongside
projects including [Doomeroids](https://github.com/GarrettCMiller/Doomeroids)
and [Quantum](https://github.com/GarrettCMiller/Quantum). This project is
separate from the unrelated GLib library associated with the GNOME ecosystem.

The code explores the lower-level services behind Windows games and graphics
applications:

- Direct3D 9 rendering, textures, sprites, meshes and effects.
- Input, sound, resource management and utility classes.
- Logging, profiling, custom collections and memory-management experiments.
- Mathematical expression parsing and vector-graphics experiments.
- Integration with external libraries, including a wrapper around PhysX.

## Repository guide

- `GLib/`: framework implementation and Visual Studio project files.
- `inc/`: public headers and included third-party material.
- `lib/`: historical library files.

## Status and dependencies

This repository is an archival work-in-progress and portfolio code sample. It
has not been rebuilt or run for this README. The code depends on older Windows
and DirectX tooling, and different projects may require compatible historical
versions of GLib and its dependencies. Individual features vary in maturity.

This repository includes or integrates third-party components such as TinyXML,
HawkNL and PhysX. Those projects have their own authors and terms. Confirm
third-party notices and redistribution rights before repackaging. This README
does not assign a new license.
