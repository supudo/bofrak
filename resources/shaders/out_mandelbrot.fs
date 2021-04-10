#version 410 core

uniform sampler2D fvTexture01;
in vec2 fvTextureCoord;
in float fvTicks;
in vec2 fvResolution;
in vec3 fvCameraPosition;
in vec4 fvMouse;

out vec4 fragColor;

void main(void) {
  float scale = 2.2;
  int iter = 70;
  vec2 center = vec2(0.7, 0.0);

  vec2 z, c;

  c.x = 1.3333 * (fvTextureCoord.x - 0.5) * scale - center.x;
  c.y = (fvTextureCoord.y - 0.5) * scale - center.y;

  int i;
  z = c;
  for (i = 0; i < iter; i++) {
    float x = (z.x * z.x - z.y * z.y) + c.x;
    float y = (z.y * z.x + z.x * z.y) + c.y;

    if ((x * x + y * y) > 4.0)
      break;
    z.x = x;
    z.y = y;
  }

  float tc = (i == iter ? 0.0 : float(i)) / 100.0;
  fragColor = texture(fvTexture01, vec2(tc, 0));
}
