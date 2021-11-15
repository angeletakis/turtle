#ifndef TURTLE_TOKEN_H
#define TURTLE_TOKEN_H

/*
 * This file contains the magic numbers and data structure
 * definitions nessessary to parse Python. Cython does exist,
 * but it does not generate readable code that is a literal
 * translation of a file. With the introduction of c++2a,
 * almost all constructs used in python can be used in C++,
 * it's just the translation needs to be automated.
 * This is more of my try at reinventing the wheel.
 *
 * My motivations to do this to translate Python3 code
 * to C++20 in order to create C++ bash scripts that run fast.
 *
 * Throughout this file I make long comments -- these are not
 * ment for you, but me, as I have a terrible memory.
 *
 * The use of enums are really great as they can be
 * used for pre-calculated magic numbers.
 * What we need to do is take the "magic" out of magic numbers.
 *
 * Currently the the token flag is represented by a 32 bit unsigned integer.
 *
 * Different Types of tokens are separated into classes;
 * Separating tokens into classes makes it possible to compresses
 * the information and give each types of token its unique "fingerprint"
 * or id.
 *
 * Once a specific token has been identified as some sort of predefined or
 * identifier it's token string is deleted from program memory, as
 * it is more efficent to just store the identifying "fingerprint" of a token
 * then a multi character string.
 *      Example:
 *          The '(' left brace token is predefined, give it the numeric id
 *          DELIMITER_CURVED_LEFT_BRACE, delete token string and go to
 *          the next token
 *
 * At the parse tree stage, each deliminar          is a     terminal,
 *                          each identifier or data is a non-terminal
 *
 */

#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>

#include "global.hpp"

//#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/cpp_dec_float.hpp>
#include <any>

namespace turtle {
// In case I want to change it to something bigger in the future
typedef uint64_t turtle_flag_t;

template <class T> using turtle_vector = std::basic_string<T>;

typedef long turtle_int;
typedef turtle_int turtle_float;
typedef std::any turtle_any;
using std::any_cast;

typedef turtle_int t_int;
typedef turtle_float t_float;
typedef turtle_any t_any;
/*struct turtle_num {
    turtle_int sig_digits = 0;
    turtle_int num        = 0;
    constexpr turtle_num(const std::string_view& _n){
        num = static_cast<turtle_int>(_n);
    }
};*/
} // namespace turtle

//      M_turtle_flag(N) (0000'0000 0000'0000 0000'0000 0000'0001 <<  N )
#define M_turtle_flag(N)                                                       \
  (static_cast<turtle::turtle_flag_t>(1u)                                      \
   << static_cast<turtle::turtle_flag_t>(N))

consteval turtle::turtle_flag_t constexpr_ceil(const float num) {
  return (static_cast<float>(static_cast<turtle::turtle_flag_t>(num)) == num)
             ? static_cast<turtle::turtle_flag_t>(num)
             : static_cast<turtle::turtle_flag_t>(num) + ((num > 0) ? 1 : 0);
}

consteval size_t Log2(const size_t n) {
  return ((n < 2) ? 0 : 1 + Log2(n / 2));
}
consteval size_t num_of_bits_required(const size_t max) {
  return constexpr_ceil(Log2(max) + 1);
}
#define bit_range_will_not_overflow(r1, r2)                                    \
  (((r1) + (r2)) < bits_in_turtle_flag)
#define bit_range_will_not_underflow(r1) ((r1) >= 0)
#define CHECK_BIT_RANGE(BITS_NEEDED, MESSAGE)                                  \
  static_assert(bit_range_will_not_overflow(                                   \
                    num_of_bits_required_for_token_type, (BITS_NEEDED)) &&     \
                    bit_range_will_not_underflow(BITS_NEEDED),                 \
                MESSAGE)

#if 0 // def DEBUG_CPP
#include "enum.h"
#define TURTLE_CLASS(ENAME, ...)                                               \
  BETTER_ENUM(ENAME, turtle_flag, __VA_ARGS__, NUMBER_OF_ENUMS)
#else
#define TURTLE_CLASS(ENAME, ...)                                               \
  struct ENAME {                                                               \
    enum : turtle_flag_t { __VA_ARGS__, NUMBER_OF_ENUMS };                     \
  };
#endif

namespace turtle::token {
#define ENUM_NAME Type
TURTLE_CLASS(ENUM_NAME, CONTROL,
             DELIMITERS, // such as '(' or ')' or '.' or '[' or ']' ','
             ARITHMETIC,
             KEYWORD,   // any builtin type
             DATA,      // such as a number or string
             IDENTIFIER // any label
)
#undef ENUM_NAME
} // namespace turtle::token

constexpr auto num_of_bits_required_for_token_type =
    num_of_bits_required(turtle::token::Type::NUMBER_OF_ENUMS);
