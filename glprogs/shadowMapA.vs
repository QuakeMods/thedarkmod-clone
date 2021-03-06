#version 140

uniform vec4 u_lightOrigin;
uniform mat4 u_modelMatrix;
uniform float u_lightRadius;

in vec4 attr_Position;
in vec4 attr_TexCoord;

out vec2 texCoord;

const mat3 cubicTransformations[6] = mat3[6] (
	mat3(
		0, 0, -1,
		0, -1, 0,
		-1, 0, 0
	),
	mat3(
		0, 0, 1,
		0, -1, 0,
		1, 0, 0
	),
	mat3(
		1, 0, 0,
		0, 0, -1,
		0, 1, 0
	),
	mat3(
		1, 0, 0,
		0, 0, 1,
		0, -1, 0
	),
	mat3(
		1, 0, 0,
		0, -1, 0,
		0, 0, -1
	),
	mat3(
		-1, 0, 0,
		0, -1, 0,
		0, 0, 1
	)
);

const float clipEps = 0e-2;
const vec4 ClipPlanes[4] = vec4[4] (
	vec4(1, 0, -1, clipEps),
	vec4(-1, 0, -1, clipEps),
	vec4(0, 1, -1, clipEps),
	vec4(0, -1, -1, clipEps)
);

void main() {
	texCoord = (attr_TexCoord).st;
	gl_Position = u_modelMatrix * attr_Position - u_lightOrigin;
            vec4 frag_pos = vec4(cubicTransformations[gl_InstanceID] * gl_Position.xyz, 1);
            gl_Position.x = frag_pos.x / 6 + frag_pos.z * 5/6 - frag_pos.z / 3 * gl_InstanceID;
            gl_Position.y = frag_pos.y;
            gl_Position.z = -frag_pos.z - 2*u_lightRadius;
            gl_Position.w = -frag_pos.z;
			gl_ClipDistance[0] = dot(frag_pos, ClipPlanes[0]);
			gl_ClipDistance[1] = dot(frag_pos, ClipPlanes[1]);
			gl_ClipDistance[2] = dot(frag_pos, ClipPlanes[2]);
			gl_ClipDistance[3] = dot(frag_pos, ClipPlanes[3]);
}