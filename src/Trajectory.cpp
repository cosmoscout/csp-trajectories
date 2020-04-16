////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Trajectory.hpp"

#include "../../../src/cs-scene/CelestialObserver.hpp"
#include "../../../src/cs-utils/FrameTimings.hpp"
#include "logger.hpp"

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

Trajectory::Trajectory(std::shared_ptr<Plugin::Properties> properties, std::string sTargetCenter,
    std::string sTargetFrame, std::string const& sParentCenter, std::string const& sParentFrame,
    unsigned uSamples, double tStartExistence, double tEndExistence)
    : cs::scene::CelestialObject(sParentCenter, sParentFrame, tStartExistence, tEndExistence)
    , mProperties(std::move(properties))
    , mTargetCenter(std::move(sTargetCenter))
    , mTargetFrame(std::move(sTargetFrame))
    , mSamples(uSamples)
    , mStartIndex(0)
    , mLastUpdateTime(-1.0) {
  pLength.connect([this](double val) {
    mPoints.clear();
    mTrajectory.setMaxAge(val * 24 * 60 * 60);
  });

  pColor.connect([this](VistaColor const& val) {
    mTrajectory.setStartColor(glm::vec4(val[0], val[1], val[2], 1.F));
    mTrajectory.setEndColor(glm::vec4(val[0], val[1], val[2], 0.F));
  });

  mTrajectory.setUseLinearDepthBuffer(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Trajectory::update(double tTime, cs::scene::CelestialObserver const& oObs) {
  cs::scene::CelestialObject::update(tTime, oObs);

  double dLengthSeconds = pLength.get() * 24.0 * 60.0 * 60.0;
  mTrailIsInExistence   = (tTime > mStartExistence && tTime < mEndExistence + dLengthSeconds);

  if (mProperties->mEnableTrajectories.get() && mTrailIsInExistence) {
    double dSampleLength = dLengthSeconds / mSamples;

    cs::scene::CelestialAnchor target(mTargetCenter, mTargetFrame);

    // only recalculate if there is not too much change from frame to frame
    if (std::abs(mLastFrameTime - tTime) <= dLengthSeconds / 10.0) {
      // make sure to re-sample entire trajectory if complete reset is required
      bool completeRecalculation = false;

      if (mPoints.size() != mSamples) {
        mPoints.resize(mSamples);
        completeRecalculation = true;
      }

      if (tTime > mLastSampleTime + dLengthSeconds || tTime < mLastSampleTime - dLengthSeconds) {
        completeRecalculation = true;
      }

      if (mLastUpdateTime < tTime) {
        if (completeRecalculation) {
          mLastSampleTime = tTime - dLengthSeconds - dSampleLength;
          mStartIndex     = 0;
        }

        while (mLastSampleTime < tTime) {
          mLastSampleTime += dSampleLength;

          try {
            double     tSampleTime = glm::clamp(mLastSampleTime, mStartExistence, mEndExistence);
            glm::dvec3 pos         = getRelativePosition(tSampleTime, target);
            mPoints[mStartIndex]   = glm::dvec4(pos.x, pos.y, pos.z, tSampleTime);

            pVisibleRadius = std::max(glm::length(pos), pVisibleRadius.get());

            mStartIndex = (mStartIndex + 1) % static_cast<int>(mSamples);
          } catch (...) {
            // data might be unavailable
          }
        }
      } else {
        if (completeRecalculation) {
          mLastSampleTime = tTime + dLengthSeconds + dSampleLength;
          mStartIndex     = 0;
        }

        while (mLastSampleTime - dSampleLength > tTime) {
          mLastSampleTime -= dSampleLength;

          try {
            double tSampleTime =
                glm::clamp(mLastSampleTime - dLengthSeconds, mStartExistence, mEndExistence);
            glm::dvec3 pos = getRelativePosition(tSampleTime, target);
            mPoints[(mStartIndex - 1 + mSamples) % mSamples] =
                glm::dvec4(pos.x, pos.y, pos.z, tSampleTime);

            mStartIndex =
                (mStartIndex - 1 + static_cast<int>(mSamples)) % static_cast<int>(mSamples);
            pVisibleRadius = std::max(glm::length(pos), pVisibleRadius.get());
          } catch (...) {
            // data might be unavailable
          }
        }
      }

      mLastUpdateTime = tTime;

      if (completeRecalculation) {
        logger()->debug("Recalculating trajectory for {}.", mTargetCenter);
      }
    }

    mLastFrameTime = tTime;

    if (pVisible.get()) {
      glm::dvec3 tip = getRelativePosition(tTime, target);
      mTrajectory.upload(matWorldTransform, tTime, mPoints, tip, mStartIndex);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool Trajectory::Do() {
  if (mProperties->mEnableTrajectories.get() && pVisible.get() && mTrailIsInExistence) {
    cs::utils::FrameTimings::ScopedTimer timer("Trajectories");
    mTrajectory.Do();
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool Trajectory::GetBoundingBox(VistaBoundingBox& /*bb*/) {
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
