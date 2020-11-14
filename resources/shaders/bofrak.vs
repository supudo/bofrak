#version 410 core

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec2 a_textureCoord;

uniform float vvTicks;
uniform vec2 vvResolution;
uniform lowp vec3 vvCameraPosition;
uniform lowp vec4 vvMouse;

out float fvTicks;
out vec2 fvResolution;
out vec3 fvCameraPosition;
out lowp vec4 fvMouse;
out vec2 fvTextureCoord;

void main() {
  gl_Position = vec4(a_vertexPosition, 1.0);
  fvTextureCoord = a_textureCoord;
  fvTicks = vvTicks;
  fvResolution = vvResolution;
  fvCameraPosition = vvCameraPosition;
  fvMouse = vvMouse;
}
