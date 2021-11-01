//VERTEX SHADER
#version 450 // version

// IN
// location = location in ExoModel::getAttributeDescriptions()
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec2 texCoord;

// OUT
layout (location = 0) out vec3 fragColor;
//layout (location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projectionViewMatrix;
	vec3 directionToLight;
} ubo;

const float AMBIENT = 0.02;

void main() { // runs for each vertex
  gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(inPosition, 1.0); // 1.0 is homogeneous coordinate

  // temporary
  vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

  // for space-like lighting remove ambient light (reflected light)
  float lightIntensity = AMBIENT + max(dot(normalWorldSpace, ubo.directionToLight), 0);

  fragColor = lightIntensity * color;
  //fragTexCoord = lightIntensity * texCoord;
}
