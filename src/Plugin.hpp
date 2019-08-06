////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_PLUGIN_HPP
#define CSP_TRAJECTORIES_PLUGIN_HPP

#include "../../../src/cs-core/PluginBase.hpp"
#include "../../../src/cs-core/Settings.hpp"
#include "../../../src/cs-utils/Property.hpp"

#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <optional>

namespace csp::trajectories {

class DeepSpaceDot;
class SunFlare;
class Trajectory;

/// This plugin is providing HUD elements that display trajectories and markers for orbiting
/// objects.
/// The configuration of this plugin is done via the provided json config. See README.md for
/// details.
class Plugin : public cs::core::PluginBase {
 public:
  /// Runtime properties for enabling and disabling features.
  struct Properties {
    cs::utils::Property<bool> mEnableTrajectories = true; ///< Toggles trajectories at runtime.
    cs::utils::Property<bool> mEnableSunFlares    = true; ///< Toggles flares at runtime.
    cs::utils::Property<bool> mEnablePlanetMarks  = true; ///< Toggles dots at runtime.
  };

  struct Settings {
    /// Settings for a trail behind an object.
    struct Trail {
      double  mLength;           ///< The length of the trail in days.
      int32_t mSamples;          ///< The amount of samples that make the trail up. The higher
                                 ///< the better it looks, but the worse the performance gets.
      std::string mParentCenter; ///< The spice name of the parents center.
      std::string mParentFrame;  ///< The spice name of the parents frame.
    };

    /// The root settings for a single trajectory.
    struct Trajectory {
      glm::vec3           mColor;     ///< Specifies the color of the trail and dot.
      std::optional<bool> mDrawDot;   ///< If available and true a dot will indicate the
                                      ///< objects position.
      std::optional<bool> mDrawFlare; ///< If available and true a flare will be drawn around
                                      ///< the object.
      std::optional<Trail> mTrail;    ///< If available a trail will be drawn behind the object.
    };

    std::map<std::string, Trajectory> mTrajectories; ///< All trajectories with their name as key.
  };

  Plugin();

  void init() override;
  void deInit() override;

 private:
  Settings                    mPluginSettings;
  std::shared_ptr<Properties> mProperties;

  std::vector<std::shared_ptr<Trajectory>>   mTrajectories;
  std::vector<std::shared_ptr<DeepSpaceDot>> mDeepSpaceDots;
  std::vector<std::shared_ptr<SunFlare>>     mSunFlares;
  std::vector<VistaOpenGLNode*>              mTrajectoryNodes;
  std::vector<VistaOpenGLNode*>              mDeepSpaceDotNodes;
  std::vector<VistaOpenGLNode*>              mSunFlareNodes;
};

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_PLUGIN_HPP
