// simple vert shader - simple.vert
#version 130
// OpenGL3.0

uniform mat4x4 MVP;
in vec3 in_Position;
in vec3 in_Color;
out vec3 ex_Color;

// simple shader program
// multiply each vertex position by the MVP matrix

void main(void){
	gl_Position = MVP * vec4(in_Position,1.0);
	ex_Color = in_Color;
}
