#version 150 core

uniform material
{
    vec4 Diffuse;
};

out frag
{
	vec4 Color;
} Frag;

void main()
{
	Frag.Color = Diffuse;
}
