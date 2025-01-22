# Archero-C

## Installation

### SDL
```
git clone https://github.com/libsdl-org/SDL
cd SDL
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

### SDL_image
```
git clone https://github.com/libsdl-org/SDL_image
cd SDL_image
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

### SDL_ttf
```
git clone https://github.com/libsdl-org/SDL_ttf
cd SDL_ttf
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```
