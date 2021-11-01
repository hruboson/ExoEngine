// FRAGMENT SHADER
#version 450

// IN
layout(location = 0) in vec3 fragColor;
//layout(location = 1) in vec2 fragTexCoord;

// OUT
layout(location = 0) out vec4 outColor; // output variable

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
  outColor = vec4(fragColor, 1.0);
}
