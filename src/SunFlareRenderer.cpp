////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SunFlareRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "SunFlare.hpp"

#include "../../../src/cs-utils/FrameTimings.hpp"
#include "../../../src/cs-utils/filesystem.hpp"
#include "../../../src/cs-utils/utils.hpp"

namespace csp::trajectories {

SunFlareRenderer::SunFlareRenderer() {
  mShader.InitVertexShaderFromString(
      cs::utils::filesystem::loadToString("../share/resources/shaders/SunFlare.vert.glsl"));
  mShader.InitFragmentShaderFromString(
      cs::utils::filesystem::loadToString("../share/resources/shaders/SunFlare.frag.glsl"));
  mShader.Link();

  mUniforms.matModelView  = mShader.GetUniformLocation("uMatModelView");
  mUniforms.matProjection = mShader.GetUniformLocation("uMatProjection");
  mUniforms.color         = mShader.GetUniformLocation("uColor");
  mUniforms.aspect        = mShader.GetUniformLocation("uAspect");
  mUniforms.farClip       = mShader.GetUniformLocation("uFarClip");
}

bool SunFlareRenderer::Do() {
  cs::utils::FrameTimings::ScopedTimer timer("SunFlare");

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glDepthMask(GL_FALSE);

  // draw simple dot
  mShader.Bind();

  // get viewport to draw dot with correct aspect ration
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  float fAspect = 1.f * viewport[2] / viewport[3];
  mShader.SetUniform(mUniforms.aspect, fAspect);
  mShader.SetUniform(mUniforms.farClip, cs::utils::getCurrentFarClipDistance());

  // get modelview and projection matrices
  GLfloat glMatMV[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, &glMatMV[0]);

  GLfloat glMatP[16];
  glGetFloatv(GL_PROJECTION_MATRIX, &glMatP[0]);
  glUniformMatrix4fv(mUniforms.matProjection, 1, GL_FALSE, glMatP);

  for (const auto& flare : mSunFlares) {
    if (flare->mProperties->mEnableSunFlares.get() && flare->getIsInExistence()) {
      auto matMV = glm::make_mat4x4(glMatMV) * glm::mat4(flare->getWorldTransform());
      glUniformMatrix4fv(mUniforms.matModelView, 1, GL_FALSE, glm::value_ptr(matMV));

      mShader.SetUniform(
          mUniforms.color, flare->pColor.get()[0], flare->pColor.get()[1], flare->pColor.get()[2]);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  mShader.Release();

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);

  return true;
}

bool SunFlareRenderer::GetBoundingBox(VistaBoundingBox& bb) {
  return false;
}

void SunFlareRenderer::setSunFlares(const std::vector<std::shared_ptr<SunFlare>>& sunFlares) {
  mSunFlares = sunFlares;
}
} // namespace csp::trajectories