#version 330

uniform vec3 uColor;
uniform float uFarClip;

in vec2 vTexCoords;
in float fDepth;

layout(location = 0) out vec4 oColor;

void main() {
    float dist = length(vTexCoords);
    float blob = pow(dist, 10.0);
    oColor  = mix(vec4(uColor, 1.0), vec4(0), blob);

    // substract a small value to prevent depth fighting with trajectories
    gl_FragDepth = fDepth / uFarClip - 0.00001;
}