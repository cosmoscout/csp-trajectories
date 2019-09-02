////////////////////////////////////////////////////////////////////////////////////////////////////
//                               This file is part of CosmoScout VR                               //
//      and may be used under the terms of the MIT license. See the LICENSE file for details.     //
//                        Copyright: (c) 2019 German Aerospace Center (DLR)                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Plugin.hpp"

#include "DeepSpaceDot.hpp"
#include "SunFlare.hpp"
#include "Trajectory.hpp"

#include "../../../src/cs-core/GuiManager.hpp"
#include "../../../src/cs-core/SolarSystem.hpp"
#include "../../../src/cs-utils/convert.hpp"
#include "../../../src/cs-utils/utils.hpp"

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernelOpenSGExt/VistaOpenSGMaterialTools.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT_FN cs::core::PluginBase* create() {
  return new csp::trajectories::Plugin;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT_FN void destroy(cs::core::PluginBase* pluginBase) {
  delete pluginBase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace csp::trajectories {

////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(const nlohmann::json& j, Plugin::Settings::Trail& o) {
  o.mLength       = cs::core::parseProperty<double>("length", j);
  o.mSamples      = cs::core::parseProperty<int32_t>("samples", j);
  o.mParentCenter = cs::core::parseProperty<std::string>("parentCenter", j);
  o.mParentFrame  = cs::core::parseProperty<std::string>("parentFrame", j);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(const nlohmann::json& j, Plugin::Settings::Trajectory& o) {
  nlohmann::json c = j.at("color");
  for (int i = 0; i < 3; ++i)
    o.mColor[i] = c.at(i);

  o.mDrawDot   = cs::core::parseOptional<bool>("drawDot", j);
  o.mDrawFlare = cs::core::parseOptional<bool>("drawFlare", j);

  o.mTrail = cs::core::parseOptionalSection<Plugin::Settings::Trail>("trail", j);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(const nlohmann::json& j, Plugin::Settings& o) {
  cs::core::parseSection("csp-trajectories", [&] {
    o.mTrajectories =
        cs::core::parseMap<std::string, Plugin::Settings::Trajectory>("trajectories", j);
  });
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin::Plugin()
    : mProperties(std::make_shared<Properties>()) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin::init() {
  std::cout << "Loading: CosmoScout VR Plugin Trajectories" << std::endl;

  mPluginSettings = mAllSettings->mPlugins.at("csp-trajectories");

  for (auto const& settings : mPluginSettings.mTrajectories) {
    auto anchor = mAllSettings->mAnchors.find(settings.first);

    if (anchor == mAllSettings->mAnchors.end()) {
      throw std::runtime_error(
          "There is no Anchor \"" + settings.first + "\" defined in the settings.");
    }

    auto   existence       = cs::utils::convert::getExistenceFromSettings(*anchor);
    double tStartExistence = existence.first;
    double tEndExistence   = existence.second;

    // sun flare ---------------------------------------------------------------
    if (settings.second.mDrawFlare && *settings.second.mDrawFlare) {
      auto flare = std::make_shared<SunFlare>(mProperties, anchor->second.mCenter,
          anchor->second.mFrame, tStartExistence, tEndExistence);
      mSolarSystem->registerAnchor(flare);

      flare->pColor =
          VistaColor(settings.second.mColor.r, settings.second.mColor.g, settings.second.mColor.b);

      auto sunFlareNode = mSceneGraph->NewOpenGLNode(mSceneGraph->GetRoot(), flare.get());

      VistaOpenSGMaterialTools::SetSortKeyOnSubtree(
          sunFlareNode, static_cast<int>(cs::utils::DrawOrder::eAtmospheres) + 1);

      mSunFlares.push_back(flare);
      mSunFlareNodes.push_back(sunFlareNode);
    }

    if (settings.second.mTrail) {
      auto trajectory =
          std::make_shared<Trajectory>(mProperties, anchor->second.mCenter, anchor->second.mFrame,
              settings.second.mTrail->mParentCenter, settings.second.mTrail->mParentFrame,
              settings.second.mTrail->mSamples, tStartExistence, tEndExistence);
      mSolarSystem->registerAnchor(trajectory);

      trajectory->pLength = settings.second.mTrail->mLength;
      trajectory->pColor =
          VistaColor(settings.second.mColor.r, settings.second.mColor.g, settings.second.mColor.b);

      auto trajectoryNode = mSceneGraph->NewOpenGLNode(mSceneGraph->GetRoot(), trajectory.get());
      VistaOpenSGMaterialTools::SetSortKeyOnSubtree(
          trajectoryNode, static_cast<int>(cs::utils::DrawOrder::eTransparentItems) - 1);
      mTrajectoryNodes.push_back(trajectoryNode);
      mTrajectories.push_back(trajectory);
    }

    if (settings.second.mDrawDot && *settings.second.mDrawDot) {
      auto dot = std::make_shared<DeepSpaceDot>(mProperties, anchor->second.mCenter,
          anchor->second.mFrame, tStartExistence, tEndExistence);
      mSolarSystem->registerAnchor(dot);

      dot->pColor =
          VistaColor(settings.second.mColor.r, settings.second.mColor.g, settings.second.mColor.b);

      auto deepSpaceDotNode = mSceneGraph->NewOpenGLNode(mSceneGraph->GetRoot(), dot.get());
      VistaOpenSGMaterialTools::SetSortKeyOnSubtree(
          deepSpaceDotNode, static_cast<int>(cs::utils::DrawOrder::eTransparentItems) - 1);
      mDeepSpaceDotNodes.push_back(deepSpaceDotNode);

      // do not perform distance culling for DeepSpaceDots
      dot->pVisibleRadius = -1;

      if (settings.second.mTrail) {
        // hide dot as soon as the corresponding trajectory is hidden
        dot->pVisible.connectFrom(mTrajectories.back()->pVisible);
      } else {
        dot->pVisible = true;
      }

      mDeepSpaceDots.push_back(dot);
    }
  }

  mGuiManager->addSettingsSectionToSideBarFromHTML("Trajectories", "radio_button_unchecked",
      "../share/resources/gui/trajectories-settings.html");

  mGuiManager->getSideBar()->registerCallback<bool>("set_enable_trajectories",
      ([this](bool value) { mProperties->mEnableTrajectories = value; }));

  mGuiManager->getSideBar()->registerCallback<bool>(
      "set_enable_planet_marks", ([this](bool value) { mProperties->mEnablePlanetMarks = value; }));

  mGuiManager->getSideBar()->registerCallback<bool>(
      "set_enable_sun_flare", ([this](bool value) { mProperties->mEnableSunFlares = value; }));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin::deInit() {
  for (auto const& flare : mSunFlares) {
    mSolarSystem->unregisterAnchor(flare);
  }
  for (auto const& flareNode : mSunFlareNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(flareNode);
  }

  for (auto const& trajectory : mTrajectories) {
    mSolarSystem->unregisterAnchor(trajectory);
  }
  for (auto const& trajectoryNode : mTrajectoryNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(trajectoryNode);
  }

  for (auto const& dot : mDeepSpaceDots) {
    mSolarSystem->unregisterAnchor(dot);
  }
  for (auto const& deepSpaceDotNode : mDeepSpaceDotNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(deepSpaceDotNode);
  }

  mGuiManager->getSideBar()->unregisterCallback("set_enable_trajectories");
  mGuiManager->getSideBar()->unregisterCallback("set_enable_planet_marks");
  mGuiManager->getSideBar()->unregisterCallback("set_enable_sun_flare");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
