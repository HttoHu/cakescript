#include <map>
#include <utils.h>
namespace cake::utils {
std::optional<std::string> conv_escape(std::string_view text) {
  static std::map<char, char> escape_character_table = {{'r', '\r'},  {'n', '\n'},  {'b', '\b'},
                                                        {'t', '\t'},  {'a', '\a'},  {'0', '\0'},
                                                        {'\'', '\''}, {'\"', '\"'}, {'\\', '\\'}};
  auto _convert_escape = [&](char c) {
    auto result = escape_character_table.find(c);
    if (result == escape_character_table.end()) {
      throw std::runtime_error("");
    }
    return result->second;
  };

  try {
    int pos = 0;
    std::string value;
    for (; pos < text.size(); pos++) {
      if (text[pos] == '\\') {
        pos++;
        if (pos >= text.size())
          return std::nullopt;
        char tmp = _convert_escape(text[pos]);
        value += tmp;
      } else
        value += text[pos];
    }
    return value;
  } catch (std::exception &e) {
    return std::nullopt;
  }
}
} // namespace cake::utils