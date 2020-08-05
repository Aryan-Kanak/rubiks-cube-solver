#version 330

layout (location = 0) in vec3 vertex_position;

out vec3 vs_position;

uniform mat4 mvp;

void main()
{
    vs_position = vec4(mvp * vec4(vertex_position, 1.0f)).xyz;

    gl_Position = mvp * vec4(vertex_position, 1.0f);
}
