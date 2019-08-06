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

namespace csp::trajectories {

/// Adds a flare effect around the object. Only makes sense for stars, but if you want you can
/// make anything glow like a christmas light :D
class SunFlare : public cs::scene::CelestialObject, public IVistaOpenGLDraw {
 public:
  cs::utils::Property<VistaColor> pColor = VistaColor(1, 1, 1); ///< The color of the flare.

  SunFlare(std::shared_ptr<Plugin::Properties> const& properties, std::string const& sCenterName,
      std::string const& sFrameName, double tStartExistence, double tEndExistence);
  ~SunFlare() override = default;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

 private:
  std::shared_ptr<Plugin::Properties> mProperties;
  VistaGLSLShader                     mShader;

  static const std::string QUAD_VERT;
  static const std::string QUAD_FRAG;
};
} // namespace csp::trajectories
#endif // CSP_TRAJECTORIES_SUN_FLARE_HPP
