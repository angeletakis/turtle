//
// Created by alex on 11/15/21.
//

#include "document_types.hpp"
#include "global.hpp"
#include "node.hpp"

using namespace turtle;
using namespace turtle::token::flag;
#include <bitset>

void tokenize(std::vector<lexeme_t> &tokens) {
  constexpr static turtle_flag_t tmap[std::numeric_limits<char>::max()] = {
      ['\t'] = ~static_cast<turtle_flag_t>(0u),
      [' '] = ~static_cast<turtle_flag_t>(0u),
      ['#'] = ~static_cast<turtle_flag_t>(0u),
      ['\n'] = ~static_cast<turtle_flag_t>(0u),
      ['\r'] =  ~static_cast<turtle_flag_t>(0u),
      ['\''] = Data::DATA_TYPE_STRING,
      ['"'] = Data::DATA_TYPE_STRING,
      ['0' ... '9'] = token::flag::Data::DATA_TYPE_NUMBER,
      [','] = Delimiter::DELIMITER_COMMA,
      [':'] = Delimiter::DELIMITER_COLON,
      ['{'] = Delimiter::DELIMITER_CURLY_LEFT_BRACE,
      ['}'] = Delimiter::DELIMITER_CURLY_RIGHT_BRACE,
      ['['] = Delimiter::DELIMITER_SQUARE_LEFT_BRACE,
      [']'] = Delimiter::DELIMITER_SQUARE_RIGHT_BRACE,
      ['('] = Delimiter::DELIMITER_CURVED_LEFT_BRACE,
      [')'] = Delimiter::DELIMITER_CURVED_RIGHT_BRACE,
      [';'] = Delimiter::DELIMITER_SEMICOLON};
  turtle_flag_t current_flag = 0u;
  for (auto &current_token : tokens) {
    const auto &sv = std::get<std::string>(current_token.data);
    current_flag = tmap[sv[0]];
    // ignore whitespace
    if (current_flag != Control::NULL_TOKEN || current_flag == ~static_cast<turtle_flag_t>(0u)) {
      goto TOKEN_FOUND;
    }
    if (sv.length() > sizeof(decltype(turtleBuiltinTokenMap[0][0]))) {
      goto TOKEN_FOUND;
    }
    for (const auto &token_builtin : turtleBuiltinTokenMap) {
      const auto& i = sti(sv.data());
      if (token_builtin[0] == i) {
        current_flag = token_builtin[1];
        goto TOKEN_FOUND;
      }
    }
  TOKEN_FOUND:
    if (current_flag == 0u) {
      current_flag = token::flag::Type::IDENTIFIER;
    }
    LOG(info) << std::bitset<sizeof(turtle_flag_t)*8>(current_flag);
    current_flag = 0u;
  }
}
