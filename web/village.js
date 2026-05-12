// ============================================================
// My Village V2 — Pure WebGL 2.0 implementation
// ============================================================
const canvas = document.getElementById('glCanvas');
const gl = canvas.getContext('webgl2');
if (!gl) { alert('WebGL 2 not supported'); }

// ---- Shaders ---------------------------------------------------------------
const VS = `#version 300 es
in vec2 aPos;
in vec3 aColor;
out vec3 ourColor;
uniform mat4 projection;
void main() {
    ourColor = aColor;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}`;

// Shader darkens scene at night, tints sky blue→deep-navy
const FS = `#version 300 es
precision mediump float;
in vec3 ourColor;
out vec4 fragColor;
uniform float timeOfDay;
void main() {
    float ambient = 0.18 + 0.82 * timeOfDay;
    fragColor = vec4(ourColor * ambient, 1.0);
}`;

function mkShader(type, src) {
    const s = gl.createShader(type);
    gl.shaderSource(s, src);
    gl.compileShader(s);
    if (!gl.getShaderParameter(s, gl.COMPILE_STATUS))
        console.error(gl.getShaderInfoLog(s));
    return s;
}
const prog = gl.createProgram();
gl.attachShader(prog, mkShader(gl.VERTEX_SHADER,   VS));
gl.attachShader(prog, mkShader(gl.FRAGMENT_SHADER, FS));
gl.linkProgram(prog);
const STRIDE = 20;
const posLoc   = gl.getAttribLocation(prog, 'aPos');
const colorLoc = gl.getAttribLocation(prog, 'aColor');

// ---- Static geometry helpers -----------------------------------------------
const verts = [];
function quad(x, y, w, h, r, g, b) {
    for (const [px,py] of [[x,y],[x+w,y],[x+w,y+h],[x,y],[x+w,y+h],[x,y+h]])
        verts.push(px, py, r, g, b);
}
function circleV(arr, cx, cy, rad, r, g, b, segs=32) {
    for (let i = 0; i < segs; i++) {
        const a0 = i*2*Math.PI/segs, a1 = (i+1)*2*Math.PI/segs;
        arr.push(cx,cy,r,g,b,
                 cx+rad*Math.cos(a0), cy+rad*Math.sin(a0), r,g,b,
                 cx+rad*Math.cos(a1), cy+rad*Math.sin(a1), r,g,b);
    }
}
function triV(arr, x1,y1,x2,y2,x3,y3, r,g,b) {
    arr.push(x1,y1,r,g,b, x2,y2,r,g,b, x3,y3,r,g,b);
}
function quadV(arr, x,y,w,h, r,g,b) {
    for (const [px,py] of [[x,y],[x+w,y],[x+w,y+h],[x,y],[x+w,y+h],[x,y+h]])
        arr.push(px, py, r, g, b);
}

// ---- Build static scene geometry -------------------------------------------

// Sky (day colour — shader dims at night)
quad(0, 380, 1100, 320,  0.28, 0.62, 0.86);
// Grass
quad(0, 160, 1100, 220,  0.27, 0.55, 0.12);
// Ground/dirt strip
quad(0, 0,   1100, 185,  0.18, 0.38, 0.10);
// Road
quad(0, -60, 1100, 140,  0.15, 0.15, 0.15);

// ---- MOUNTAINS (layered silhouettes in the grass band) ----------------------
// Back-range (lighter, further)
triV(verts,   0,380,  220,520, 440,380,  0.45,0.55,0.42);
triV(verts, 180,380,  400,555, 620,380,  0.45,0.55,0.42);
triV(verts, 450,380,  650,535, 850,380,  0.45,0.55,0.42);
triV(verts, 680,380,  870,518, 1100,380, 0.45,0.55,0.42);
// Front-range (darker, closer)
triV(verts,  -50,380, 150,490, 350,380,  0.35,0.48,0.28);
triV(verts,  280,380, 490,510, 700,380,  0.35,0.48,0.28);
triV(verts,  600,380, 800,505, 1000,380, 0.35,0.48,0.28);
triV(verts,  850,380, 1050,495,1200,380, 0.35,0.48,0.28);

