#version 140

uniform transform
{
    mat4 MVP;
};

in vec2 Position;

void main()
{	
	gl_Position = MVP * vec4(Position, 0.0, 1.0);
}

