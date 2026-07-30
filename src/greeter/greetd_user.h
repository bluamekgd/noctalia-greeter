#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <sys/types.h>

namespace greeter {

  inline constexpr const char* kGreeterUserEnv = "GREETER_USER";
  inline constexpr const char* kDefaultGreeterUser = "greeter";

  struct GreeterAccountOwnership {
    uid_t uid = 0;
    gid_t gid = 0;
  };

  // Greetd session user for install/setup (GREETER_USER, state dir owner, or greetd config).
  [[nodiscard]] std::optional<std::string> resolveGreeterAccountName();

  // Owner for synced state files: existing dataDir owner when not root, else greetd session user.
  [[nodiscard]] std::optional<GreeterAccountOwnership>
  resolveDataDirOwnership(const std::filesystem::path& dataDir, std::string& errorOut);

} // namespace greeter