// House body
quad(550, 100, 400, 230,  0.55, 0.55, 0.58);
// Roof (triangle shape using two triangles)
triV(verts, 530,330, 750,450, 970,330,  0.22, 0.10, 0.06);
// Roof overlap base
quad(540, 320, 420, 15,  0.18, 0.08, 0.04);
// Chimney
quad(740, 325, 30, 75,  0.45, 0.22, 0.12);
quad(734, 395, 42, 10,  0.35, 0.18, 0.10); // chimney cap

// Windows border + glass + dividers (left)
quad(595, 200, 80, 85,  0.08, 0.05, 0.05);
quad(600, 205, 70, 75,  0.82, 0.90, 1.00);
quad(633, 205, 4,  75,  0.08, 0.05, 0.05);
quad(600, 240, 70, 4,   0.08, 0.05, 0.05);
// Windows right
quad(825, 200, 80, 85,  0.08, 0.05, 0.05);
quad(830, 205, 70, 75,  0.82, 0.90, 1.00);
quad(863, 205, 4,  75,  0.08, 0.05, 0.05);
quad(830, 240, 70, 4,   0.08, 0.05, 0.05);

// Door frame + door
quad(697, 97, 106, 125, 0.12, 0.07, 0.04);
quad(700, 100, 100, 120, 0.88, 0.80, 0.70);
// Door shading panel
triV(verts, 760,120, 760,215, 700,215,  0.75, 0.65, 0.55);
// Door handle
circleV(verts, 790, 160, 5, 0.8, 0.7, 0.2);

// Street lamp pole + arm + light
quad(395, 80,  5, 85, 0.12, 0.12, 0.12);
quad(378, 158, 25, 5,  0.12, 0.12, 0.12);
circleV(verts, 391, 163, 10,  1.0, 1.0, 0.75);

// Flagpole & RWANDAN FLAG: blue (56%), yellow strip (11%), green (33%)
quad(1000, 100, 5, 175, 0.5, 0.5, 0.5);
quad(1005, 240, 90, 48, 0.00, 0.60, 0.85);  // blue
quad(1005, 230, 90, 10, 0.98, 0.82, 0.02);  // yellow strip
quad(1005, 194, 90, 36, 0.07, 0.47, 0.20);  // green

// Tree 1 — trunk + layered canopy (right/small)
quad(282, 185, 14, 72,  0.55, 0.35, 0.15);
circleV(verts, 274, 252, 32, 0.10, 0.50, 0.08);
circleV(verts, 312, 252, 32, 0.10, 0.50, 0.08);
circleV(verts, 283, 292, 27, 0.12, 0.55, 0.09);
circleV(verts, 305, 292, 27, 0.12, 0.55, 0.09);
circleV(verts, 293, 318, 22, 0.15, 0.58, 0.10);

// Tree 2 — trunk + big layered canopy (left/large)
quad(102, 130, 38, 155, 0.55, 0.35, 0.15);
for (const [cx,cy,r] of [
    [42,282,42],[95,282,42],[155,282,42],[210,282,42],
    [68,342,42],[118,342,42],[178,342,42],[118,366,56]
]) circleV(verts, cx,cy,r, 0.10,0.48,0.08);

// Sun static (colour; shown/hidden via dynamic layer override)
circleV(verts, 100, 625, 35,  1.0, 0.88, 0.05);

const STATIC_COUNT = verts.length / 5;

