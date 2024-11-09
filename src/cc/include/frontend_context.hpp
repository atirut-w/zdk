#pragma once
#include <filesystem>
#include <vector>

struct FrontendContext {
  std::vector<std::filesystem::path> sources;
};
