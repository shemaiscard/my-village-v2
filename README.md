# My Village V2

**Live Demo (WebGL):** [myville.netlify.app](https://myville.netlify.app/)

A modernized, cross-platform 2D village simulation built in **C++ with OpenGL** and mirrored in the browser via a standalone **WebGL 2.0** implementation. Demonstrates a complete transition from legacy fixed-function OpenGL to a programmable shader pipeline.

## Features

**Scene**
- Dynamic sky with colour transitions (night navy → noon blue)
- Sun arc, crescent moon, and twinkling stars
- Layered mountain silhouettes, two trees, a house with glowing night windows
- Street lamp with soft glow halo at night
- Rwandan flag on a flagpole with correct proportions

**Animation**
- Real-time day/night cycle (sinusoidal time function)
- Moving clouds, flying plane, driving car with headlights, flying birds
- Animated road dashes

**Particles**
- Chimney smoke with wind drift and expanding puffs
- Rain particles as slanted velocity-aligned streaks (toggle on/off)

## Build & Run (Native)

```bash
# Ubuntu/Debian dependencies
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev

# Build
make native

# Run
./bin/MyVillageV2
```

| Key | Action |
|-----|--------|
| R   | Toggle rain |
| ESC | Exit |

## Run Web Version Locally

```bash
cd my-village-v2/web
python3 -m http.server 8000
# Open http://localhost:8000
```

## Deploy to Netlify

Point publish directory to `web/`. No build command needed.

## Project Structure

```
my-village-v2/
├── assets/shaders/     GLSL vertex + fragment shaders
├── include/            ParticleSystem, Scene, Shader headers
├── src/                C++ source files
├── web/                Self-contained WebGL 2.0 version
└── Makefile
```

## Technical Notes

- Shaders: GLSL 1.30 (native) / GLSL ES 3.00 (WebGL)
- Static geometry uploaded to GPU once at startup; dynamic elements rebuilt each frame
- Single `timeOfDay` uniform drives all colour, visibility, and glow calculations

## Author
Shema Nkindi Giscard

## License
MIT License
