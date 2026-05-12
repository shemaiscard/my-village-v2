#version 100
attribute vec2 aPos;
attribute vec3 aColor;

varying vec3 ourColor;
varying vec2 TexCoord;

uniform mat4 projection;

void main() {
    ourColor = aColor;
    TexCoord = aPos / vec2(1100.0, 700.0);
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
