#include "output.h"

namespace stewkk::lab11 {

std::string Output::GetStr() const {
    return out_.str();
}

void Output::NewLine() {
    out_ << line_.view() << '\n';
    ClearLine();
    line_ << ident_.view();
}

void Output::ClearLine() {
    line_ = std::ostringstream{};
}

void Output::IncreaseIdent() {
    ident_ << "  ";
}

void Output::ResetIdent() {
    ident_ = std::ostringstream{};
}

void Output::Put(const std::string& str) {
    line_ << str;
}

std::size_t Output::GetPrefixLength() const {
    return line_.view().size();
}

void Output::CheckpointLine() {
    checkpoint_ = std::ostringstream{line_.str()};
}

void Output::RestoreCheckpoint() {
    line_ = std::move(checkpoint_);
}

std::string Output::GetIdent() {
    return ident_.str();
}

void Output::SetIdent(std::string ident) {
    ident_ = std::ostringstream{std::move(ident)};
}

}  // namespace stewkk::lab11
