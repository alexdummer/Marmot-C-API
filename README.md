# Marmot C API

This directory contains the C API wrappers for the [Marmot](https://github.com/marmot-project/Marmot) Finite Element Library. It provides an opaque C interface that allows C programs to hook directly into the core `MarmotElement` and `MarmotElementFactory` C++ classes safely.

## Features
- **Object-Oriented Opaqueness**: Elements are created and tracked via a managed `MarmotElement_t` pointer.
- **Robust Exception Catching**: Invalid inputs passed from the C boundary are caught internally; `std::invalid_argument` and `std::runtime_error` won't crash the orchestrating C process.
- **Flat C Arrays**: Transparent parameter injections using standard continuous block `double*` arrays.

## Build Instructions

The build relies on finding the native compiled Marmot library. You **must explicitly map** the library path and the include path using CMake variables when configuring.

```bash
mkdir build && cd build

# Replace these paths with the actual locations of your Marmot build and headers
cmake \
  -DMARMOT_INCLUDE_DIR=/path/to/marmot/include \
  -DMARMOT_LIB_DIR=/path/to/marmot/build \
  ..

make -j
```

Once built, the dynamically linked `libmarmot_c_api.so` binary will be available to link against your C executable.

## Example

A comprehensive test example demonstrating full C API capability mappings is provided in the `examples/` directory.

To run it after compiling:
```bash
LD_LIBRARY_PATH=/path/to/marmot/build:$(pwd) ./example_c
```

## Contributing
This wrapper adopts the Marmot C++ framework style. Validation and formatting configs (`.clang-format` and `.pre-commit-config.yaml`) are provided on the root level to seamlessly format C source codes before commits.
