////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_DEEP_SPACE_DOT_HPP
#define CSP_TRAJECTORIES_DEEP_SPACE_DOT_HPP

#include "Plugin.hpp"

#include "../../../src/cs-scene/CelestialObject.hpp"

#include <VistaBase/VistaColor.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <glm/glm.hpp>

namespace csp::trajectories {

/// A deep space dot is a simple marker indicating the position of an object, when it is too
/// small to see.
class DeepSpaceDot : public cs::scene::CelestialObject, public IVistaOpenGLDraw {
 public:
  cs::utils::Property<VistaColor> pColor = VistaColor(1, 1, 1); ///< The color of the marker.

  DeepSpaceDot(std::shared_ptr<Plugin::Properties> properties, std::string const& sCenterName,
      std::string const& sFrameName, double tStartExistence, double tEndExistence);
  ~DeepSpaceDot() override = default;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

 private:
  std::shared_ptr<Plugin::Properties> mProperties;
  VistaGLSLShader                     mShader;

  static const char* QUAD_VERT;
  static const char* QUAD_FRAG;
};
} // namespace csp::trajectories
#endif // CSP_TRAJECTORIES_DEEP_SPACE_DOT_HPP
