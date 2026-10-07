#pragma once
#include <M5Cardputer.h>

enum class NavKey { None, Up, Down, Left, Right, Escape };

class KeyboardADV {
public:
  void begin() {}
  void update() { M5Cardputer.update(); }
  bool changed() const { return M5Cardputer.Keyboard.isChange(); }
  const Keyboard_Class::KeysState& state() const { return M5Cardputer.Keyboard.keysState(); }

  NavKey navigation() const {
    const auto &s = state();
    if (!s.fn) return NavKey::None;
    for (auto c : s.word) {
      if (c == ';') return NavKey::Up;
      if (c == ',') return NavKey::Left;
      if (c == '.') return NavKey::Down;
      if (c == '/') return NavKey::Right;
      if (c == '`') return NavKey::Escape;
    }
    return NavKey::None;
  }

  char printable() const {
    const auto &s = state();
    if (s.fn) return 0;
    if (s.word.empty()) return 0;
    return s.word.front();
  }
};
