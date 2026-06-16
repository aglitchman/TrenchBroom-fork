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
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui/KeyboardShortcutUtils.h"

#include <QKeyEvent>
#include <QKeySequence>

namespace tb::ui
{
namespace
{

constexpr auto ShortcutModifierMask =
  Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;

bool isLatinLetterOrDigit(const Qt::Key key)
{
  return (key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9);
}

Qt::Key physicalKey(const QKeyEvent& event)
{
  const auto nativeVirtualKey = event.nativeVirtualKey();
  if (nativeVirtualKey >= 'A' && nativeVirtualKey <= 'Z')
  {
    return static_cast<Qt::Key>(
      static_cast<int>(Qt::Key_A) + static_cast<int>(nativeVirtualKey - 'A'));
  }
  if (nativeVirtualKey >= '0' && nativeVirtualKey <= '9')
  {
    return static_cast<Qt::Key>(
      static_cast<int>(Qt::Key_0) + static_cast<int>(nativeVirtualKey - '0'));
  }

  return static_cast<Qt::Key>(event.key());
}

bool matchesShortcut(
  const QKeyEvent& event,
  const QKeySequence& shortcut,
  const bool compareModifiers,
  const bool usePhysicalKey)
{
  if (shortcut.count() != 1)
  {
    return false;
  }

  const auto keyCombination = shortcut[0];
  const auto shortcutKey = keyCombination.key();
  const auto eventKey = usePhysicalKey && isLatinLetterOrDigit(shortcutKey)
                          ? physicalKey(event)
                          : static_cast<Qt::Key>(event.key());

  if (shortcutKey != eventKey)
  {
    return false;
  }

  return !compareModifiers
         || (keyCombination.keyboardModifiers() & ShortcutModifierMask)
              == (event.modifiers() & ShortcutModifierMask);
}

} // namespace

bool eventMatchesPhysicalKey(const QKeyEvent& event, const QKeySequence& shortcut)
{
  return matchesShortcut(event, shortcut, false, true);
}

} // namespace tb::ui
