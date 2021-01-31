#version 330 core

// Input
layout (location = 0) in vec3 aPos;

// Output


void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}