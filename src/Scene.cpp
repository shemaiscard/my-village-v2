#include "Scene.h"
#include "ParticleSystem.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <cstdlib>

// Animation globals
static float absTime   = 0.0f;
static float cloudAnim = 0.0f;
static float dashAnim  = 0.0f;
static float birdAnim  = 0.0f;
static float planeAnim = 1400.0f; // starts off-right

static ParticleSystem* smoke;
static ParticleSystem* rain;

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------
static void pushQuad(std::vector<Vertex>& v,
                     float x, float y, float w, float h,
                     float r, float g, float b)
{
    v.push_back({x,   y,   r, g, b});  v.push_back({x+w, y,   r, g, b});
    v.push_back({x+w, y+h, r, g, b});  v.push_back({x,   y,   r, g, b});
    v.push_back({x+w, y+h, r, g, b});  v.push_back({x,   y+h, r, g, b});
}
static void pushTri(std::vector<Vertex>& v,
                    float x1,float y1,float x2,float y2,float x3,float y3,
                    float r,float g,float b)
{
    v.push_back({x1,y1,r,g,b}); v.push_back({x2,y2,r,g,b}); v.push_back({x3,y3,r,g,b});
}

// Immediate-mode helpers
static void imCircle(float cx, float cy, float radius,
                     float r, float g, float b, int segs = 32)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; ++i) {
        float a = i * 2.0f * 3.14159f / segs;
        glVertex2f(cx + radius * cosf(a), cy + radius * sinf(a));
    }
    glEnd();
}

static void imQuad(float x, float y, float w, float h,
                   float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x,   y);   glVertex2f(x+w, y);
    glVertex2f(x+w, y+h); glVertex2f(x,   y+h);
    glEnd();
}

// ---------------------------------------------------------------------------
Scene::Scene() : timeOfDay(0.8f), carPosition(0.0f), isDay(true), isRaining(false)
{
    smoke = new ParticleSystem(500);
    rain  = new ParticleSystem(1000);
    initGeometry();
}
Scene::~Scene() { delete smoke; delete rain; }

