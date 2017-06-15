#version 330 core

uniform sampler2D ktexture;
uniform float delta;
in vec2 coord;
out vec4 color;

void main() {
    vec4 info = texture2D(ktexture, coord);
    vec3 dx = vec3(delta, 0.0, info.r - texture2D(ktexture, vec2(coord.x + delta, coord.y)).r);
    vec3 dy = vec3(0.0, delta, info.r - texture2D(ktexture, vec2(coord.x, coord.y + delta)).r);
    info.ba = normalize(cross(dx, dy)).xy;
    color = info;
}


