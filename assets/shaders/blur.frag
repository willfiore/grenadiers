#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool horizontal;

const float weight[6] =
float[] (0.095474, 0.094998, 0.093585, 0.091276, 0.088139, 0.084264);
const int m = 1;

void main()
{
  vec2 texOffset = 1.0 / textureSize(screenTexture, 0);

  // Contribution of current fragment
  vec3 result = texture(screenTexture, TexCoords).rgb * weight[0];

  if (horizontal) {
    for(int i = 1; i < 6; ++i)
    {
      result += texture(screenTexture,
	  TexCoords + vec2(texOffset.x * m*i, 0.0)).rgb * weight[i];
      result += texture(screenTexture,
	  TexCoords - vec2(texOffset.x * m*i, 0.0)).rgb * weight[i];
    }
  }
  else {
    for(int i = 1; i < 6; ++i)
    {
      result += texture(screenTexture,
	  TexCoords + vec2(0.0, texOffset.y * m*i)).rgb * weight[i];
      result += texture(screenTexture,
	  TexCoords - vec2(0.0, texOffset.y * m*i)).rgb * weight[i];
    }
  }

  FragColor = vec4(result, 1.0);
}