constexpr auto bits_in_turtle_flag = (sizeof(turtle::turtle_flag_t) * 8);
// make constexpr in order to reduce compile time
constexpr auto tokenTypeOffset =
    (bits_in_turtle_flag - num_of_bits_required_for_token_type);

CHECK_BIT_RANGE(num_of_bits_required(turtle::token::Type::NUMBER_OF_ENUMS),
                "Too many token types");

namespace turtle {
// std::vector<struct Node> SyntaxGroups
//          ||
//          \/
// std::vector<struct Node> SemanticGroups

#if 0//DEBUG_CPP
template <class T> constexpr const char *flagstr(T values, turtle_flag flag) {
  for (const auto &v : values) {
    if (v._to_integral() == flag) {
      return v._to_string();
    }
  }
  return "";
}
#endif

namespace token {

#define ENUM_NAME Control
TURTLE_CLASS(ENUM_NAME, NULL_TOKEN, HAS_VALUE = NULL_TOKEN, NEWLINE, NL,
             ENDMARKER, ERROR, ERRORTOKEN, TokenError, UNSUPPORTED)
#undef ENUM_NAME

#define ENUM_NAME Data
TURTLE_CLASS(ENUM_NAME, DATA_TYPE_STRING,
             DATA_TYPE_RAW, // Used in combination with the string flag
             DATA_TYPE_FORMATTED,
             DATA_TYPE_FORMAT_TYPE, // "%s" printf style or "{}".format style?
             DATA_TYPE_UNICODE,

             DATA_TYPE_NUMBER, DATA_TYPE_INT, DATA_TYPE_FLOAT,
             DATA_TYPE_COMPLEX, DATA_TYPE_EXPONENTIAL, DATA_TYPE_HEX,
             DATA_TYPE_OCTAL, DATA_TYPE_BINARY,
             DATA_TYPE_, // unknown -- make effort to determine type
             DATA_TYPE_COMMENT)
#undef /*cuz its*/ ENUM_NAME // sun

#define ENUM_NAME Operator
TURTLE_CLASS(ENUM_NAME,
             DELIMITER_AT_SIGN, // '@' Python decorator, akin to passing a
                                // function pointer
             DELIMITER_COLON,   // ':' symbol
             DELIMITER_SEMICOLON, DELIMITER_COMMA,
             DELIMITER_PERIOD,   // access token '.'
             DELIMITER_ELLIPSIS, // ...
             DELIMINAR_RARROW, DELIMITER_BRACE,
             // DELIMITER_ACCESS,

             DELIMITER_LEFT_OR_RIGHT_BRACE = DELIMITER_BRACE +
                                             1u, // 0 = Left, 1 = Right
             DELIMITER_CURVED_BRACE,             // '(' or ')'
             DELIMITER_SQUARE_BRACE,             // '[' or ']'
             DELIMITER_CURLY_BRACE,              // '{' or '}'
             DELIMITER_ASSIGN                    // '=' symbol
)
#undef ENUM_NAME

// like identifiers these will be represented as integers
#define ENUM_NAME Keyword
TURTLE_CLASS(ENUM_NAME, KEYWORD_FALSE, KEYWORD_CLASS, KEYWORD_FINALLY,
             KEYWORD_IS, KEYWORD_RETURN, KEYWORD_NONE, KEYWORD_CONTINUE,
             KEYWORD_FOR, KEYWORD_LAMBDA, KEYWORD_TRY, KEYWORD_TRUE,
             KEYWORD_DEF, KEYWORD_FROM, KEYWORD_NONLOCAL, KEYWORD_WHILE,
             KEYWORD_AND, KEYWORD_DEL, KEYWORD_GLOBAL, KEYWORD_NOT,
             KEYWORD_WITH, KEYWORD_AS, KEYWORD_ELIF, KEYWORD_IF, KEYWORD_OR,
             KEYWORD_YIELD, KEYWORD_ASSERT, KEYWORD_ELSE, KEYWORD_IMPORT,
             KEYWORD_PASS, KEYWORD_BREAK, KEYWORD_EXCEPT, KEYWORD_IN,
             KEYWORD_RAISE)
#undef ENUM_NAME

#define ENUM_NAME Arithmetic
TURTLE_CLASS(ENUM_NAME,
             ARITHMETIC_OPERATION = 0u, // 1 = ARITHMETIC, 0 = LOGICAL (greater
                                        // than, less than, equal to, not)

             ARITHMETIC_ADD, ARITHMETIC_SUB, ARITHMETIC_MULL, ARITHMETIC_DIV,
             ARITHMETIC_MOD, ARITHMETIC_FLOOR, ARITHMETIC_EXPONENTIAL,
             ARITHMETIC_BIT_AND, ARITHMETIC_BIT_OR, ARITHMETIC_BIT_XOR,
             ARITHMETIC_BIT_NOT, ARITHMETIC_BIT_LEFT_SHIFT,
             ARITHMETIC_BIT_RIGHT_SHIFT,

             ARITHMETIC_EQUAL_TO = 1, // LOGICAL CLASS
             ARITHMETIC_GREATER_THAN, ARITHMETIC_LESS_THAN,
             ARITHMETIC_NOT //,

             // ARITHMETIC_NOT_EQUAL_TO,

             // ARITHMETIC_GREATER_THAN_EQUAL_TO,

             // ARITHMETIC_LESS_THAN_EQUAL_TO,
)
#undef ENUM_NAME

namespace flag {

/*
 * Below are the precomputed flags
 *
 * This program strives for low memeory use at the cost of speed
 *
 *  TokenFlags
 *
 *    MSB    LSB
 *    |      |
 * #1 000...[Type of Control] - > CONTROL
 * #2 001...[Type of Delim]   - > DELIMITERS
 * #3 010...[Type of Arith]   - > ARITHMETIC
 * #4 011...[Type of Keyword] - > KEYWORD
 * #5 100...[Type of Data]    - > DATA
 * #6 101...[Numeric id]      - > IDENTIFIER
 *
 * The MSB in the token flags segment will be the IDENTIFIER flag.
 * If the token is not an IDENTIFIER, The token
 * types 2-6 will have a numeric id that can be extracted via
 *      (TokenFlag >> ( ( sizeof(TURTLES_FLAG_DATA_TYPE) * 8 ) - 4 ) )
 */

// Refer to the huge comment in the flag namespace on wtf this is & does
#define M_typeFlagMacro(N) (N << tokenTypeOffset)
#define ENUM_NAME Type
TURTLE_CLASS(ENUM_NAME, CONTROL = M_typeFlagMacro(token::ENUM_NAME::CONTROL),
             DELIMITERS = M_typeFlagMacro(token::ENUM_NAME::DELIMITERS),
             ARITHMETIC = M_typeFlagMacro(token::ENUM_NAME::ARITHMETIC),
             KEYWORD = M_typeFlagMacro(token::ENUM_NAME::KEYWORD),
             DATA = M_typeFlagMacro(token::ENUM_NAME::DATA),
             IDENTIFIER = M_typeFlagMacro(token::ENUM_NAME::IDENTIFIER))
#undef ENUM_NAME
/*
 *
 *   ┌──> Flag Type - Control Class Id
 *   │
 * ┌─┤┌───> Is null token
 * 00000000  00000000 00000000 00000000
 *
 *
 *
 *
 *   ┌──> Flag Type - Control Class Id
 *   │
 * ┌─┤┌───> Is not null
 * 00011111  11111111 11111111 11111111
 *     │└─────────────────────────────┴──> Amount Of whitespace
 *     └────> Is newline
 */

#define ENUM_NAME Control
TURTLE_CLASS(ENUM_NAME, NULL_TOKEN = 0u | flag::Type::CONTROL,
             NEWLINE = M_turtle_flag(token::ENUM_NAME::NEWLINE) |
                       M_turtle_flag(token::ENUM_NAME::HAS_VALUE) |
                       flag::Type::CONTROL,
             ENDMARKER = M_turtle_flag(token::ENUM_NAME::ENDMARKER) |
                         M_turtle_flag(token::ENUM_NAME::HAS_VALUE) |
                         flag::Type::CONTROL,
             ERRORTOKEN, TokenError, UNSUPPORTED)
#undef offset
#undef ENUM_NAME

/*
    Any identifier Tokens are represented by a numeric id,
    To get the tokens numeric id perform
        ( flag::IDENTIFIER XOR Node.NodeFlag )


      ┌──> Flag Type - IDENTIFIER Class Id
      │
    ┌─┤
    10100000  00000000 00000000 00000000
       └───────────────────────────────┴──> Numeric Id
*/

/*

   //Strings
      ┌──> Flag Type - DATA Class Id
      │
    ┌─┤
    10000001  00000000 00000000 00000000
       ││││└──> Is string
       │││└───> Is raw string
       ││└────> Is formated string
       │└─────> Format Type
       └──────> Is unicode string
 */
#define ENUM_NAME Data
TURTLE_CLASS(
    ENUM_NAME,
    DATA_TYPE_STRING = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_STRING) |
                       flag::Type::DATA,
    DATA_TYPE_RAW = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_RAW) |
                    flag::Type::DATA,
    DATA_TYPE_FORMATTED = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_FORMATTED) |
                          flag::Type::DATA,
    PRINTF_STYLE_FORMAT =
        M_turtle_flag(token::ENUM_NAME::DATA_TYPE_FORMAT_TYPE) |
        flag::Type::DATA,
    DATA_TYPE_UNICODE = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_UNICODE) |
                        flag::Type::DATA,

    DATA_TYPE_RAW_STRING = (flag::ENUM_NAME::DATA_TYPE_RAW |
                            flag::ENUM_NAME::DATA_TYPE_STRING),
    DATA_TYPE_FORMATED_STRING = (flag::ENUM_NAME::DATA_TYPE_FORMATTED |
                                 flag::ENUM_NAME::DATA_TYPE_STRING),
    DATA_TYPE_FORMATED_PRINTF_STRING =
        (flag::ENUM_NAME::PRINTF_STYLE_FORMAT |
         flag::ENUM_NAME::DATA_TYPE_FORMATED_STRING),
    DATA_TYPE_FORMATED_PYTHON_STRING = DATA_TYPE_FORMATED_STRING,
    DATA_TYPE_UNICODE_STRING = (flag::ENUM_NAME::DATA_TYPE_UNICODE |
                                flag::ENUM_NAME::DATA_TYPE_STRING),
    DATA_TYPE_FORMATED_UNICODE_STRING =
        (flag::ENUM_NAME::DATA_TYPE_FORMATED_STRING |
         flag::ENUM_NAME::DATA_TYPE_UNICODE_STRING),

    DATA_TYPE_COMMENT = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_COMMENT) |
                        flag::Type::DATA,
    DATA_TYPE_NUMBER = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_NUMBER) |
                       flag::Type::DATA,
    DATA_TYPE_INT = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_INT) |
                    flag::ENUM_NAME::DATA_TYPE_NUMBER,
    DATA_TYPE_FLOAT = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_FLOAT) |
                      flag::ENUM_NAME::DATA_TYPE_NUMBER,
    DATA_TYPE_COMPLEX = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_COMPLEX) |
                        flag::ENUM_NAME::DATA_TYPE_NUMBER,
    DATA_TYPE_EXPONENTIAL =
        M_turtle_flag(token::ENUM_NAME::DATA_TYPE_EXPONENTIAL) |
        flag::ENUM_NAME::DATA_TYPE_NUMBER,
    DATA_TYPE_HEX = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_HEX) |
                    flag::ENUM_NAME::DATA_TYPE_INT,
    DATA_TYPE_OCTAL = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_OCTAL) |
                      flag::ENUM_NAME::DATA_TYPE_INT,
    DATA_TYPE_BINARY = M_turtle_flag(token::ENUM_NAME::DATA_TYPE_BINARY) |
                       flag::ENUM_NAME::DATA_TYPE_INT)
