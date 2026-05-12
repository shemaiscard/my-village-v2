#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <GL/gl.h>

struct Particle {
    float x, y;
    float vx, vy;
    float life;   // 0.0 to 1.0
    float size;
    float r, g, b, a;
    int   type;   // 0 = smoke, 1 = rain
};


enum class ParticleType { SMOKE, RAIN };

class ParticleSystem {
public:
    ParticleSystem(int maxParticles);
    
    void update(float deltaTime, ParticleType type);
    void render();
    void renderWithTod(float timeOfDay);
    
    void emitSmoke(float x, float y);
    void emitRain(float windowWidth, float windowHeight);

private:
    std::vector<Particle> particles;
    int maxParticles;
};

#endif
