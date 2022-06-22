#version 330 core

layout(location = 0) in vec4 position;

out vec2 texCoords;

void main() {
    gl_Position = position;
    texCoords = (position.xy + 1) * 0.5;
}