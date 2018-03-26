#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
  vec4 col = texture(screenTexture, TexCoords);

  vec4 tint = vec4(1.0);
  if (int(TexCoords.y * 540.0) % 2 == 0)
    tint = vec4(1.0, 0.7, 1.0, 1.0);

  col = col * tint;
  FragColor = col;

}
