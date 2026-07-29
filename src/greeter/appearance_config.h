#pragma once

#include "config/config_types.h"
#include "ui/palette.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

struct Color;

struct GreeterOutputWallpaper {
  std::string path;
  WallpaperFillMode fillMode = WallpaperFillMode::Crop;
  Color fillColor = rgba(0.0f, 0.0f, 0.0f, 0.0f);
};

struct GreeterSyncedAppearance {
  Palette palette{};
  std::string themeMode;
  std::string wallpaperPath;
  WallpaperFillMode wallpaperFillMode = WallpaperFillMode::Crop;
  Color wallpaperFillColor = rgba(0.0f, 0.0f, 0.0f, 0.0f);
  // connector name -> wallpaper (from appearance.json "wallpapers")
  std::unordered_map<std::string, GreeterOutputWallpaper> wallpapersByOutput;
  float cornerRadiusScale = 1.0f;
  // From appearance.json "font_family"; empty means leave the greeter default.
  std::string fontFamily;

  [[nodiscard]] GreeterOutputWallpaper wallpaperForOutput(std::string_view outputName) const {
    if (!outputName.empty()) {
      const auto it = wallpapersByOutput.find(std::string(outputName));
      if (it != wallpapersByOutput.end() && !it->second.path.empty()) {
        return it->second;
      }
    }
    GreeterOutputWallpaper fallback;
    fallback.path = wallpaperPath;
    fallback.fillMode = wallpaperFillMode;
    fallback.fillColor = wallpaperFillColor;
    return fallback;
  }
};

// Legacy Sync appearance.json path; used only for the one-shot migration into sync.toml.
[[nodiscard]] std::filesystem::path greeterAppearanceConfigPath();

// Synced scheme source, in order: greeter.toml embedded appearance (if its palette is
// complete), else sync.toml [appearance] (Sync-owned, if its palette is complete), else the
// legacy live appearance.json — migrated into sync.toml once when found.
[[nodiscard]] std::optional<GreeterSyncedAppearance> loadGreeterSyncedAppearance();
