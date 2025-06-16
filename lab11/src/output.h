#pragma once

#include <string>
#include <sstream>

namespace stewkk::lab11 {

class Output {
  public:
    std::string GetStr() const;

    void NewLine();
    void ClearLine();
    void IncreaseIdent();
    void ResetIdent();
    std::string GetIdent();
    void SetIdent(std::string ident);
    void Put(const std::string& str);
    std::size_t GetPrefixLength() const;
    void CheckpointLine();
    void RestoreCheckpoint();

  private:

    std::ostringstream ident_;
    std::ostringstream line_;
    std::ostringstream checkpoint_;
    std::ostringstream out_;
};

}  // namespace stewkk::lab11
