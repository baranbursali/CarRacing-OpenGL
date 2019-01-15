#version 430 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoords;

out vec3 ourColor;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = vTexCoords;
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    ourColor = vColor;
}