#undef ENUM_NAME
#undef DataShiftToMargin

/*
 * The DELIMITER_ASSIGN and DELIMITER_BRACE group is marked by their LSB being
 * set
 *
 *
 *   ┌──> Flag Type - Deliminar Class Id
 *   │                            ┌──> Other Deliminar tokens
 *   │                            │  ┌──> DELIMITER_BRACE token class
 * ┌─┤                          ┌─┴─┐│┌───> DELIMITER_ASSIGN operator class
 * 00100000  00000000 00000000 01111111
 *
 * DELIMITER_ASSIGN operator class:
 * Any Deliminar that has the 1st LSB flag set is a ASSIGN-ment
 *
 * DELIMITER_BRACE token class:
 * Any Deliminar that has the 2nd LSB flag set is a brace
 */

// The below may look cryptic,
// But heres whats happening:
//
// Shift an arithmetic integer to go next to the DELIMITER_ASSIGN flag
// (DELIMITER_ASSIGN + 1 )
//
// Then give it the DELIMITER_ASSIGN flag

#define ENUM_NAME Operator
// offset the arithmetic tokens to be next to the DELIMITER_ASSIGN token
#define DeliminarAssignOffset_M(x)                                             \
  (x << (token::ENUM_NAME::DELIMITER_ASSIGN + 1))