// ---------------------------------------------------------------------------
// VBO: only truly static, never-changing geometry
// ---------------------------------------------------------------------------
void Scene::initGeometry()
{
    vertices.clear();

    // Sky placeholder (overdrawn dynamically each frame in overlay)
    pushQuad(vertices, 0, 380, 1100, 320,  0.28f, 0.62f, 0.86f);

    // Grass
    pushQuad(vertices, 0, 160, 1100, 220,  0.27f, 0.55f, 0.12f);
    // Ground/dirt
    pushQuad(vertices, 0,   0, 1100, 185,  0.18f, 0.38f, 0.10f);
    // Road base
    pushQuad(vertices, 0, -60, 1100, 140,  0.15f, 0.15f, 0.15f);

    // --- Mountains: back range (lighter) ------------------------------------
    pushTri(vertices,    0,380,  220,520,  440,380,  0.45f,0.55f,0.42f);
    pushTri(vertices,  180,380,  400,555,  620,380,  0.45f,0.55f,0.42f);
    pushTri(vertices,  450,380,  650,535,  850,380,  0.45f,0.55f,0.42f);
    pushTri(vertices,  680,380,  870,518, 1100,380,  0.45f,0.55f,0.42f);
    // Front range (darker, closer)
    pushTri(vertices,  -50,380,  150,490,  350,380,  0.35f,0.48f,0.28f);
    pushTri(vertices,  280,380,  490,510,  700,380,  0.35f,0.48f,0.28f);
    pushTri(vertices,  600,380,  800,505, 1000,380,  0.35f,0.48f,0.28f);
    pushTri(vertices,  850,380, 1050,495, 1200,380,  0.35f,0.48f,0.28f);

    // --- House body + triangular roof + chimney ----------------------------
    pushQuad(vertices, 550, 100, 400, 230,  0.55f, 0.55f, 0.58f);
    pushTri(vertices,  530,330, 750,450, 970,330,   0.22f, 0.10f, 0.06f);
    pushQuad(vertices, 540, 318, 420,  14,  0.18f, 0.08f, 0.04f); // eave
    pushQuad(vertices, 740, 325,  30,  75,  0.45f, 0.22f, 0.12f); // chimney
    pushQuad(vertices, 734, 395,  42,  10,  0.35f, 0.18f, 0.10f); // chimney cap

    // --- Windows: left (border + cross-pane) --------------------------------
    pushQuad(vertices, 595, 200, 80, 85,  0.08f,0.05f,0.05f);
    pushQuad(vertices, 600, 205, 70, 75,  0.82f,0.90f,1.00f);
    pushQuad(vertices, 633, 205,  4, 75,  0.08f,0.05f,0.05f);
    pushQuad(vertices, 600, 240, 70,  4,  0.08f,0.05f,0.05f);
    // Windows: right
    pushQuad(vertices, 825, 200, 80, 85,  0.08f,0.05f,0.05f);
    pushQuad(vertices, 830, 205, 70, 75,  0.82f,0.90f,1.00f);
    pushQuad(vertices, 863, 205,  4, 75,  0.08f,0.05f,0.05f);
    pushQuad(vertices, 830, 240, 70,  4,  0.08f,0.05f,0.05f);


    // --- Flagpole -----------------------------------------------------------
    pushQuad(vertices,1000, 100,   5, 175, 0.50f,0.50f,0.50f);
    // --- RWANDAN FLAG: blue (top 56%), yellow strip (11%), green (33%) ------
    pushQuad(vertices,1005, 240,  90,  48, 0.00f,0.60f,0.85f);  // blue
    pushQuad(vertices,1005, 230,  90,  10, 0.98f,0.82f,0.02f);  // yellow strip
    pushQuad(vertices,1005, 194,  90,  36, 0.07f,0.47f,0.20f);  // green

    // --- Street lamp (pole + arm) -------------------------------------------
    pushQuad(vertices, 395,  80,   5, 85,  0.12f,0.12f,0.12f);
    pushQuad(vertices, 378, 158,  25,  5,  0.12f,0.12f,0.12f);

    // --- Trees: trunks ------------------------------------------------------
    pushQuad(vertices, 282, 185, 14, 72,  0.55f,0.35f,0.15f);
    pushQuad(vertices, 102, 130, 38,155,  0.55f,0.35f,0.15f);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Scene::addQuad(float x, float y, float w, float h, float r, float g, float b)
{
    pushQuad(vertices, x, y, w, h, r, g, b);
}

// ---------------------------------------------------------------------------
void Scene::update(float dt)
{
    absTime   += dt;
    // Slow sinusoidal day/night cycle starting at noon
    static float cycle = 1.57f;
    cycle    += dt * 0.07f;
    if (cycle > 6.28f) cycle = 0.0f;
    timeOfDay = 0.5f + 0.5f * sinf(cycle);

    cloudAnim += dt * 18.0f;
    dashAnim  += dt * 20.0f;
    birdAnim  += dt * 0.5f;
    planeAnim -= dt * 95.0f;
    if (planeAnim < -200.0f) planeAnim = 1400.0f;

    // Smoke from chimney (750, 400 = chimney top in scene coords)
    smoke->emitSmoke(755, 400);
    smoke->update(dt, ParticleType::SMOKE);

    if (isRaining) {
        for (int k = 0; k < 12; k++) rain->emitRain(1100, 700);
    }
    rain->update(dt, ParticleType::RAIN);
}

// ---------------------------------------------------------------------------
// Overlay: drawn with fixed-function after the VBO pass
// ---------------------------------------------------------------------------
static void drawOverlay(float tod, float absT)
{
    // ---- DYNAMIC SKY COLOUR ------------------------------------------------
    // Overwrite VBO sky with a colour that changes with time
    float skyR = 0.05f + 0.23f*tod;
    float skyG = 0.05f + 0.57f*tod;
    float skyB = 0.15f + 0.71f*tod;
    imQuad(0, 380, 1100, 320,  skyR, skyG, skyB);

    // ---- SUN or MOON -------------------------------------------------------
    if (tod > 0.25f) {
        // SUN moves on an arc across the sky
        float t     = (tod - 0.25f) / 0.75f;          // 0→1 across day
        float sunX  = 80.0f  + 940.0f * t;
        float sunY  = 420.0f + 180.0f * sinf(3.14159f * t); // arc peak ~noon
        // Outer glow
        imCircle(sunX, sunY, 55, 1.0f, 0.80f, 0.10f, 20);
        // Core
        imCircle(sunX, sunY, 35, 1.0f, 0.92f, 0.05f);
        // Rays
        glColor3f(1.0f, 0.85f, 0.05f);
        for (int a = 0; a < 360; a += 12) {
            float rad = a * 3.14159f / 180.0f;
            float x1 = sunX + 38*cosf(rad), y1 = sunY + 38*sinf(rad);
            float x2 = sunX + 58*cosf(rad), y2 = sunY + 58*sinf(rad);
            glLineWidth(2.0f);
            glBegin(GL_LINES); glVertex2f(x1,y1); glVertex2f(x2,y2); glEnd();
        }
        glLineWidth(1.0f);
    } else {
        // MOON (crescent) in upper-right sky
        const float moonX = 980, moonY = 620;
        imCircle(moonX, moonY, 38, 0.92f, 0.92f, 0.80f);          // full disk
        imCircle(moonX+18, moonY+8, 32, skyR, skyG, skyB);         // crescent bite

        // STARS
        srand(42);
        for (int s = 0; s < 50; s++) {
            float sx = 20.0f + (rand() % 1060);
            float sy = 395.0f + (rand() % 270);
            float sr = 1.5f + (s % 3) * 0.8f;
            float twinkle = 0.5f + 0.5f*sinf(absT*2.0f + s*1.3f);
            imCircle(sx, sy, sr, twinkle, twinkle, twinkle*0.9f, 8);
        }
        srand(time(nullptr)); // restore random seed

        // Soft moon glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.85f, 0.85f, 0.65f, 0.18f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(moonX, moonY);
        for (int k = 0; k <= 32; k++) {
            float a = k * 2.0f * 3.14159f / 32;
            glVertex2f(moonX + 65*cosf(a), moonY + 65*sinf(a));
        }
        glEnd();
        glDisable(GL_BLEND);
    }

    // ---- CLOUDS (visible daytime, fade at dusk) ----------------------------
    if (tod > 0.2f) {
        float ca = fminf(1.0f, (tod - 0.2f) / 0.3f);
        float cm = fmodf(cloudAnim, 1200.0f);
        // Cloud 1
        imCircle(160+cm, 625, 20, ca,ca,ca);
        imCircle(200+cm, 625, 35, ca,ca,ca);
        imCircle(225+cm, 625, 35, ca,ca,ca);
        imCircle(265+cm, 625, 20, ca,ca,ca);
        // Cloud 2
        imCircle(360+cm, 615, 20, ca,ca,ca);
        imCircle(410+cm, 615, 35, ca,ca,ca);
        imCircle(435+cm, 615, 35, ca,ca,ca);
        imCircle(470+cm, 615, 35, ca,ca,ca);
        imCircle(500+cm, 615, 20, ca,ca,ca);
    }

    // ---- PLANE (daytime only) ----------------------------------------------
    if (tod > 0.3f) {
        float np = planeAnim;
        glColor3f(0.1f, 0.1f, 0.9f);
        glBegin(GL_POLYGON);
        glVertex2f(np,625); glVertex2f(np+25,640); glVertex2f(np+90,640);
        glVertex2f(np+105,650); glVertex2f(np+125,650); glVertex2f(np+85,625);
        glEnd();
        glColor3f(0.75f,0.75f,0.75f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(np,625); glVertex2f(np+25,640); glVertex2f(np+90,640);
        glVertex2f(np+105,650); glVertex2f(np+125,650); glVertex2f(np+85,625);
        glEnd();
    }

    // ---- TREE FOLIAGE (draw after mountains so it overlaps) ----------------
    // Small tree (right)
    imCircle(274, 252, 32, 0.10f,0.50f,0.08f);
    imCircle(312, 252, 32, 0.10f,0.50f,0.08f);
    imCircle(283, 292, 27, 0.12f,0.55f,0.09f);
    imCircle(305, 292, 27, 0.12f,0.55f,0.09f);
    imCircle(293, 318, 22, 0.15f,0.58f,0.10f);
    // Big tree (left)
    imCircle( 42, 282, 42, 0.10f,0.48f,0.08f);
    imCircle( 95, 282, 42, 0.10f,0.48f,0.08f);
    imCircle(155, 282, 42, 0.10f,0.48f,0.08f);
    imCircle(210, 282, 42, 0.10f,0.48f,0.08f);
    imCircle( 68, 342, 42, 0.10f,0.48f,0.08f);
    imCircle(118, 342, 42, 0.10f,0.48f,0.08f);
    imCircle(178, 342, 42, 0.10f,0.48f,0.08f);
    imCircle(118, 366, 56, 0.10f,0.48f,0.08f);

    // ---- STREET LAMP BULB + NIGHT GLOW ------------------------------------
    imCircle(391, 163, 10, 1.0f, 1.0f, 0.75f);

    // ---- RWANDAN FLAG SUN (in blue band, upper-right corner) ---------------
    imCircle(1082, 270, 8, 1.0f, 0.82f, 0.04f);   // sun disk
    glColor3f(1.0f, 0.82f, 0.04f);
    glLineWidth(1.5f);
    for (int ra = 0; ra < 360; ra += 30) {
        float rar = ra * 3.14159f / 180.0f;
        glBegin(GL_LINES);
        glVertex2f(1082 + 9*cosf(rar),  270 + 9*sinf(rar));
        glVertex2f(1082 + 15*cosf(rar), 270 + 15*sinf(rar));
        glEnd();
    }
    glLineWidth(1.0f);

    if (tod < 0.5f) {
        float glow = (0.5f - tod) * 2.0f;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.95f, 0.5f, glow * 0.45f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(391, 163);
        for (int k = 0; k <= 32; k++) {
            float a = k * 2.0f * 3.14159f / 32;
            glVertex2f(391 + 50*cosf(a), 163 + 50*sinf(a));
        }
        glEnd();
        glDisable(GL_BLEND);

        // Window glow at night
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.9f, 0.85f, 0.3f, glow * 0.75f);
        glBegin(GL_QUADS);
        glVertex2f(600,205); glVertex2f(670,205);
        glVertex2f(670,280); glVertex2f(600,280);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(830,205); glVertex2f(900,205);
        glVertex2f(900,280); glVertex2f(830,280);
        glEnd();
        glDisable(GL_BLEND);
    }

    // ---- ANIMATED ROAD DASHES ----------------------------------------------
    float doff = fmodf(dashAnim, 100.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    for (float dx = -doff; dx < 1100; dx += 100.0f) {
        imQuad(dx, 36, 60, 8,  0.9f, 0.9f, 0.9f);
    }

    // ---- CAR ---------------------------------------------------------------
    float cx = fmodf(180.0f + cloudAnim * 1.5f, 1300.0f) - 100.0f;

    glColor3f(0.85f, 0.08f, 0.08f);
    glBegin(GL_POLYGON);
    glVertex2f(cx,    55); glVertex2f(cx+80, 55);
    glVertex2f(cx+75, 85); glVertex2f(cx,    85);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(cx+10, 85); glVertex2f(cx+65, 85);
    glVertex2f(cx+55,115); glVertex2f(cx+20,115);
    glEnd();
    // Car windows (blue tint)
    glColor3f(0.82f, 0.90f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(cx+15, 85); glVertex2f(cx+35, 85);
    glVertex2f(cx+35,105); glVertex2f(cx+20,105);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(cx+40, 85); glVertex2f(cx+60, 85);
    glVertex2f(cx+55,105); glVertex2f(cx+40,105);
    glEnd();
    // Wheels
    imCircle(cx+55, 50, 10, 0.12f,0.12f,0.12f);
    imCircle(cx+25, 50, 10, 0.12f,0.12f,0.12f);
    imCircle(cx+55, 50,  5, 0.75f,0.75f,0.75f);
    imCircle(cx+25, 50,  5, 0.75f,0.75f,0.75f);
    // Headlights at night
    if (tod < 0.4f) {
        float hl = (0.4f - tod) / 0.4f;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 0.7f, hl * 0.7f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx+78, 68);
        for (int k = 0; k <= 24; k++) {
            float a = -0.5f + k * 1.0f / 24;
            glVertex2f(cx+78 + 50*cosf(a), 68 + 50*sinf(a));
        }
        glEnd();
        glDisable(GL_BLEND);
        imCircle(cx+78, 68, 7, 1.0f, 1.0f, 0.8f, 12);
    }

    // ---- BIRDS (day only) --------------------------------------------------
    if (tod > 0.35f) {
        float bi = birdAnim;
        glColor3f(0.2f, 0.1f, 0.03f);
        auto bird = [&](float ox, float oy, float sp) {
            float bx = fmodf(ox + sp*bi, 1200.0f) - 50.0f;
            glBegin(GL_POLYGON); glVertex2f(bx,oy+10); glVertex2f(bx+12,oy+10); glVertex2f(bx+12,oy); glEnd();
            glBegin(GL_POLYGON); glVertex2f(bx+12,oy+15); glVertex2f(bx+18,oy+15); glVertex2f(bx+12,oy+10); glEnd();
        };
        bird(300,455,40); bird(200,460,50); bird(150,455,30);
    }
}

// ---------------------------------------------------------------------------
void Scene::render(Shader& shader)
{
    // 1. VBO: static tinted geometry
    shader.use();
    shader.setFloat("timeOfDay", timeOfDay);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);

    // 2. Overlay: fixed-function pipeline
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 1100, 0, 700);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    drawOverlay(timeOfDay, absTime);

    // 3. Particles
    smoke->renderWithTod(timeOfDay);
    if (isRaining) rain->renderWithTod(timeOfDay);


    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// ---------------------------------------------------------------------------
void Scene::toggleDayNight() { isDay = !isDay; }
void Scene::toggleRain()     { isRaining = !isRaining; }
void Scene::moveCar(float amount) { carPosition += amount; }
