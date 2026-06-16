/*
 Copyright (C) 2026 Artsiom Trubchyk
 Copyright (C) 2026 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QKeyEvent>

#include "ui/CatchConfig.h"
#include "ui/KeyboardShortcutUtils.h"

#include <catch2/catch_test_macros.hpp>

namespace tb::ui
{
namespace
{
constexpr auto CyrillicTse = 0x0426;
constexpr auto NativeVirtualW = 'W';

} // namespace

TEST_CASE("eventMatchesPhysicalKey")
{
  SECTION("matches physical latin letter key on non-latin layout")
  {
    const auto event = QKeyEvent{QEvent::KeyPress, CyrillicTse, {}, 0, NativeVirtualW, 0};
    const auto shortcut = QKeySequence{Qt::Key_W};

    CHECK(eventMatchesPhysicalKey(event, shortcut));
  }

  SECTION("matches logical latin letter key")
  {
    const auto event = QKeyEvent{QEvent::KeyPress, Qt::Key_W, {}, 0, NativeVirtualW, 0};
    const auto shortcut = QKeySequence{Qt::Key_W};

    CHECK(eventMatchesPhysicalKey(event, shortcut));
  }

  SECTION("ignores modifiers for fly mode")
  {
    const auto event =
      QKeyEvent{QEvent::KeyPress, CyrillicTse, Qt::ShiftModifier, 0, NativeVirtualW, 0};
    const auto shortcut = QKeySequence{Qt::Key_W};

    CHECK(eventMatchesPhysicalKey(event, shortcut));
  }
}

} // namespace tb::ui
