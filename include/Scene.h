#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "MathUtils.h"
#include <vector>

struct Vertex {
    float x, y;
    float r, g, b;
};

class Scene {
public:
    Scene();
    ~Scene();

    void update(float deltaTime);
    void render(Shader& shader);

    void toggleDayNight();
    void toggleRain();
    void moveCar(float amount);

private:
    float timeOfDay;
    float carPosition;
    bool  isDay;
    bool  isRaining;

    unsigned int vao, vbo;
    std::vector<Vertex> vertices;

    void initGeometry();
    void addQuad(float x, float y, float w, float h,
                 float r, float g, float b);
};

#endif
