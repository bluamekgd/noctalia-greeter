#include "greeter/privileged_state_paths.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace greeter::privileged_state {

  namespace {

    [[nodiscard]] bool lstatPath(const std::filesystem::path& path, struct stat& out, std::string& errorOut) {
      if (::lstat(path.c_str(), &out) != 0) {
        errorOut = std::string("lstat failed for '") + path.string() + "': " + std::strerror(errno);
        return false;
      }
      return true;
    }

    [[nodiscard]] bool refuseSymlink(const std::filesystem::path& path, const struct stat& st, std::string& errorOut) {
      if (S_ISLNK(st.st_mode)) {
        errorOut = std::string("refusing to operate on symlink '") + path.string() + "'";
        return false;
      }
      return true;
    }

  } // namespace

  bool removeSymlinkIfPresent(const std::filesystem::path& path, std::string& errorOut) {
    struct stat st{};
    if (::lstat(path.c_str(), &st) != 0) {
      if (errno == ENOENT) {
        return true;
      }
      errorOut = std::string("lstat failed for '") + path.string() + "': " + std::strerror(errno);
      return false;
    }
    if (!S_ISLNK(st.st_mode)) {
      return true;
    }
    if (::unlink(path.c_str()) != 0) {
      errorOut = std::string("failed to remove symlink '") + path.string() + "': " + std::strerror(errno);
      return false;
    }
    return true;
  }

  bool setMode(const std::filesystem::path& path, const mode_t mode, std::string& errorOut) {
    struct stat st{};
    if (!lstatPath(path, st, errorOut)) {
      return false;
    }
    if (!refuseSymlink(path, st, errorOut)) {
      return false;
    }
    if (::fchmodat(AT_FDCWD, path.c_str(), mode, AT_SYMLINK_NOFOLLOW) != 0) {
      errorOut = std::string("chmod failed for '") + path.string() + "': " + std::strerror(errno);
      return false;
    }
    return true;
  }

  bool setOwnership(const std::filesystem::path& path, const uid_t uid, const gid_t gid, std::string& errorOut) {
    struct stat st{};
    if (!lstatPath(path, st, errorOut)) {
      return false;
    }
    if (!refuseSymlink(path, st, errorOut)) {
      return false;
    }
    if (::fchownat(AT_FDCWD, path.c_str(), uid, gid, AT_SYMLINK_NOFOLLOW) != 0) {
      errorOut = std::string("chown failed for '") + path.string() + "': " + std::strerror(errno);
      return false;
    }
    return true;
  }

} // namespace greeter::privileged_state
