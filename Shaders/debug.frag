#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

void main()
{ 
	//color = texture(screenTexture, TexCoords);
    vec4 colora = texture(screenTexture, TexCoords);
	color = vec4(colora.a, 0.0, 0.0, 1.0);
}