// ---- Dynamic buffer --------------------------------------------------------
const dynVerts = [];
function dc(cx,cy,rad,r,g,b,segs=28) { circleV(dynVerts,cx,cy,rad,r,g,b,segs); }
function dq(x,y,w,h,r,g,b)           { quadV(dynVerts,x,y,w,h,r,g,b); }
function dt(x1,y1,x2,y2,x3,y3,r,g,b){ triV(dynVerts,x1,y1,x2,y2,x3,y3,r,g,b); }
function cpoly(pts,r,g,b) {
    for (let i=1;i<pts.length-1;i++)
        dynVerts.push(pts[0][0],pts[0][1],r,g,b,
                      pts[i][0],pts[i][1],r,g,b,
                      pts[i+1][0],pts[i+1][1],r,g,b);
}

// ---- Upload static VAO/VBO -------------------------------------------------
const vao = gl.createVertexArray(); gl.bindVertexArray(vao);
const vbo = gl.createBuffer();      gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(verts), gl.STATIC_DRAW);
gl.enableVertexAttribArray(posLoc);   gl.vertexAttribPointer(posLoc,   2,gl.FLOAT,false,STRIDE,0);
gl.enableVertexAttribArray(colorLoc); gl.vertexAttribPointer(colorLoc, 3,gl.FLOAT,false,STRIDE,8);

// Dynamic VAO/VBO
const dvao = gl.createVertexArray(); gl.bindVertexArray(dvao);
const dvbo = gl.createBuffer();      gl.bindBuffer(gl.ARRAY_BUFFER, dvbo);
gl.enableVertexAttribArray(posLoc);   gl.vertexAttribPointer(posLoc,   2,gl.FLOAT,false,STRIDE,0);
gl.enableVertexAttribArray(colorLoc); gl.vertexAttribPointer(colorLoc, 3,gl.FLOAT,false,STRIDE,8);

const proj = new Float32Array([
    2/1100,0,0,0,  0,2/700,0,0,  0,0,-1,0,  -1,-1,0,1
]);

// ---- UI controls -----------------------------------------------------------
const slider     = document.getElementById('timeSlider');
const rainBtn    = document.getElementById('rainToggle');
const timeLabel  = document.getElementById('timeLabel');
let autoCycle = true;
let manualTime = 0.8;
let isRaining  = false;

if (slider) {
    slider.addEventListener('input', e => {
        manualTime = e.target.value / 100;
        autoCycle  = false;
    });
}
if (rainBtn) {
    rainBtn.addEventListener('click', () => {
        isRaining = !isRaining;
        rainBtn.textContent = isRaining ? '⛅ Stop Rain' : '🌧️ Toggle Rain';
        rainBtn.style.background = isRaining
            ? 'rgba(40,80,200,0.5)' : '';
    });
}

// ---- Particles (smoke + rain) ----------------------------------------------
const smokeParticles = [];
const rainParticles  = [];

function emitSmoke() {
    if (Math.random() > 0.25) return;   // ~25% chance each frame = slow, gentle
    smokeParticles.push({
        x: 755 + (Math.random()-0.5)*5,
        y: 400,
        vx: (Math.random()-0.5)*8 + 4,   // scene units per SECOND, gentle drift right
        vy: 22 + Math.random()*14,        // scene units per SECOND, slow rise
        life: 1.0, rad: 4 + Math.random()*4
    });
}
function emitRain() {
    for (let k = 0; k < 10; k++) {
        rainParticles.push({
            x: Math.random()*1300 - 100, y: 710,
            vx: -25,          // scene units per SECOND (slant left)
            vy: -(260 + Math.random()*40),  // scene units per SECOND (fall)
            life: 1.0
        });
    }
}
function updateParticles(dt) {
    emitSmoke();
    if (isRaining) emitRain();

    for (let i = smokeParticles.length-1; i >= 0; i--) {
        const p = smokeParticles[i];
        p.x  += p.vx * dt;           // dt-scaled movement
        p.y  += p.vy * dt;
        p.vx *= (1 - dt * 0.5);      // horizontal drift slows as particle ages
        p.rad += 5 * dt;             // expand ~5 units/sec
        p.life -= dt * 0.32;         // lives ~3 seconds
        if (p.life <= 0) smokeParticles.splice(i,1);
    }
    for (let i = rainParticles.length-1; i >= 0; i--) {
        const p = rainParticles[i];
        p.x  += p.vx * dt;           // dt-scaled movement
        p.y  += p.vy * dt;
        p.life -= dt * 0.9;
        if (p.life <= 0 || p.y < 60) rainParticles.splice(i,1);
    }
}

