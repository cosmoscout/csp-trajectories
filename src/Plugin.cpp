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
#include "../../../src/cs-utils/logger.hpp"

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

void from_json(nlohmann::json const& j, Plugin::Settings::Trail& o) {
  cs::core::Settings::deserialize(j, "length", o.mLength);
  cs::core::Settings::deserialize(j, "samples", o.mSamples);
  cs::core::Settings::deserialize(j, "parentCenter", o.mParentCenter);
  cs::core::Settings::deserialize(j, "parentFrame", o.mParentFrame);
}

void to_json(nlohmann::json& j, Plugin::Settings::Trail const& o) {
  cs::core::Settings::serialize(j, "length", o.mLength);
  cs::core::Settings::serialize(j, "samples", o.mSamples);
  cs::core::Settings::serialize(j, "parentCenter", o.mParentCenter);
  cs::core::Settings::serialize(j, "parentFrame", o.mParentFrame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(nlohmann::json const& j, Plugin::Settings::Trajectory& o) {
  cs::core::Settings::deserialize(j, "color", o.mColor);
  cs::core::Settings::deserialize(j, "drawDot", o.mDrawDot);
  cs::core::Settings::deserialize(j, "drawFlare", o.mDrawFlare);
  cs::core::Settings::deserialize(j, "trail", o.mTrail);
}

void to_json(nlohmann::json& j, Plugin::Settings::Trajectory const& o) {
  cs::core::Settings::serialize(j, "color", o.mColor);
  cs::core::Settings::serialize(j, "drawDot", o.mDrawDot);
  cs::core::Settings::serialize(j, "drawFlare", o.mDrawFlare);
  cs::core::Settings::serialize(j, "trail", o.mTrail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(nlohmann::json const& j, Plugin::Settings& o) {
  cs::core::Settings::deserialize(j, "trajectories", o.mTrajectories);
}

void to_json(nlohmann::json& j, Plugin::Settings const& o) {
  cs::core::Settings::serialize(j, "trajectories", o.mTrajectories);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin::Plugin()
    : mProperties(std::make_shared<Properties>()) {

  // Create default logger for this plugin.
  spdlog::set_default_logger(cs::utils::logger::createLogger("csp-trajectories"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin::init() {

  spdlog::info("Loading plugin...");

  mPluginSettings = mAllSettings->mPlugins.at("csp-trajectories");

  for (auto const& settings : mPluginSettings.mTrajectories) {
    auto anchor = mAllSettings->mAnchors.find(settings.first);

    if (anchor == mAllSettings->mAnchors.end()) {
      throw std::runtime_error(
          "There is no Anchor \"" + settings.first + "\" defined in the settings.");
    }

    auto [tStartExistence, tEndExistence] = anchor->second.getExistence();

    // sun flare ---------------------------------------------------------------
    if (settings.second.mDrawFlare && *settings.second.mDrawFlare) {
      auto flare = std::make_shared<SunFlare>(mGraphicsEngine, mProperties, anchor->second.mCenter,
          anchor->second.mFrame, tStartExistence, tEndExistence);
      mSolarSystem->registerAnchor(flare);

      flare->pColor =
          VistaColor(settings.second.mColor.r, settings.second.mColor.g, settings.second.mColor.b);

      auto sunFlareNode = mSceneGraph->NewOpenGLNode(mSceneGraph->GetRoot(), flare.get());

      VistaOpenSGMaterialTools::SetSortKeyOnSubtree(
          sunFlareNode, static_cast<int>(cs::utils::DrawOrder::eAtmospheres) + 1);

      mSunFlares.push_back(flare);
      mSunFlareNodes.emplace_back(sunFlareNode);
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
      mTrajectories.push_back(trajectory);
      mTrajectoryNodes.emplace_back(trajectoryNode);
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
      mDeepSpaceDotNodes.emplace_back(deepSpaceDotNode);

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

  mGuiManager->getGui()->registerCallback("trajectories.setEnableTrajectories",
      "Enables or disables the rendering of trajectories.",
      std::function([this](bool value) { mProperties->mEnableTrajectories = value; }));

  mGuiManager->getGui()->registerCallback("trajectories.setEnablePlanetMarks",
      "Enables or disables the rendering of points marking the position of the planets.",
      std::function([this](bool value) { mProperties->mEnablePlanetMarks = value; }));

  mGuiManager->getGui()->registerCallback("trajectories.setEnableSunFlare",
      "Enables or disables the rendering of a glare around the sun.",
      std::function([this](bool value) { mProperties->mEnableSunFlares = value; }));

  spdlog::info("Loading done.");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin::deInit() {
  spdlog::info("Unloading plugin...");

  for (auto const& flare : mSunFlares) {
    mSolarSystem->unregisterAnchor(flare);
  }
  for (auto const& flareNode : mSunFlareNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(flareNode.get());
  }

  for (auto const& trajectory : mTrajectories) {
    mSolarSystem->unregisterAnchor(trajectory);
  }
  for (auto const& trajectoryNode : mTrajectoryNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(trajectoryNode.get());
  }

  for (auto const& dot : mDeepSpaceDots) {
    mSolarSystem->unregisterAnchor(dot);
  }
  for (auto const& deepSpaceDotNode : mDeepSpaceDotNodes) {
    mSceneGraph->GetRoot()->DisconnectChild(deepSpaceDotNode.get());
  }

  mGuiManager->removeSettingsSection("Trajectories");

  mGuiManager->getGui()->unregisterCallback("trajectories.setEnableTrajectories");
  mGuiManager->getGui()->unregisterCallback("trajectories.setEnablePlanetMarks");
  mGuiManager->getGui()->unregisterCallback("trajectories.setEnableSunFlare");

  spdlog::info("Unloading done.");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace csp::trajectories
