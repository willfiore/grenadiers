#version 330 core
layout (std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};
uniform float time;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  vec3 lightColor_hsv = vec3(0.79, 1.0, 0.7);
  vec3 lightColor = hsv2rgb(lightColor_hsv);

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(vec3(0.0, -1.0, 0.2));

  // Ambient
  float ambientStrength = 0.0;
  vec3 ambient = lightColor;
  ambient *= ambientStrength;

  // Diffuse
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // Specular
  vec3 result = ambient + diffuse;

  float marginSize = 8.f;
  if (FragPos.z > 100.f - marginSize ||
      FragPos.z < -100.f + marginSize)
    result = lightColor;
  else
    result *= 0.2;

  FragColor = vec4(result, 1.0);
}
