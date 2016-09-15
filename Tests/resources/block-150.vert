#version 150 core

uniform transform
{
    mat4 MVP;
} Transform;

in attrib
{
	vec2 Position;
} Attrib;

void main()
{	
	gl_Position = Transform.MVP * vec4(Attrib.Position, 0.0, 1.0);
}

