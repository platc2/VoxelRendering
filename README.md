# VoxelRendering

## Dependencies

Install the following tools:

- conan
- cmake

## Setup and build

### Linux (gcc/clang)

```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. --build=missing
cmake ..
cmake --build .
```

### Windows (Visual Studio)

```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MDd --build=missing
cmake .. -G "Visual Studio 15 2017 Win64"
cmake --build .
```

## Sources

- Block assets: https://opengameart.org/content/free-low-poly-game-asset-3d-blocks
- Perlin noise library: https://github.com/Reputeless/PerlinNoise
- LearnOpenGL.com

## clang-tidy

`run-clang-tidy-8 -p ./build/`
