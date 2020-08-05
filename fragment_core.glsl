#version 330

out vec4 fs_colour;

uniform vec4 colour;

void main()
{
    fs_colour = colour;
}
