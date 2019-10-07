#version 330

uniform vec3 uColor;
uniform float uFarClip;

in vec2 vTexCoords;
in float fDepth;

layout(location = 0) out vec3 oColor;

void main() {
    // sun disc
    float dist = length(vTexCoords) * 100;
    float glow = exp(1.0 - dist);
    oColor = uColor * glow;

    // sun glow
    dist = min(1.0, length(vTexCoords));
    glow = 1.0 - pow(dist, 0.05);
    oColor += uColor * glow * 2;

    gl_FragDepth = fDepth / uFarClip;
}