// ---- Render loop -----------------------------------------------------------
let last = 0;
function render(ts) {
    const dt = Math.min((ts - last)/1000, 0.05);
    last = ts;
    const t = ts/1000;

    // timeOfDay: 0=night, 1=full day  (slider is linear Dawn→Day)
    let timeOfDay = autoCycle
        ? 0.5 + 0.5*Math.sin(t*0.25)
        : manualTime;

    if (slider && autoCycle) {
        slider.value = timeOfDay * 100;
    }
    // Update label
    if (timeLabel) {
        if (timeOfDay < 0.25)       timeLabel.textContent = '🌙 Night';
        else if (timeOfDay < 0.5)   timeLabel.textContent = '🌅 Dawn';
        else if (timeOfDay < 0.75)  timeLabel.textContent = '🌤️ Day';
        else                         timeLabel.textContent = '☀️ Noon';
    }

    updateParticles(dt);

    const cloudOff = (t * 18) % 1200;
    const carOff   = ((t * 28) % 1300) - 100;

    // ---- Build dynamic geometry --------------------------------------------
    dynVerts.length = 0;

    // Sky gradient cover (update colour by time — black at night, sky-blue at noon)
    const skyR = 0.05 + 0.23*timeOfDay;
    const skyG = 0.05 + 0.57*timeOfDay;
    const skyB = 0.15 + 0.71*timeOfDay;
    dq(0, 380, 1100, 320, skyR, skyG, skyB);

    // ---- SUN or MOON depending on time of day ------------------------------
    if (timeOfDay > 0.25) {
        // SUN — rises and sets on a horizontal arc
        const sunX = 80 + 900*(timeOfDay - 0.25)/0.75;
        const sunY = 500 + 130*Math.sin(Math.PI*(timeOfDay - 0.25)/0.75);
        // Glow halo
        const gR = 1.0, gG = 0.75, gB = 0.2;
        dc(sunX, sunY, 60, gR, gG, 0.05, 20);  // soft outer
        dc(sunX, sunY, 35, 1.0, 0.92, 0.05);    // core
        // Rays
        for (let a = 0; a < 360; a += 12) {
            const rad = a*Math.PI/180;
            dynVerts.push(
                sunX+38*Math.cos(rad), sunY+38*Math.sin(rad), 1,0.88,0.1,
                sunX+56*Math.cos(rad), sunY+56*Math.sin(rad), 1,0.80,0.0,
                sunX+38*Math.cos(rad+0.08), sunY+38*Math.sin(rad+0.08), 1,0.88,0.1
            );
        }
    } else {
        // MOON — crescent in upper-right sky
        const moonX = 980, moonY = 620;
        dc(moonX, moonY, 38, 0.92, 0.92, 0.80);   // full moon disk
        // Crescent: cover with night-sky colour
        dc(moonX+18, moonY+8, 32, skyR, skyG, skyB); // bite out of moon
        // Stars
        const seed = Math.floor(t*0.1)*1000;
        for (let s = 0; s < 40; s++) {
            const sx = ((seed+s*137)%1060)+20;
            const sy = 400 + ((seed+s*251)%260);
            const sr = 2 + (s%3);
            const sparkle = 0.5 + 0.5*Math.sin(t*2+s);
            dc(sx, sy, sr, sparkle, sparkle, sparkle*0.9, 8);
        }
        // Moon glow
        dc(moonX, moonY, 55, 0.85, 0.85, 0.65, 20);
    }

    // ---- CLOUDS (only visible at day) --------------------------------------
    if (timeOfDay > 0.2) {
        const ca = Math.min(1, (timeOfDay-0.2)/0.3);
        const cr = ca, cg = ca, cb = ca;
        for (const [cx,cy,r] of [
            [160+cloudOff,625,20],[200+cloudOff,625,35],[225+cloudOff,625,35],[265+cloudOff,625,20],
            [360+cloudOff,615,20],[410+cloudOff,615,35],[435+cloudOff,615,35],[470+cloudOff,615,35],[500+cloudOff,615,20]
        ]) dc(cx,cy,r,cr,cg,cb);
    }

    // ---- PLANE (flies at daytime) ------------------------------------------
    if (timeOfDay > 0.3) {
        const np = -((t*100)%1400) + 1200;
        cpoly([[np,625],[np+25,640],[np+90,640],[np+105,650],[np+125,650],[np+85,625]],0,0,1);
        dc(np+25, 625, 2, 1,0,0, 8);
        dc(np+29, 623, 2, 1,1,0, 8);
    }

    // ---- CAR ---------------------------------------------------------------
    const cx2 = carOff;
    cpoly([[cx2,55],[cx2+80,55],[cx2+75,85],[cx2,85]],0.85,0.08,0.08);
    cpoly([[cx2+10,85],[cx2+65,85],[cx2+55,115],[cx2+20,115]],0.85,0.08,0.08);
    cpoly([[cx2+15,85],[cx2+35,85],[cx2+35,105],[cx2+20,105]],0.82,0.90,1.0);
    cpoly([[cx2+40,85],[cx2+60,85],[cx2+55,105],[cx2+40,105]],0.82,0.90,1.0);
    dc(cx2+55,50,10, 0.12,0.12,0.12);
    dc(cx2+25,50,10, 0.12,0.12,0.12);
    dc(cx2+55,50, 5, 0.75,0.75,0.75);
    dc(cx2+25,50, 5, 0.75,0.75,0.75);
    // Headlights at night (proper cone + bulb)
    if (timeOfDay < 0.4) {
        const hl = (0.4 - timeOfDay) / 0.4;  // 0→1 as night deepens
        // Light cone (fan of triangles pointing right)
        const coneX = cx2 + 80, coneY = 68;
        const coneLen = 80, halfAngle = 0.38;  // ~22 degrees half-angle
        const coneSteps = 10;
        for (let ci = 0; ci < coneSteps; ci++) {
            const a0 = -halfAngle + ci     * (2*halfAngle/coneSteps);
            const a1 = -halfAngle + (ci+1) * (2*halfAngle/coneSteps);
            const alpha = hl * 0.35 * (1 - ci/coneSteps); // fade toward tip edges
            dynVerts.push(
                coneX, coneY,   1.0, 1.0, 0.7,
                coneX + coneLen*Math.cos(a0), coneY + coneLen*Math.sin(a0), 1.0*alpha,0.9*alpha,0.4*alpha,
                coneX + coneLen*Math.cos(a1), coneY + coneLen*Math.sin(a1), 1.0*alpha,0.9*alpha,0.4*alpha
            );
        }
        // Bulb
        dc(coneX, coneY, 7, 1.0, 1.0, 0.85, 12);
    }

    // ---- BIRDS (day only) --------------------------------------------------
    if (timeOfDay > 0.35) {
        const bi = t*0.5;
        function bird(ox,oy,sp) {
            const bx = ((ox+sp*bi)%1200)-50;
            dynVerts.push(bx,oy+10,0.2,0.1,0.03, bx+12,oy+10,0.2,0.1,0.03, bx+12,oy,0.2,0.1,0.03);
            dynVerts.push(bx+12,oy+15,0.2,0.1,0.03, bx+18,oy+15,0.2,0.1,0.03, bx+12,oy+10,0.2,0.1,0.03);
        }
        bird(300,455,40); bird(200,460,50); bird(150,455,30);
    }

    // ---- RWANDAN FLAG SUN (in blue band upper-right) -----------------------
    dc(1082, 268, 8, 1.0, 0.82, 0.04, 16);
    // Rays
    for (let ra = 0; ra < 360; ra += 30) {
        const r = ra * Math.PI / 180;
        dynVerts.push(
            1082 + 9*Math.cos(r),  268 + 9*Math.sin(r),  1.0,0.82,0.04,
            1082 + 15*Math.cos(r), 268 + 15*Math.sin(r), 1.0,0.80,0.02,
            1082 + 9*Math.cos(r+0.15), 268 + 9*Math.sin(r+0.15), 1.0,0.82,0.04
        );
    }

    const dashOff = (t*20)%100;
    for (let dx = -dashOff; dx < 1100; dx += 100) {
        dq(dx, 36, 60, 8,  0.9,0.9,0.9);
    }

    // ---- STREET LAMP GLOW at night -----------------------------------------
    if (timeOfDay < 0.5) {
        const glow = 1.0 - timeOfDay*2;
        dc(391, 163, 30, 1.0*glow, 0.95*glow, 0.5*glow, 16);
        // Window glow at night
        dq(600,205,70,75,  0.9*glow, 0.85*glow, 0.4*glow);
        dq(830,205,70,75,  0.9*glow, 0.85*glow, 0.4*glow);
    }

    // ---- SMOKE particles ---------------------------------------------------
    for (const p of smokeParticles) {
        const v = p.life * 0.6;
        dc(p.x, p.y, p.rad, v,v,v, 14);
    }

    // ---- RAIN particles — slanted quad streaks matching native GL_LINES ----
    // Each drop is a thin angled quad: 2px wide, ~14px long, slanted left
    const DT_STREAK = 0.055;  // streak length = 1/18th sec of travel
    for (const p of rainParticles) {
        const alpha = 0.55 + 0.3*p.life;
        // Head and tail of the streak
        const tx = p.vx * DT_STREAK;   // ~-1.4
        const ty = p.vy * DT_STREAK;   // ~-14.3
        // Perpendicular direction (normalised, scaled to half-width = 1.5)
        const len = Math.sqrt(tx*tx + ty*ty);
        const nx = -ty/len * 1.5;
        const ny =  tx/len * 1.5;
        // quad: 4 corners of the streak
        const ax = p.x + nx,      ay = p.y + ny;
        const bx = p.x - nx,      by = p.y - ny;
        const cx = p.x+tx - nx,   cy = p.y+ty - ny;
        const dx = p.x+tx + nx,   dy = p.y+ty + ny;
        dynVerts.push(
            ax,ay, 0.75,0.80,0.95,   bx,by, 0.75,0.80,0.95,   cx,cy, 0.5,0.6,0.8,
            ax,ay, 0.75,0.80,0.95,   cx,cy, 0.5,0.6,0.8,       dx,dy, 0.5,0.6,0.8
        );
    }

    // ---- Draw --------------------------------------------------------------
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(prog);
    gl.uniformMatrix4fv(gl.getUniformLocation(prog,'projection'), false, proj);
    gl.uniform1f(gl.getUniformLocation(prog,'timeOfDay'), timeOfDay);

    // Static geometry (trees, house, grass, etc — tinted by shader)
    gl.bindVertexArray(vao);
    gl.drawArrays(gl.TRIANGLES, 0, STATIC_COUNT);

    // Dynamic geometry (sky cover, sun/moon, car, etc)
    gl.bindVertexArray(dvao);
    gl.bindBuffer(gl.ARRAY_BUFFER, dvbo);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(dynVerts), gl.DYNAMIC_DRAW);
    gl.enableVertexAttribArray(posLoc);   gl.vertexAttribPointer(posLoc,   2,gl.FLOAT,false,STRIDE,0);
    gl.enableVertexAttribArray(colorLoc); gl.vertexAttribPointer(colorLoc, 3,gl.FLOAT,false,STRIDE,8);
    gl.drawArrays(gl.TRIANGLES, 0, dynVerts.length/5);

    requestAnimationFrame(render);
}
requestAnimationFrame(render);
