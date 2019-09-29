#version 120

//varying vec2 texCoord;
uniform sampler2D tex;

void main() {
  //gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  gl_FragColor = texture2D(tex, gl_TexCoord[0].st);
}
