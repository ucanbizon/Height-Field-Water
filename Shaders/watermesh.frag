#version 330 core
in vec3 frag_position;
uniform sampler2D water;
uniform samplerCube skybox;
uniform vec3 camPos;
out vec4 color;

void main()
{ 

	vec3 lightPos = vec3(1.0, 1.0, 1.0);
	vec4 info = texture2D(water, frag_position.xy*0.5+0.5);
	vec3 normal = vec3(info.b, info.a, sqrt(1.0 - dot(info.ba, info.ba)));

	//vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - frag_position);
    float diff = max(dot(normal, lightDir), 0.0);
	float ratio = 1.00/1.33;
    vec3 I = normalize(frag_position - camPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 diffuse = diff *texture(skybox, R).rgb;
	//FragColor = vec4(texture(skybox, R).rgb, 1.0); vec4(diffuse+0.2, 1.0);


	color = vec4(diffuse, 1.0);
}

