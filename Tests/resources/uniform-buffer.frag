#version 140

uniform material
{
    vec4 Diffuse;
};

out vec4 FragColor;

void main()
{
	FragColor = Diffuse;
}
