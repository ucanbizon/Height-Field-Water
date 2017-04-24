#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D water;
out vec3 frag_position;

void main(){
	vec4 info = texture2D(water, texCoords);
	vec3 posheight = position;
	posheight.z += info.r;

	frag_position = posheight;
    gl_Position = projection * view *  model * vec4(posheight, 1.0f);
}