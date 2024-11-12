#pragma once
#include <filesystem>
#include <vector>

struct FrontendContext {
  std::vector<std::filesystem::path> sources;
  std::vector<std::filesystem::path> preprocessed;
  std::vector<std::filesystem::path> objects;
};
