#version 330 core

uniform sampler2D u_texture;

layout(location = 0) out vec4 color;

in vec2 texCoords;

void main() {
    color = texture(u_texture, texCoords);//vec4(0.0, 1.0, 1.0, 1.0);
}