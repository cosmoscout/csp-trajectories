////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSP_TRAJECTORIES_LOGGER_HPP
#define CSP_TRAJECTORIES_LOGGER_HPP

#include <spdlog/spdlog.h>

namespace csp::trajectories {

/// This creates the default singleton logger for "csp-trajectories" when called for the first time
/// and returns it. See cs-utils/logger.hpp for more logging details.
std::shared_ptr<spdlog::logger> logger();

} // namespace csp::trajectories

#endif // CSP_TRAJECTORIES_LOGGER_HPP
