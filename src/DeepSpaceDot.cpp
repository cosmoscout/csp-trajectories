////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DeepSpaceDot.hpp"

#include "../../../src/cs-utils/FrameTimings.hpp"
#include "../../../src/cs-utils/utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string DeepSpaceDot::QUAD_VERT = R"(
#version 400 compatibility

out vec2 vTexCoords;
out float fDepth;

uniform float uAspect;
uniform mat4 uMatModelView;
uniform mat4 uMatProjection;

void main()
{
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
            pos.xy += vec2(-w,  h);
            vTexCoords = vec2(-1, 1);
            break;
        case 1:
            pos.xy += vec2( w,  h);
            vTexCoords = vec2(1, 1);
            break;
        case 2:
            pos.xy += vec2(-w, -h);
            vTexCoords = vec2(-1, -1);
            break;
        default:
            pos.xy += vec2( w, -h);
            vTexCoords = vec2(1, -1);
            break;
    }

    gl_Position = pos;
}
)";

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string DeepSpaceDot::QUAD_FRAG = R"(
#version 400 compatibility

uniform vec3 uCcolor;
uniform float uFarClip;

in vec2 vTexCoords;
in float fDepth;

layout(location = 0) out vec4 oColor;

void main()
{
    float dist = length(vTexCoords);
    float blob = pow(dist, 10.0);
    oColor  = mix(vec4(uCcolor, 1.0), vec4(0), blob);
    
    // substract a small value to prevent depth fighting with trajectories
    gl_FragDepth = fDepth / uFarClip - 0.00001;
}
)";

////////////////////////////////////////////////////////////////////////////////////////////////////

DeepSpaceDot::DeepSpaceDot(std::shared_ptr<Plugin::Properties> const& properties,
    std::string const& sCenterName, std::string const& sFrameName, double tStartExistence,
    double tEndExistence)
    : cs::scene::CelestialObject(sCenterName, sFrameName, tStartExistence, tEndExistence)
    , mProperties(properties) {
  mShader.InitVertexShaderFromString(QUAD_VERT);
  mShader.InitFragmentShaderFromString(QUAD_FRAG);
  mShader.Link();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool DeepSpaceDot::Do() {
  if (mProperties->mEnablePlanetMarks.get() && getIsInExistence() && pVisible.get()) {
    cs::utils::FrameTimings::ScopedTimer timer("Planet Marks");
    // get viewport to draw dot with correct aspect ration
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float fAspect = 1.f * viewport[2] / viewport[3];

    // get model view and projection matrices
    GLfloat glMatMV[16], glMatP[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, &glMatMV[0]);
    glGetFloatv(GL_PROJECTION_MATRIX, &glMatP[0]);
    auto matMV = glm::make_mat4x4(glMatMV) * glm::mat4(getWorldTransform());

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

bool DeepSpaceDot::GetBoundingBox(VistaBoundingBox& bb) {
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
