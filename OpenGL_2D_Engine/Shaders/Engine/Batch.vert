
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 posOffset;
layout (location = 2) in vec3 color;

out vec2 TexCoords;
out vec3 FragColor;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    FragColor = color;
    gl_Position = projection * model * vec4(vertex.xy + posOffset, 0.0, 1.0);
}  