TURTLE_CLASS(
    ENUM_NAME,
    DELIMITER_ASSIGN = M_turtle_flag(token::ENUM_NAME::DELIMITER_ASSIGN) |
                       flag::Type::DELIMITERS,

    ARITHMETIC_ADD_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_ADD) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_SUB_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_SUB) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_MULL_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_MULL) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_DIV_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_DIV) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_MOD_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_MOD) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_FLOOR_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_FLOOR) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_EXPONENTIAL_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_EXPONENTIAL) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_BIT_AND_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_AND) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_BIT_OR_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_OR) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_BIT_XOR_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_XOR) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_LEFT_SHIFT) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,
    ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN =
        DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_RIGHT_SHIFT) |
        flag::ENUM_NAME::DELIMITER_ASSIGN,

    DELIMITER_AT_SIGN = M_turtle_flag(token::ENUM_NAME::DELIMITER_AT_SIGN) |
                        flag::Type::DELIMITERS,
    //@=
    ARITHMETIC_AT_ASSIGN = flag::ENUM_NAME::DELIMITER_AT_SIGN |
                           flag::ENUM_NAME::DELIMITER_ASSIGN,
    DELIMINAR_RARROW = M_turtle_flag(token::ENUM_NAME::DELIMINAR_RARROW) |
                       flag::Type::DELIMITERS,
    DELIMITER_COLON = M_turtle_flag(token::ENUM_NAME::DELIMITER_COLON) |
                      flag::Type::DELIMITERS,
    DELIMITER_SEMICOLON = M_turtle_flag(token::ENUM_NAME::DELIMITER_SEMICOLON) |
                          flag::Type::DELIMITERS,
    DELIMITER_COMMA = M_turtle_flag(token::ENUM_NAME::DELIMITER_COMMA) |
                      flag::Type::DELIMITERS,
    DELIMITER_PERIOD = M_turtle_flag(token::ENUM_NAME::DELIMITER_PERIOD) |
                       flag::Type::DELIMITERS,
    DELIMITER_ELLIPSIS = M_turtle_flag(token::ENUM_NAME::DELIMITER_ELLIPSIS) |
                         flag::Type::DELIMITERS,
    DELIMITER_ACCESS = DELIMITER_PERIOD,

    DELIMITER_BRACE = M_turtle_flag(token::ENUM_NAME::DELIMITER_BRACE) |
                      flag::Type::DELIMITERS,
    DELIMITER_LEFT_BRACE =
        M_turtle_flag(token::ENUM_NAME::DELIMITER_LEFT_OR_RIGHT_BRACE) |
        flag::Type::DELIMITERS, // 1 = Left, 0 = Right

    DELIMITER_CURVED_BRACE =
        M_turtle_flag(token::ENUM_NAME::DELIMITER_CURVED_BRACE) |
        flag::Type::DELIMITERS,
    DELIMITER_SQUARE_BRACE =
        M_turtle_flag(token::ENUM_NAME::DELIMITER_SQUARE_BRACE) |
        flag::Type::DELIMITERS,
    DELIMITER_CURLY_BRACE =
        M_turtle_flag(token::ENUM_NAME::DELIMITER_CURLY_BRACE) |
        flag::Type::DELIMITERS,

    DELIMITER_CURVED_RIGHT_BRACE = (DELIMITER_CURVED_BRACE | DELIMITER_BRACE),
    DELIMITER_CURVED_LEFT_BRACE = (DELIMITER_CURVED_BRACE |
                                   DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_RIGHT_BRACE = (DELIMITER_SQUARE_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_LEFT_BRACE = (DELIMITER_SQUARE_BRACE |
                                   DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_RIGHT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_LEFT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_LEFT_BRACE |
                                  DELIMITER_BRACE))

#undef DeliminarAssignOffset_M
#undef ENUM_NAME

#define ENUM_NAME Keyword
TURTLE_CLASS(
    ENUM_NAME,
    KEYWORD_FALSE = token::ENUM_NAME::KEYWORD_FALSE | flag::Type::KEYWORD,
    KEYWORD_CLASS = token::ENUM_NAME::KEYWORD_CLASS | flag::Type::KEYWORD,
    KEYWORD_FINALLY = token::ENUM_NAME::KEYWORD_FINALLY | flag::Type::KEYWORD,
    KEYWORD_IS = token::ENUM_NAME::KEYWORD_IS | flag::Type::KEYWORD,
    KEYWORD_RETURN = token::ENUM_NAME::KEYWORD_RETURN | flag::Type::KEYWORD,
    KEYWORD_NONE = token::ENUM_NAME::KEYWORD_NONE | flag::Type::KEYWORD,
    KEYWORD_CONTINUE = token::ENUM_NAME::KEYWORD_CONTINUE | flag::Type::KEYWORD,
    KEYWORD_FOR = token::ENUM_NAME::KEYWORD_FOR | flag::Type::KEYWORD,
    KEYWORD_LAMBDA = token::ENUM_NAME::KEYWORD_LAMBDA | flag::Type::KEYWORD,
    KEYWORD_TRY = token::ENUM_NAME::KEYWORD_TRY | flag::Type::KEYWORD,
    KEYWORD_TRUE = token::ENUM_NAME::KEYWORD_TRUE | flag::Type::KEYWORD,
    KEYWORD_DEF = token::ENUM_NAME::KEYWORD_DEF | flag::Type::KEYWORD,
    KEYWORD_FROM = token::ENUM_NAME::KEYWORD_FROM | flag::Type::KEYWORD,
    KEYWORD_NONLOCAL = token::ENUM_NAME::KEYWORD_NONLOCAL | flag::Type::KEYWORD,
    KEYWORD_WHILE = token::ENUM_NAME::KEYWORD_WHILE | flag::Type::KEYWORD,
    KEYWORD_AND = token::ENUM_NAME::KEYWORD_AND | flag::Type::KEYWORD,
    KEYWORD_DEL = token::ENUM_NAME::KEYWORD_DEL | flag::Type::KEYWORD,
    KEYWORD_GLOBAL = token::ENUM_NAME::KEYWORD_GLOBAL | flag::Type::KEYWORD,
    KEYWORD_NOT = token::ENUM_NAME::KEYWORD_NOT | flag::Type::KEYWORD,
    KEYWORD_WITH = token::ENUM_NAME::KEYWORD_WITH | flag::Type::KEYWORD,
    KEYWORD_AS = token::ENUM_NAME::KEYWORD_AS | flag::Type::KEYWORD,
    KEYWORD_ELIF = token::ENUM_NAME::KEYWORD_ELIF | flag::Type::KEYWORD,
    KEYWORD_IF = token::ENUM_NAME::KEYWORD_IF | flag::Type::KEYWORD,
    KEYWORD_OR = token::ENUM_NAME::KEYWORD_OR | flag::Type::KEYWORD,
    KEYWORD_YIELD = token::ENUM_NAME::KEYWORD_YIELD | flag::Type::KEYWORD,
    KEYWORD_ASSERT = token::ENUM_NAME::KEYWORD_ASSERT | flag::Type::KEYWORD,
    KEYWORD_ELSE = token::ENUM_NAME::KEYWORD_ELSE | flag::Type::KEYWORD,
    KEYWORD_IMPORT = token::ENUM_NAME::KEYWORD_IMPORT | flag::Type::KEYWORD,
    KEYWORD_PASS = token::ENUM_NAME::KEYWORD_PASS | flag::Type::KEYWORD,
    KEYWORD_BREAK = token::ENUM_NAME::KEYWORD_BREAK | flag::Type::KEYWORD,
    KEYWORD_EXCEPT = token::ENUM_NAME::KEYWORD_EXCEPT | flag::Type::KEYWORD,
    KEYWORD_IN = token::ENUM_NAME::KEYWORD_IN | flag::Type::KEYWORD,
    KEYWORD_RAISE = token::ENUM_NAME::KEYWORD_RAISE | flag::Type::KEYWORD)
#undef ENUM_NAME

/*
 * The '=' token is classified as a deliminar;
 * It is NOT an ARITHMETIC token
 */

/*
 * The type of ARITHMETIC_OPERATION is set by the LSB
 * When the LSB is 1, it is of the ARITHMETIC_OPERATION class
 * When the LSB is 0, it is of the LOGICAL_OPERATION class
 *
 *   ┌─ Flag Type - ARITHMETIC Class Id
 * ┌─┤                               ┌───> ARITHMETIC_OPERATION class
 * 01000000 00000000 00000000 00000001
 *
 *
 *   ┌─ Flag Type - ARITHMETIC Class Id
 * ┌─┤                               ┌───> LOGICAL_OPERATION class
 * 01000000 00000000 00000000 00000000
 */
#define ENUM_NAME Arithmetic
TURTLE_CLASS(
    ENUM_NAME,
    ARITHMETIC_OPERATION =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_OPERATION) |
        flag::Type::ARITHMETIC,

    ARITHMETIC_ADD = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_ADD) |
                     flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_SUB = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_SUB) |
                     flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_MULL = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_MULL) |
                      flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_DIV = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_DIV) |
                     flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_MOD = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_MOD) |
                     flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_FLOOR = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_FLOOR) |
                       flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_EXPONENTIAL =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_EXPONENTIAL) |
        flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_AND = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_AND) |
                         flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_OR = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_OR) |
                        flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_XOR = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_XOR) |
                         flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_NOT = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_NOT) |
                         flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_LEFT_SHIFT =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_LEFT_SHIFT) |
        flag::ENUM_NAME::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_RIGHT_SHIFT =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_BIT_RIGHT_SHIFT) |
        flag::ENUM_NAME::ARITHMETIC_OPERATION,

    ARITHMETIC_EQUAL_TO = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_EQUAL_TO) |
                          flag::Type::ARITHMETIC,

    ARITHMETIC_NOT_EQUAL = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_NOT) |
                           flag::ENUM_NAME::ARITHMETIC_EQUAL_TO,

    ARITHMETIC_GREATER_THAN =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_GREATER_THAN) |
        flag::Type::ARITHMETIC,

    ARITHMETIC_LESS_THAN =
        M_turtle_flag(token::ENUM_NAME::ARITHMETIC_LESS_THAN) |
        flag::Type::ARITHMETIC,

    ARITHMETIC_NOT = M_turtle_flag(token::ENUM_NAME::ARITHMETIC_NOT) |
                     flag::Type::ARITHMETIC,

    ARITHMETIC_GREATER_THAN_EQUAL_TO =
        (flag::ENUM_NAME::ARITHMETIC_EQUAL_TO |
         flag::ENUM_NAME::ARITHMETIC_GREATER_THAN),

    ARITHMETIC_LESS_THAN_EQUAL_TO = (flag::ENUM_NAME::ARITHMETIC_EQUAL_TO |
                                     flag::ENUM_NAME::ARITHMETIC_LESS_THAN))
