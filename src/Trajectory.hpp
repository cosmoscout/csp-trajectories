////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_TRAJECTORY_HPP
#define CSP_TRAJECTORIES_TRAJECTORY_HPP

#include "Plugin.hpp"

#include "../../../src/cs-scene/CelestialObject.hpp"
#include "../../../src/cs-scene/Trajectory.hpp"

#include <VistaBase/VistaColor.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <memory>

namespace csp::trajectories {

/// A trajectory trails behind an object in space to give a better understanding of its movement.
class Trajectory : public cs::scene::CelestialObject, public IVistaOpenGLDraw {
 public:
  /// The length of the trajectory in days.
  cs::utils::Property<double> pLength = 1.0;

  /// The color of the trajectory.
  cs::utils::Property<VistaColor> pColor = VistaColor(1, 1, 1);

  Trajectory(std::shared_ptr<Plugin::Properties> properties, std::string sTargetCenter,
      std::string sTargetFrame, std::string const& sParentCenter, std::string const& sParentFrame,
      unsigned uSamples, double tStartExistence, double tEndExistence);

  /// This is called automatically by the SolarSystem.
  void update(double tTime, cs::scene::CelestialObserver const& oObs) override;

  bool Do() override;
  bool GetBoundingBox(VistaBoundingBox& bb) override;

 private:
  std::shared_ptr<Plugin::Properties> mProperties;
  cs::scene::Trajectory               mTrajectory;
  std::string                         mTargetCenter;
  std::string                         mTargetFrame;

  std::vector<glm::dvec4> mPoints;
  unsigned                mSamples;
  int                     mStartIndex;
  double                  mLastSampleTime{};
  double                  mLastUpdateTime;
  double                  mLastFrameTime{};

  bool mTrailIsInExistence = false;
};

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_TRAJECTORY_HPP
