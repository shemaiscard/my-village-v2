#include "ParticleSystem.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem(int maxParticles) : maxParticles(maxParticles) {
    particles.reserve(maxParticles);
}

// ---------------------------------------------------------------------------
// Chimney smoke: slow, wispy, drifts upward with gentle wind
// ---------------------------------------------------------------------------
void ParticleSystem::emitSmoke(float x, float y) {
    if (particles.size() >= (size_t)maxParticles) return;
    if ((rand() % 100) > 30) return;   // ~30% chance per frame = sparse, natural

    Particle p;
    p.x    = x + ((rand() % 100) / 100.0f - 0.5f) * 6.0f;
    p.y    = y;
    p.vx   = ((rand() % 100) / 100.0f - 0.5f) * 6.0f + 4.0f;  // drift right
    p.vy   = 18.0f + (rand() % 100) / 100.0f * 12.0f;          // slow rise
    p.life = 1.0f;
    p.size = 5.0f + (rand() % 5);      // starts small, grows into puff
    p.r    = 0.60f + (rand() % 15) * 0.01f;
    p.g    = 0.60f + (rand() % 15) * 0.01f;
    p.b    = 0.62f + (rand() % 15) * 0.01f;
    p.a    = 0.55f;
    p.type = 0;   // 0 = smoke
    particles.push_back(p);
}

// ---------------------------------------------------------------------------
// Rain: spawn at top of sky with a slight horizontal slant
// ---------------------------------------------------------------------------
void ParticleSystem::emitRain(float windowWidth, float windowHeight) {
    if (particles.size() >= (size_t)maxParticles) return;

    Particle p;
    p.x    = (rand() % (int)(windowWidth + 300)) - 150.0f;
    p.y    = windowHeight;
    p.vx   = -25.0f;    // slant left  (scene units per second)
    p.vy   = -260.0f;   // fall speed
    p.life = 1.0f;
    p.size = 2.0f;      // thin drop
    p.r    = 0.55f;
    p.g    = 0.70f;
    p.b    = 0.95f;
    p.a    = 0.70f;
    p.type = 1;   // 1 = rain
    particles.push_back(p);
}

// ---------------------------------------------------------------------------
void ParticleSystem::update(float deltaTime, ParticleType type) {
    for (size_t i = 0; i < particles.size(); ) {
        Particle& p = particles[i];
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;

        if (p.type == 0) {
            // Smoke: expand and fade
            p.life -= deltaTime * 0.32f;
            p.a     = p.life * 0.50f;
            p.size += deltaTime * 16.0f;
            // Horizontal drift slows as particle ages (floats weightlessly)
            p.vx   *= (1.0f - deltaTime * 0.5f);
        } else {
            // Rain: die when it hits the road surface (y ~80 in scene coords)
            if (p.y < 80.0f) {
                p.life = 0.0f;
            } else {
                p.life -= deltaTime * 0.9f;
            }
        }

        if (p.life <= 0.0f) {
            particles[i] = particles.back();
            particles.pop_back();
        } else {
            ++i;
        }
    }
}

// ---------------------------------------------------------------------------
// Render:  smoke = expanding translucent quads
//          rain  = 2-pixel wide slanted line streaks
// ---------------------------------------------------------------------------
void ParticleSystem::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Smoke: rounded square blobs ----------------------------------------
    glBegin(GL_QUADS);
    for (const auto& p : particles) {
        if (p.type != 0) continue;
        glColor4f(p.r, p.g, p.b, p.a);
        float s = p.size * 0.5f;
        glVertex2f(p.x - s, p.y - s);
        glVertex2f(p.x + s, p.y - s);
        glVertex2f(p.x + s, p.y + s);
        glVertex2f(p.x - s, p.y + s);
    }
    glEnd();

    // --- Rain: slanted streaks using GL_LINES --------------------------------
    // Each streak spans ~1/18th second of travel (matches vx/vy direction)
    // vx=-25, vy=-260 over dt=0.055 → dx=-1.4, dy=-14.4 — a visible angled dash
    const float DT_STREAK = 0.055f;
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    for (const auto& p : particles) {
        if (p.type != 1) continue;
        glColor4f(p.r, p.g, p.b, p.a);
        // Head of streak (current position)
        glVertex2f(p.x, p.y);
        // Tail of streak (where particle was one DT_STREAK ago)
        glVertex2f(p.x - p.vx * DT_STREAK,
                   p.y - p.vy * DT_STREAK);
    }
    glEnd();
    glLineWidth(1.0f);

    glDisable(GL_BLEND);
}
