// FRAGMENT SHADER
#version 450

// IN
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
//layout(location = 1) in vec2 fragTexCoord;

// OUT
layout(location = 0) out vec4 outColor; // output variable

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projectionViewMatrix;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor; // w = light intensity
} ubo;

void main() {
	vec3 directionToLight = ubo.lightPosition - fragPosWorld.xyz;
	float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance ^2
	
	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}