#undef ENUM_NAME
} // namespace flag

} // namespace token

// convert 8 byte string to 64 bit intager
consteval uint_fast64_t sti(const char *str) {
  uint_fast64_t res = 0;
  for (uint_fast8_t i = 0; str[i]; ++i) {
    res <<= 8;
    res |= static_cast<uint_fast64_t>(str[i]);
  }
  return res;
}

/*
 * Unfortunately std::map can not be constexpr-ed
 * Although memory is cheap, the less used the better
 * And it is my opinion to allocations on the stack make
 * for a safer garentee that the program will run effectively,
 * efficently, and fast
 */
constexpr uint_fast64_t turtleBuiltinTokenMap[][2] = {
    {sti(","), token::flag::Operator::DELIMITER_COMMA},
    {sti(";"), token::flag::Operator::DELIMITER_SEMICOLON},
    {sti(":"), token::flag::Operator::DELIMITER_COLON},
    {sti("()"), token::flag::Operator::DELIMITER_CURLY_RIGHT_BRACE},
    {sti("("), token::flag::Operator::DELIMITER_CURVED_LEFT_BRACE},
    {sti(")"), token::flag::Operator::DELIMITER_CURVED_RIGHT_BRACE},
    {sti("{}"), token::flag::Operator::DELIMITER_CURLY_RIGHT_BRACE},
    {sti("{"), token::flag::Operator::DELIMITER_CURLY_LEFT_BRACE},
    {sti("}"), token::flag::Operator::DELIMITER_CURLY_RIGHT_BRACE},
    {sti("[]"), token::flag::Operator::DELIMITER_CURLY_RIGHT_BRACE},
    {sti("["), token::flag::Operator::DELIMITER_SQUARE_LEFT_BRACE},
    {sti("]"), token::flag::Operator::DELIMITER_SQUARE_RIGHT_BRACE},
    {sti("..."), token::flag::Operator::DELIMITER_ELLIPSIS},
    {sti("."), token::flag::Operator::DELIMITER_PERIOD},
    {sti("="), token::flag::Operator::DELIMITER_ASSIGN},
    {sti("->"), token::flag::Operator::DELIMINAR_RARROW},
    {sti(":="), token::flag::Control::UNSUPPORTED}, // NOTE -- walrus
    {sti("@"), token::flag::Operator::DELIMITER_AT_SIGN},
    {sti("@="), token::flag::Operator::ARITHMETIC_AT_ASSIGN},
    {sti("+"), token::flag::Arithmetic::ARITHMETIC_ADD},
    {sti("-"), token::flag::Arithmetic::ARITHMETIC_SUB},
    {sti("*"), token::flag::Arithmetic::ARITHMETIC_MULL},
    {sti("/"), token::flag::Arithmetic::ARITHMETIC_DIV},
    {sti("%"), token::flag::Arithmetic::ARITHMETIC_MOD},
    {sti(">"), token::flag::Arithmetic::ARITHMETIC_GREATER_THAN},
    {sti("<"), token::flag::Arithmetic::ARITHMETIC_LESS_THAN},
    {sti("&"), token::flag::Arithmetic::ARITHMETIC_BIT_AND},
    {sti("|"), token::flag::Arithmetic::ARITHMETIC_BIT_OR},
    {sti("^"), token::flag::Arithmetic::ARITHMETIC_BIT_XOR},
    {sti("~"), token::flag::Arithmetic::ARITHMETIC_BIT_NOT},
    {sti("!"), token::flag::Arithmetic::ARITHMETIC_NOT},
    {sti("=="), token::flag::Arithmetic::ARITHMETIC_EQUAL_TO},
    {sti("!="), token::flag::Arithmetic::ARITHMETIC_NOT_EQUAL},
    {sti("//"), token::flag::Arithmetic::ARITHMETIC_FLOOR},
    {sti("**"), token::flag::Arithmetic::ARITHMETIC_EXPONENTIAL},
    {sti("<<"), token::flag::Arithmetic::ARITHMETIC_BIT_LEFT_SHIFT},
    {sti(">>"), token::flag::Arithmetic::ARITHMETIC_BIT_LEFT_SHIFT},
    {sti("+="), token::flag::Operator::ARITHMETIC_ADD_ASSIGN},
    {sti("-="), token::flag::Operator::ARITHMETIC_SUB_ASSIGN},
    {sti("*="), token::flag::Operator::ARITHMETIC_MULL_ASSIGN},
    {sti("/="), token::flag::Operator::ARITHMETIC_DIV_ASSIGN},
    {sti("%="), token::flag::Operator::ARITHMETIC_MOD_ASSIGN},
    {sti(">="), token::flag::Arithmetic::ARITHMETIC_GREATER_THAN_EQUAL_TO},
    {sti("<="), token::flag::Arithmetic::ARITHMETIC_LESS_THAN_EQUAL_TO},
    {sti("//="), token::flag::Operator::ARITHMETIC_FLOOR_ASSIGN},
    {sti("**="), token::flag::Operator::ARITHMETIC_EXPONENTIAL_ASSIGN},
    {sti("&="), token::flag::Operator::ARITHMETIC_BIT_AND_ASSIGN},
    {sti("|="), token::flag::Operator::ARITHMETIC_BIT_OR_ASSIGN},
    {sti("^="), token::flag::Operator::ARITHMETIC_BIT_XOR_ASSIGN},

    //"~=" operator does not exist

    {sti("<<="), token::flag::Operator::ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN},
    {sti(">>="), token::flag::Operator::ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN},
    {sti("async"), token::flag::Control::UNSUPPORTED},
    {sti("await"), token::flag::Control::UNSUPPORTED},
    {sti("False"), token::flag::Keyword::KEYWORD_FALSE},
    {sti("True"), token::flag::Keyword::KEYWORD_TRUE},
    {sti("class"), token::flag::Keyword::KEYWORD_CLASS},
    {sti("finally"), token::flag::Keyword::KEYWORD_FINALLY},
    {sti("is"), token::flag::Keyword::KEYWORD_IS},
    {sti("return"), token::flag::Keyword::KEYWORD_RETURN},
    {sti("None"), token::flag::Keyword::KEYWORD_NONE},
    {sti("continue"), token::flag::Keyword::KEYWORD_CONTINUE},
    {sti("for"), token::flag::Keyword::KEYWORD_FOR},
    {sti("lambda"), token::flag::Keyword::KEYWORD_LAMBDA},
    {sti("try"), token::flag::Keyword::KEYWORD_TRY},
    {sti("def"), token::flag::Keyword::KEYWORD_DEF},
    {sti("from"), token::flag::Keyword::KEYWORD_FROM},
    {sti("nonlocal"), token::flag::Keyword::KEYWORD_NONLOCAL},
    {sti("while"), token::flag::Keyword::KEYWORD_WHILE},
    {sti("and"), token::flag::Keyword::KEYWORD_AND},
    {sti("del"), token::flag::Keyword::KEYWORD_DEL},
    {sti("global"), token::flag::Keyword::KEYWORD_GLOBAL},
    {sti("not"), token::flag::Keyword::KEYWORD_NOT},
    {sti("with"), token::flag::Keyword::KEYWORD_WITH},
    {sti("as"), token::flag::Keyword::KEYWORD_AS},
    {sti("elif"), token::flag::Keyword::KEYWORD_ELIF},
    {sti("if"), token::flag::Keyword::KEYWORD_IF},
    {sti("or"), token::flag::Keyword::KEYWORD_OR},
    {sti("yield"), token::flag::Keyword::KEYWORD_YIELD},
    {sti("assert"), token::flag::Keyword::KEYWORD_ASSERT},
    {sti("else"), token::flag::Keyword::KEYWORD_ELSE},
    {sti("import"), token::flag::Keyword::KEYWORD_IMPORT},
    {sti("pass"), token::flag::Keyword::KEYWORD_PASS},
    {sti("break"), token::flag::Keyword::KEYWORD_BREAK},
    {sti("except"), token::flag::Keyword::KEYWORD_EXCEPT},
    {sti("in"), token::flag::Keyword::KEYWORD_IN},
    {sti("raise"), token::flag::Keyword::KEYWORD_RAISE}};

struct lexeme_t {
  std::string_view str;
  uint_fast16_t x = 0, y = 0;
};
struct node_t {
  turtle::turtle_flag_t flag = 0u;
  turtle::lexeme_t token;
  union {
    node_t *child[2] = {nullptr, nullptr};
    struct {
      node_t *left;
      node_t *right;
    };
  };
  /*
  [[nodiscard]] constexpr turtle::turtle_flag_t type() const;
  [[nodiscard]] constexpr turtle::turtle_flag_t type_of(
      turtle::turtle_flag_t _f);
  [[nodiscard]] constexpr turtle::turtle_flag_t test_bit(unsigned char i) const;
  [[nodiscard]] constexpr turtle::turtle_flag_t hasType(
      turtle::turtle_flag_t _f) const;
  [[nodiscard]] constexpr turtle::turtle_flag_t hasFlag(
      const turtle::turtle_flag_t _f);*/
};

struct document_t {
  // Document Nodes
  std::vector<struct lexeme_t> lexemes;
  std::vector<struct node_t> nodes;
  std::vector<std::any> data;
};
} // namespace turtle

#endif // TURTLE_TOKEN_H
