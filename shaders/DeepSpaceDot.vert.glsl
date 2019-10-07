#version 330

out vec2 vTexCoords;
out float fDepth;

uniform float uAspect;
uniform mat4 uMatModelView;
uniform mat4 uMatProjection;

void main() {
    vec4 pos = uMatModelView * vec4(0, 0, 0, 1);
    fDepth = length(pos.xyz);

    pos = uMatProjection * pos;

    if (pos.w < 0) {
        gl_Position = vec4(0);
        return;
    }

    pos /= pos.w;

    float h = 0.0075;
    float w = h / uAspect;

    pos.z = 0.9999999;

    switch (gl_VertexID) {
        case 0:
            pos.xy += vec2(-w, h);
            vTexCoords = vec2(-1, 1);
        break;
        case 1:
            pos.xy += vec2(w, h);
            vTexCoords = vec2(1, 1);
        break;
        case 2:
            pos.xy += vec2(-w, -h);
            vTexCoords = vec2(-1, -1);
        break;
        default :
            pos.xy += vec2(w, -h);
            vTexCoords = vec2(1, -1);
        break;
    }

    gl_Position = pos;
}