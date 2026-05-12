#version 100
precision mediump float;

varying vec3 ourColor;
varying vec2 TexCoord;

uniform float timeOfDay;

void main() {
    vec3 color = ourColor;

    // Ambient light darkens everything at night
    float ambient = 0.25 + 0.75 * timeOfDay;
    
    gl_FragColor = vec4(color * ambient, 1.0);
}
