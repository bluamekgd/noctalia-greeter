#pragma once

#include <filesystem>
#include <string>
#include <sys/types.h>

namespace greeter::privileged_state {

  // Remove a symlink at `path` when running as root before trusted writes or metadata updates.
  [[nodiscard]] bool removeSymlinkIfPresent(const std::filesystem::path& path, std::string& errorOut);

  // Refuse symlinks; use fchmodat/fchownat with AT_SYMLINK_NOFOLLOW.
  [[nodiscard]] bool setMode(const std::filesystem::path& path, mode_t mode, std::string& errorOut);
  [[nodiscard]] bool setOwnership(const std::filesystem::path& path, uid_t uid, gid_t gid, std::string& errorOut);

} // namespace greeter::privileged_state
