#version 140

uniform sampler2DRect Diffuse;

in vec2 VertTexcoord;
out vec4 FragColor;

void main()
{
	FragColor = textureRect(Diffuse, VertTexcoord);
}
