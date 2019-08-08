# ------------------------------------------------------------------------------------------------ #
#                                This file is part of CosmoScout VR                                #
#       and may be used under the terms of the MIT license. See the LICENSE file for details.      #
#                         Copyright: (c) 2019 German Aerospace Center (DLR)                        #
# ------------------------------------------------------------------------------------------------ #

# build plugin -------------------------------------------------------------------------------------

file(GLOB SOURCE_FILES src/*.cpp)

add_library(csp-trajectories SHARED
  ${SOURCE_FILES}
)

target_link_libraries(csp-trajectories
  PUBLIC
    cs-core
)

# install plugin -----------------------------------------------------------------------------------

install(TARGETS   csp-trajectories   DESTINATION "share/plugins")
install(DIRECTORY "gui"              DESTINATION "share/resources")