#pragma once

#include <optional>
#include <string>
#include <vector>

struct GreeterSyncedSessionAction {
  std::string action;
  std::optional<std::string> command;
  std::optional<std::string> label;
  std::optional<std::string> glyph;
};

struct GreeterSyncedSession {
  struct Power {
    std::optional<std::string> suspend;
    std::optional<std::string> reboot;
    std::optional<std::string> shutdown;
  } power;
  std::vector<GreeterSyncedSessionAction> actions;
};

// Prefers sync.toml [session.power]/[[session.actions]]; falls back to (and migrates into
// sync.toml) the legacy Sync-owned appearance.json "session" object when sync.toml has neither.
[[nodiscard]] std::optional<GreeterSyncedSession> loadGreeterSyncedSession();
