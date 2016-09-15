#version 140

uniform vec4 Diffuse;

in float VertInstance;

out vec4 FragColor;

void main()
{
	FragColor = Diffuse * VertInstance * 0.25;
}
