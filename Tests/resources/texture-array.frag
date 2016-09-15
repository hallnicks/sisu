#version 140

uniform sampler2DArray Diffuse;

in float VertInstance;
in vec2 VertTexcoord;
out vec4 FragColor;

void main()
{
	FragColor = texture2DArray(Diffuse, vec3(VertTexcoord, VertInstance));
}
