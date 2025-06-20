#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace stewkk::lab11 {

class IdentTable {
  std::unordered_map<std::string, std::size_t> codes_;
  std::vector<std::string> names_;

 public:
  std::size_t GetCode(const std::string& name);
  std::string At(const std::size_t code) const;
};

}  // namespace stewkk::lab11
