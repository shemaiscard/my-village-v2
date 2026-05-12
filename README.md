# My Village V2

A modernized, cross-platform 2D village simulation built in C++ with OpenGL and mirrored in the browser via a standalone WebGL 2.0 implementation. The project demonstrates a complete transition from legacy fixed-function OpenGL to a programmable shader pipeline, while maintaining full visual parity between the native desktop application and the interactive web version.

---

## Overview

The original My Village project was a single-file legacy OpenGL demo using `glBegin`/`glEnd` with no shader support. This V2 rewrite introduces:

- A programmable rendering pipeline using GLSL vertex and fragment shaders
- Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs) for efficient geometry submission
- A hybrid rendering model that combines VBO-based static geometry with an immediate-mode overlay for complex animated elements
- A standalone WebGL 2.0 engine (`village.js`) that faithfully reproduces the C++ scene in the browser with no compilation step required

---

## Features

**Scene**
- Dynamic sky with colour transitions from deep navy at night to clear blue at noon
- Sun that travels on an arc across the sky throughout the day
- Crescent moon with twinkling stars that appear at night
- Layered mountain silhouettes in the background
- Two trees with multi-layer canopy foliage
- House with triangular roof, cross-pane windows, and a shaded door
- Street lamp that emits a soft glow halo at night
- House windows that glow warm yellow at night
- Rwandan flag on a flagpole with the correct proportions and sun emblem

**Animation**
- Real-time day/night cycle driven by a sinusoidal time function
- Moving clouds that drift across the sky (fade at dusk)
- Flying plane visible during daytime
- Car driving along the road with headlights at night
- Three flying birds (daytime only)
- Animated road centre dashes

**Particles**
- Chimney smoke that rises slowly with a gentle wind drift and expands into puffs
- Rain particles rendered as slanted velocity-aligned streaks, toggled on demand

---

## Project Structure

```
my-village-v2/
  assets/
    shaders/
      scene.vert        GLSL vertex shader
      scene.frag        GLSL fragment shader
  include/
    ParticleSystem.h
    Scene.h
    Shader.h
  src/
    main.cpp
    ParticleSystem.cpp
    Scene.cpp
    Shader.cpp
  web/
    index.html          Web entry point (deployable to Netlify / GitHub Pages)
    village.js          Standalone WebGL 2.0 scene engine
    style.css           UI styling
  Makefile
  README.md
```

---

## Building and Running (Native)

### Requirements

- Linux (tested on Ubuntu 22.04+)
- GCC with C++17 support
- OpenGL development libraries
- freeglut

Install dependencies on Ubuntu/Debian:

```bash
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
```

### Build

```bash
cd my-village-v2
make native
```

### Run

```bash
./bin/MyVillageV2
```

### Controls

| Key | Action |
|-----|--------|
| R   | Toggle rain on/off |
| ESC | Exit the application |

---

## Running the Web Version

The web version requires only a local HTTP server (the browser blocks WebGL on the `file://` protocol).

```bash
cd my-village-v2/web
python3 -m http.server 8000
```

Then open `http://localhost:8000` in any browser that supports WebGL 2.0 (Chrome, Firefox, Edge).

### Web Controls

| Control | Action |
|---------|--------|
| Time of Day slider | Manually set the time (Night / Dawn / Day) |
| Toggle Rain button | Enable or disable rain |

---

## Deploying to Netlify

The `web/` directory is entirely self-contained. To publish:

1. Push the repository to GitHub.
2. On Netlify, create a new site and point the publish directory to `web/`.
3. No build command is needed — the site is plain HTML, CSS, and JavaScript.

---

## Technical Notes

**Shader compatibility** — Shaders target GLSL 1.30 (`#version 130`) for the native build and GLSL ES 3.00 (`#version 300 es`) for WebGL 2.0.

**Rendering architecture** — Static geometry (terrain, house, trees, flag) is uploaded to GPU memory once at startup. Dynamic elements (sky, sun/moon, clouds, car, birds, particles) are rebuilt every frame in a separate buffer.

**Particle system** — Smoke and rain share a common `ParticleSystem` class but are tracked with an explicit `type` field. Smoke particles render as expanding translucent quads; rain renders as velocity-aligned `GL_LINES` streaks with a slight leftward slant.

**Day/night cycle** — `timeOfDay` is a normalised value in `[0, 1]` driven by a sinusoidal function. All colour, visibility, and glow calculations derive from this single uniform. In the web version the slider sets it directly; in the native version it can be overridden by pressing D.

---

## Author

Shema Nkindi Giscard

---

## License

MIT License. See `LICENSE` for details.
