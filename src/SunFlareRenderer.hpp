////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_SUN_FLARE_RENDERER_HPP
#define CSP_TRAJECTORIES_SUN_FLARE_RENDERER_HPP

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <memory>

namespace csp::trajectories {

class SunFlare;

class SunFlareRenderer : public IVistaOpenGLDraw {
 public:
  SunFlareRenderer();
  ~SunFlareRenderer() override = default;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

  void setSunFlares(const std::vector<std::shared_ptr<SunFlare>>& sunFlares);
 private:
  VistaGLSLShader        mShader;

  struct {
    uint32_t matModelView;
    uint32_t matProjection;
    uint32_t color;
    uint32_t aspect;
    uint32_t farClip;
  } mUniforms{};

  std::vector<std::shared_ptr<SunFlare>> mSunFlares;
};

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_SUN_FLARE_RENDERER_HPP
