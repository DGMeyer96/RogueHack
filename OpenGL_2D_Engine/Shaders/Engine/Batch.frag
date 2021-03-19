#version 330 core
in vec2 TexCoords;
in vec3 FragColor;

out vec4 color;

uniform sampler2D image;
//uniform vec3 spriteColor;

void main()
{    
    color = vec4(FragColor, 1.0) * texture(image, TexCoords);
} 