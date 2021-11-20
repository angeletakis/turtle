//
// Created by alex on 11/19/21.
//

#ifndef TURTLE_DOCUMENT_TYPES_HPP
#define TURTLE_DOCUMENT_TYPES_HPP
#include <array>
#include <variant>
#include <any>

namespace turtle {

// In case I want to change it to something bigger in the future
typedef uint64_t turtle_flag_t;

struct node_t;
struct lexeme_t {
  std::variant<std::monostate, std::string, std::string_view, lexeme_t *, node_t *> data;
  uint_fast16_t x = 0, y = 0;
};

struct node_t : public lexeme_t {
  union __attribute__((packed)) {
    turtle_flag_t flag = 0u;
    struct {
      std::array<uint8_t, sizeof(decltype(flag))> flag_byte;
    };
    struct {
      std::array<uint8_t, sizeof(decltype(flag)) - 1> header_padding;
      uint8_t header;
    };
  };
  // prevent compiler from padding child type; instead padding the parent
  union __attribute__((packed)) {
    std::array<node_t *, 2> child = {nullptr, nullptr};
    struct {
      node_t *left, *right;
    };
  };
};

struct document_t {
  std::string filedata;
  // Document Nodes
  std::vector<struct lexeme_t> tokens;
  std::vector<struct node_t> nodes;
  std::vector<std::any> data;
};
} // namespace turtle

#endif // TURTLE_DOCUMENT_TYPES_HPP
