#version 140

uniform mat4 MVP;

in vec2 Position;
in vec2 Texcoord;
out vec2 VertTexcoord;
out float VertInstance;

void main()
{	
	VertInstance = gl_InstanceID;
	VertTexcoord = Texcoord;
	gl_Position = MVP * vec4(Position.x + VertInstance * 2.4 - 1.2, Position.y, 0.0, 1.0);
}
