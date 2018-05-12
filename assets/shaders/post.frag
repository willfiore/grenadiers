#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D blurTexture;

void main()
{
  vec3 col = texture(screenTexture, TexCoords).rgb;
  vec3 bloom = texture(blurTexture, TexCoords).rgb;
  //FragColor = vec4(col + 0.7*bloom, 1.0);
  FragColor = vec4(col, 1.0);
}
