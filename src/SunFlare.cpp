////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SunFlare.hpp"

#include "../../../src/cs-utils/FrameTimings.hpp"
#include "../../../src/cs-utils/utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

SunFlare::SunFlare(std::shared_ptr<Plugin::Properties> const& properties,
    std::string const& sCenterName, std::string const& sFrameName, double tStartExistence,
    double tEndExistence)
    : cs::scene::CelestialObject(sCenterName, sFrameName, tStartExistence, tEndExistence)
    , mProperties(properties) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
