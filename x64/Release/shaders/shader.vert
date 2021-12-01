//VERTEX SHADER
#version 450 // version

// IN
// location = location in ExoModel::getAttributeDescriptions()
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec2 texCoord;

// OUT
layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragPosWorld;
layout (location = 2) out vec3 fragNormalWorld;

//layout (location = 1) out vec2 fragTexCoord;

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

const float AMBIENT = 0.02;

void main() { // runs for each vertex
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);  // 1.0 is homogeneous coordinate
	gl_Position = ubo.projectionViewMatrix * positionWorld;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = positionWorld.xyz;
	fragColor = color;
}
