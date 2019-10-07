#version 330

out vec2 vTexCoords;
out float fDepth;

uniform float uAspect;
uniform mat4 uMatModelView;
uniform mat4 uMatProjection;

void main() {
    vec4 posVS = uMatModelView * vec4(0, 0, 0, 1);
    fDepth = length(posVS.xyz);

    vec4 posP = uMatProjection * posVS;
    float scale = length(uMatModelView[0]) / length(posVS.xyz);

    if (posP.w < 0) {
        gl_Position = vec4(0);
        return;
    }

    posP /= posP.w;

    float h = scale * 10e10;
    float w = h / uAspect;

    posP.z = 0.999;

    switch (gl_VertexID) {
        case 0:
            posP.xy += vec2(-w, h);
            vTexCoords = vec2(-1, 1);
        break;
        case 1:
            posP.xy += vec2(w, h);
            vTexCoords = vec2(1, 1);
        break;
        case 2:
            posP.xy += vec2(-w, -h);
            vTexCoords = vec2(-1, -1);
        break;
        default :
            posP.xy += vec2(w, -h);
            vTexCoords = vec2(1, -1);
        break;
    }

    gl_Position = posP;
}