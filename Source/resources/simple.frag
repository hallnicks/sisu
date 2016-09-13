// simple fragment shader simple.frag
#version 130
// OpenGL 3.2 replace above with #version 150
// Some drivers require the following
precision highp float;
 
in  vec3 ex_Color;
// GLSL versions after 1.3 remove the built in type gl_FragColor
// If using a shader lang version greater than #version 130
// you *may* need to uncomment the following line:
// out vec4 gl_FragColor
 
void main(void) {
    // Pass through original colour
    gl_FragColor = vec4(ex_Color,1.0);
}
