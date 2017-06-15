#version 330 core

uniform sampler2D ktexture;
uniform float delta;
in vec2 coord;
out vec4 color;

void main() {
    vec4 info = texture2D(ktexture, coord);
    vec2 dx = vec2(delta, 0.0);
    vec2 dy = vec2(0.0, delta);
    float average = (    texture2D(ktexture, coord - dx).r +
					     texture2D(ktexture, coord - dy).r +
						 texture2D(ktexture, coord + dx).r +
					     texture2D(ktexture, coord + dy).r ) * 0.25;
    info.g += (average - info.r)*0.3;
    info.g *= 0.995;
    info.r += info.g;
    color = info;
}