////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DeepSpaceDotRenderer.hpp"

#include "DeepSpaceDot.hpp"

#include "../../../src/cs-utils/FrameTimings.hpp"
#include "../../../src/cs-utils/filesystem.hpp"
#include "../../../src/cs-utils/utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

DeepSpaceDotRenderer::DeepSpaceDotRenderer() {
  mShader.InitVertexShaderFromString(
      cs::utils::filesystem::loadToString("../share/resources/shaders/DeepSpaceDot.vert.glsl"));
  mShader.InitFragmentShaderFromString(
      cs::utils::filesystem::loadToString("../share/resources/shaders/DeepSpaceDot.frag.glsl"));
  mShader.Link();

  mUniforms.matModelView  = mShader.GetUniformLocation("uMatModelView");
  mUniforms.matProjection = mShader.GetUniformLocation("uMatProjection");
  mUniforms.color         = mShader.GetUniformLocation("uColor");
  mUniforms.aspect        = mShader.GetUniformLocation("uAspect");
  mUniforms.farClip       = mShader.GetUniformLocation("uFarClip");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool DeepSpaceDotRenderer::Do() {
  cs::utils::FrameTimings::ScopedTimer timer("Planet Marks");

  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  mShader.Bind();

  // get viewport to draw dot with correct aspect ration
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  float fAspect = 1.f * viewport[2] / viewport[3];
  mShader.SetUniform(mUniforms.aspect, fAspect);

  // get model view and projection matrices
  GLfloat glMatMV[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, &glMatMV[0]);

  GLfloat glMatP[16];
  glGetFloatv(GL_PROJECTION_MATRIX, &glMatP[0]);
  glUniformMatrix4fv(mUniforms.matProjection, 1, GL_FALSE, glMatP);

  mShader.SetUniform(mUniforms.farClip, cs::utils::getCurrentFarClipDistance());

  for (const auto& dot : mDots) {
    if (dot->mProperties->mEnablePlanetMarks.get() && dot->getIsInExistence() &&
        dot->pVisible.get()) {
      auto matMV = glm::make_mat4x4(glMatMV) * glm::mat4(dot->getWorldTransform());
      glUniformMatrix4fv(mUniforms.matModelView, 1, GL_FALSE, glm::value_ptr(matMV));

      mShader.SetUniform(
          mUniforms.color, dot->pColor.get()[0], dot->pColor.get()[1], dot->pColor.get()[2]);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  mShader.Release();

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool DeepSpaceDotRenderer::GetBoundingBox(VistaBoundingBox& bb) {
  return false;
}

void DeepSpaceDotRenderer::setDeepSpaceDots(const std::vector<std::shared_ptr<DeepSpaceDot>>& dots) {
  mDots = dots;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories