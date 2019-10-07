////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_DEEP_SPACE_DOT_RENDERER_HPP
#define CSP_TRAJECTORIES_DEEP_SPACE_DOT_RENDERER_HPP

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaOGLExt/VistaGLSLShader.h>

#include <vector>
#include <memory>

namespace csp::trajectories {

class DeepSpaceDot;

class DeepSpaceDotRenderer : public IVistaOpenGLDraw {
 public:
  DeepSpaceDotRenderer();
  ~DeepSpaceDotRenderer() override = default;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

  void setDeepSpaceDots(const std::vector<std::shared_ptr<DeepSpaceDot>>& dots);

 private:
  VistaGLSLShader        mShader;

  struct {
    uint32_t matModelView;
    uint32_t matProjection;
    uint32_t color;
    uint32_t aspect;
    uint32_t farClip;
  } mUniforms{};

  std::vector<std::shared_ptr<DeepSpaceDot>> mDots;
};

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_DEEP_SPACE_DOT_RENDERER_HPP
