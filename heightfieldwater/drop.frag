#version 330 core
const float PI = 3.1415926535899793;
uniform vec2 center;
//uniform float radius;
uniform sampler2D ktexture;
//uniform float = strength;
in vec2 coord;
out vec4 color;
void main(){
	vec4 info = texture(ktexture, coord);
	float drop = max(0.0, 1.0 - length( center - coord ) / 0.1);
	drop = 0.5 - cos(drop*PI)*0.5;
	info.r += drop*0.2;
	color = info;

}