#version 120

//varying vec2 texCoord;
uniform mat4 MVP;

void main() {
  gl_Position =  MVP * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}
