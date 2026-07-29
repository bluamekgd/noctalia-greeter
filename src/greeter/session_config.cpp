#include "greeter/session_config.h"

#include "core/log.h"
#include "greeter/appearance_sync.h"
#include "greeter/greeter_config_store.h"

#include <filesystem>

namespace {

  constexpr Logger kLog("greeter-session");

  [[nodiscard]] GreeterSyncedSession
  sessionFromManifestPayload(const greeter::appearance::ManifestSyncPayload& payload) {
    GreeterSyncedSession session;
    session.power.suspend = payload.sessionPowerSuspend;
    session.power.reboot = payload.sessionPowerReboot;
    session.power.shutdown = payload.sessionPowerShutdown;
    for (const auto& row : payload.sessionActions) {
      session.actions.push_back({row.action, row.command, row.label, row.glyph});
    }
    return session;
  }

  [[nodiscard]] bool hasSessionData(const GreeterSyncedSession& session) {
    return !session.actions.empty()
        || session.power.suspend.has_value()
        || session.power.reboot.has_value()
        || session.power.shutdown.has_value();
  }

  [[nodiscard]] GreeterSyncedSession sessionFromSyncFile(const greeter::config::GreeterSyncFile& sync) {
    GreeterSyncedSession session;
    session.power.suspend = sync.sessionPowerSuspend;
    session.power.reboot = sync.sessionPowerReboot;
    session.power.shutdown = sync.sessionPowerShutdown;
    for (const auto& row : sync.sessionActions) {
      session.actions.push_back({row.action, row.command, row.label, row.glyph});
    }
    return session;
  }

  // Parses the "session" object from an appearance.json-shaped manifest at `path` (staged or
  // legacy live file), independent of sync.toml.
  [[nodiscard]] std::optional<GreeterSyncedSession> parseSyncedSessionManifest(const std::filesystem::path& path) {
    const auto payload = greeter::appearance::parseManifestForSync(path);
    if (!payload.has_value()) {
      return std::nullopt;
    }
    auto session = sessionFromManifestPayload(*payload);
    if (!hasSessionData(session)) {
      return std::nullopt;
    }
    return session;
  }

} // namespace

std::optional<GreeterSyncedSession> loadGreeterSyncedSession() {
  const auto syncPath = greeter::appearance::syncConfPath();
  greeter::config::GreeterSyncFile sync = greeter::config::loadSync(syncPath);
  if (auto fromSync = sessionFromSyncFile(sync); hasSessionData(fromSync)) {
    return fromSync;
  }

  // One-shot migration: legacy Sync-owned appearance.json "session" -> sync.toml.
  const auto legacy = parseSyncedSessionManifest(greeter::appearance::manifestPath());
  if (!legacy.has_value()) {
    return std::nullopt;
  }

  sync.sessionPowerSuspend = legacy->power.suspend;
  sync.sessionPowerReboot = legacy->power.reboot;
  sync.sessionPowerShutdown = legacy->power.shutdown;
  sync.sessionActions.clear();
  for (const auto& action : legacy->actions) {
    sync.sessionActions.push_back({action.action, action.command, action.label, action.glyph});
  }
  if (!greeter::config::writeSync(syncPath, sync)) {
    kLog.warn("failed to migrate legacy session actions into {}", syncPath.string());
  } else {
    kLog.info("migrated legacy session actions into {}", syncPath.string());
  }

  return legacy;
}
