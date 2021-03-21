
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec4 model1;
layout (location = 2) in vec4 model2;
layout (location = 3) in vec4 model3;
layout (location = 4) in vec4 model4;
layout (location = 5) in vec3 color;
layout (location = 6) in vec2 texOffset;

out vec2 TexCoords;
out vec3 FragColor;

//uniform mat4 model;
uniform mat4 projection;
uniform mat4 camera;

void main()
{
    TexCoords = vertex.zw + texOffset.xy;
    FragColor = color;

    mat4 model;
    model[0] = model1;
    model[1] = model2;
    model[2] = model3;
    model[3] = model4;

    gl_Position = projection * (camera * model) * vec4(vertex.xy, 0.0, 1.0);
}  