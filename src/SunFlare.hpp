////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_SUN_FLARE_HPP
#define CSP_TRAJECTORIES_SUN_FLARE_HPP

#include "Plugin.hpp"

#include "../../../src/cs-scene/CelestialObject.hpp"

#include <VistaBase/VistaColor.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <glm/glm.hpp>

namespace cs::core {
class GraphicsEngine;
} // namespace cs::core

namespace csp::trajectories {

/// Adds an artificial flare effect around the object. Only makes sense for stars, but if you want
/// you can make anything glow like a christmas light :D. The SunFlare is hidden when HDR rendering
/// is enabled.
class SunFlare : public cs::scene::CelestialObject, public IVistaOpenGLDraw {
 public:
  /// The color of the flare.
  cs::utils::Property<VistaColor> pColor = VistaColor(1, 1, 1);

  SunFlare(std::shared_ptr<cs::core::GraphicsEngine> graphicsEngine,
      std::shared_ptr<Plugin::Properties> properties, std::string const& sCenterName,
      std::string const& sFrameName, double tStartExistence, double tEndExistence);

  SunFlare(SunFlare const& other) = delete;
  SunFlare(SunFlare&& other)      = delete;

  SunFlare& operator=(SunFlare const& other) = delete;
  SunFlare& operator=(SunFlare&& other) = delete;

  ~SunFlare() override = default;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

 private:
  std::shared_ptr<cs::core::GraphicsEngine> mGraphicsEngine;

  std::shared_ptr<Plugin::Properties> mProperties;
  VistaGLSLShader                     mShader;

  static const char* QUAD_VERT;
  static const char* QUAD_FRAG;
};

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_SUN_FLARE_HPP
