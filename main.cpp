#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
using std::cerr;
using std::cout;

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  END_OF_FILE
};

std::ostream &operator<<(std::ostream &o, TokenType tok) {
  if (tok == TokenType::VAR) {
    o << "VAR";
  } else if (tok == TokenType::EQUAL) {
    o << "EQUAL";
  } else if (tok == TokenType::STRING) {
    o << "STRING";
  } else if (tok == TokenType::IDENTIFIER) {
    o << "IDENTIFIER";
  } else if (tok == TokenType::NUMBER) {
    o << "NUMBER";
  }else if (tok == TokenType::EQUAL_EQUAL) {
    o << "==";
  } else if (tok == TokenType::SEMICOLON) {
    o << "SEMICOLON";
  } else if (tok == TokenType::END_OF_FILE) {
    o << "END_OF_FILE";
  } else {
    o << "Token type " << tok << " is unregisted with ostream overload\n";
  }

  return o;
}

struct Token {
public:
  const TokenType type;
  const std::string lexeme;
  //  const size_t literal;
  const size_t line;
  Token(const TokenType type, const size_t line, std::string lexeme)
      : type(type), lexeme(std::move(lexeme)), line(line) {}
};


bool is_integer_literal(const std::string_view &s) {
  // TODO also allow float literals. All numerics in Lox are floats
  return std::all_of(s.cbegin(), s.cend(),
                     [](char c) { return std::isdigit(c); });
}

bool is_string_literal(const std::string_view &s) {
  if (s.empty())
    return false;

  const char first_char = s[0];
  const char last_char = s[s.size()];
  if (first_char == '"' && last_char == '"') {
    return true;
  }

  if (first_char == '\'' && last_char == '\'') {
    return true;
  }
  return false;
}

std::string strip(const std::string_view &s) {

  size_t start = 0;
  for (; start < s.size(); start++) {
    if (!std::isspace(s[start]))
      break;
  }

  size_t end;
  for (end = s.size(); end > 0; end--) {
    if (!std::isspace(s[end]))
      break;
  }

  return std::string(s.substr(start, end));
}

bool is_valid_identifier(const std::string_view &s) {
  // TODO proper alphanumeric or "_" validation
  return s != std::string("fooobasdyuasdgy7uasgdyuas");
}

class Scanner {
public:
  static TokenType identify_lexeme(const std::string_view &token) {

    if (token == "var") {
      return TokenType::VAR;
    } else if (token == "=") {
      return TokenType::EQUAL;
    } else if (token == ";") {
      return TokenType::SEMICOLON;
    } else if (token == "==") {
      return TokenType::EQUAL_EQUAL;
    }
    // Literal checks
    else if (is_integer_literal(token)) {
      return TokenType::NUMBER;
    } else if (is_string_literal(token)) {
      return TokenType::STRING;
    } else if (is_valid_identifier(token)) {
      return TokenType::IDENTIFIER;
    } else {
      cerr << "Unknown token: \"" << token << "\"" << std::endl;
      exit(1);
    }
  }

  std::vector<Token> tokenise(const std::string_view &s) {
    bool in_quote_str = false;  // 'foo'
    bool in_speech_str = false; // "foo"

    std::vector<Token> tokens;

    size_t slashes_in_a_row = 0;
    size_t last_lexme_end = 0;
    char c;
    for (size_t i = 0; i < s.size(); i++) {
      c = s[i];

      if (c == '\\') {
        slashes_in_a_row++;
      } else
        slashes_in_a_row = 0;

      // Any odd number of \ before the current char means we are escaped such
      // as "\\\\\t" Any even number is either no slashes before or the previous
      // slashes being escaped like
      // "\\\\t" "foo\\\\t" -> false
      // "foo\\\t" -> true
      const bool current_char_is_escaped = slashes_in_a_row % 2 != 0;

      if (c == '\'' && !in_speech_str && !current_char_is_escaped) {
        in_quote_str = !in_quote_str;
      } else if (c == '"' && !in_quote_str && !current_char_is_escaped) {
        in_speech_str = !in_speech_str;
      }

      const bool in_string = in_speech_str || in_quote_str;
      if ((std::isspace(c) || c == ';') && !in_string) {
        std::string token =
            std::string(s.substr(last_lexme_end, i - last_lexme_end));

        last_lexme_end = i;

        TokenType type = identify_lexeme(strip(token));

        const auto t = Token(type, 0, std::move(token));
        tokens.emplace_back(t);

        // TODO this is a hacky solution to ; not being space separated
        // Need to fix this for things like (5+5) as well.
        if (c == ';') {
          tokens.emplace_back(TokenType::SEMICOLON, 0, std::string(";"));
        }
        continue;
      }
    }

    if (last_lexme_end != s.size() - 1) {
      cerr << "Error during lexing. Trailing characters after last consumed "
              "token in statement. \""
           << s.substr(last_lexme_end) << "\"" << std::endl;
      exit(1);
    }

    auto EOF_TOKEN = Token(TokenType::END_OF_FILE, 0, "");
    tokens.emplace_back(EOF_TOKEN);

    return tokens;
  }
};

int main() {
  auto scanner = Scanner();
  //  scanner.lexise("var x = 5;");

  std::vector<Token> tokens{};
//  tokens = scanner.tokenise(R"(var x = "foo\";)");
//  tokens =   scanner.tokenise(R"(var x = 'foo\'';)");
  tokens = scanner.tokenise("x == 5;");
  for (auto t : tokens) {
    cout << t.type << ":" << t.lexeme << std::endl;
  }
}
