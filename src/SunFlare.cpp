////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SunFlare.hpp"

#include "../../../src/cs-core/GraphicsEngine.hpp"
#include "../../../src/cs-utils/FrameTimings.hpp"
#include "../../../src/cs-utils/utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string SunFlare::QUAD_VERT = R"(
#version 330

out vec2 vTexCoords;
out float fDepth;

uniform float uAspect;
uniform mat4 uMatModelView;
uniform mat4 uMatProjection;

void main()
{
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
            posP.xy += vec2(-w,  h);
            vTexCoords = vec2(-1, 1);
            break;
        case 1:
            posP.xy += vec2( w,  h);
            vTexCoords = vec2(1, 1);
            break;
        case 2:
            posP.xy += vec2(-w, -h);
            vTexCoords = vec2(-1, -1);
            break;
        default:
            posP.xy += vec2( w, -h);
            vTexCoords = vec2(1, -1);
            break;
    }

    gl_Position = posP;
}
)";

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string SunFlare::QUAD_FRAG = R"(
#version 330

uniform vec3 uCcolor;
uniform float uFarClip;

in vec2 vTexCoords;
in float fDepth;

layout(location = 0) out vec3 oColor;

void main()
{
    // sun disc
    float dist = length(vTexCoords) * 100;
    float glow = exp(1.0 - dist);
    oColor = uCcolor * glow;
    
    // sun glow
    dist = min(1.0, length(vTexCoords));
    glow = 1.0 - pow(dist, 0.05);
    oColor += uCcolor * glow * 2;

    gl_FragDepth = fDepth / uFarClip;
}
)";

////////////////////////////////////////////////////////////////////////////////////////////////////

SunFlare::SunFlare(std::shared_ptr<cs::core::GraphicsEngine> const& graphicsEngine,
    std::shared_ptr<Plugin::Properties> const& properties, std::string const& sCenterName,
    std::string const& sFrameName, double tStartExistence, double tEndExistence)
    : cs::scene::CelestialObject(sCenterName, sFrameName, tStartExistence, tEndExistence)
    , mGraphicsEngine(graphicsEngine)
    , mProperties(properties) {
  mShader.InitVertexShaderFromString(QUAD_VERT);
  mShader.InitFragmentShaderFromString(QUAD_FRAG);
  mShader.Link();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool SunFlare::Do() {
  if (mProperties->mEnableSunFlares.get() && getIsInExistence() &&
      !mGraphicsEngine->pEnableHDR.get()) {
    cs::utils::FrameTimings::ScopedTimer timer("SunFlare");
    // get viewport to draw dot with correct aspect ration
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float fAspect = 1.f * viewport[2] / viewport[3];

    // get modelview and projection matrices
    GLfloat glMatMV[16], glMatP[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, &glMatMV[0]);
    glGetFloatv(GL_PROJECTION_MATRIX, &glMatP[0]);
    auto matMV = glm::make_mat4x4(glMatMV) * glm::mat4(getWorldTransform());

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);

    // draw simple dot
    mShader.Bind();
    glUniformMatrix4fv(
        mShader.GetUniformLocation("uMatModelView"), 1, GL_FALSE, glm::value_ptr(matMV));
    glUniformMatrix4fv(mShader.GetUniformLocation("uMatProjection"), 1, GL_FALSE, glMatP);
    mShader.SetUniform(
        mShader.GetUniformLocation("uCcolor"), pColor.get()[0], pColor.get()[1], pColor.get()[2]);
    mShader.SetUniform(mShader.GetUniformLocation("uAspect"), fAspect);
    mShader.SetUniform(
        mShader.GetUniformLocation("uFarClip"), cs::utils::getCurrentFarClipDistance());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    mShader.Release();

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool SunFlare::GetBoundingBox(VistaBoundingBox& bb) {
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
