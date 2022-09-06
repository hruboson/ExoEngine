// FRAGMENT SHADER
#version 450

// IN
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
//layout (location = 3) in vec2 fragTexCoord;

// OUT
layout(location = 0) out vec4 outColor; // output variable

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

struct PointLight{
    vec4 position; // ignore w
    vec4 color; // w = intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
} ubo;

//layout (binding = 1) uniform sampler2D texSampler;

void main() {
  vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 surfaceNormal = normalize(fragNormalWorld);

  for (int i = 0; i < ubo.numLights; i++) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0; // / dot(directionToLight, directionToLight); // distance squared // dont count in this app (space-like lighting)
    float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngIncidence;
  }
  
  outColor = vec4(diffuseLight * fragColor, 1.00);
  //outColor = vec4(diffuseLight * vec3(texture(texSampler, fragTexCoord)), 1.0);
}