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

#include "ui/ActionExecutionContext.h"
#include "ui/AppControllerFixture.h"
#include "ui/CatchConfig.h"
#include "ui/KeyboardShortcutUtils.h"

#include <cstddef>
#include <vector>

#include <catch2/catch_test_macros.hpp>

namespace tb::ui
{
namespace
{
constexpr auto CyrillicTse = 0x0426;
constexpr auto NativeVirtualW = 'W';

Action makeAction(
  const std::filesystem::path& preferencePath,
  const QKeySequence& shortcut,
  const bool enabled = true)
{
  return Action{
    preferencePath,
    "Action",
    ActionContext::Any,
    shortcut,
    [](auto&) {},
    [enabled](const auto&) { return enabled; }};
}

} // namespace

TEST_CASE("eventMatchesPhysicalShortcutFallback")
{
  SECTION("matches physical latin letter key on non-latin layout")
  {
    const auto event =
      QKeyEvent{QEvent::KeyPress, CyrillicTse, Qt::ControlModifier, 0, NativeVirtualW, 0};
    const auto shortcut = QKeySequence{Qt::CTRL | Qt::Key_W};

    CHECK(eventMatchesPhysicalShortcutFallback(event, shortcut));
  }

  SECTION("does not report fallback when logical key already matches")
  {
    const auto event =
      QKeyEvent{QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier, 0, NativeVirtualW, 0};
    const auto shortcut = QKeySequence{Qt::CTRL | Qt::Key_W};

    CHECK_FALSE(eventMatchesPhysicalShortcutFallback(event, shortcut));
  }
}

TEST_CASE("findFallbackAction")
{
  auto appControllerFixture = AppControllerFixture{};
  auto& appController = appControllerFixture.appController();

  const auto event =
    QKeyEvent{QEvent::KeyPress, CyrillicTse, Qt::ControlModifier, 0, NativeVirtualW, 0};
  const auto context = ActionExecutionContext{appController, nullptr, nullptr};

  auto matchingAction = makeAction("matching", QKeySequence{Qt::CTRL | Qt::Key_W});
  auto nonMatchingAction = makeAction("nonMatching", QKeySequence{Qt::CTRL | Qt::Key_X});
  auto disabledMatchingAction =
    makeAction("disabledMatching", QKeySequence{Qt::CTRL | Qt::Key_W}, false);

  SECTION("returns no match if no action matches")
  {
    const auto actions =
      std::vector<Action*>{&nonMatchingAction, &disabledMatchingAction};

    const auto match = findFallbackAction(event, context, actions);

    CHECK(std::holds_alternative<NoFallbackActionMatch>(match));
  }

  SECTION("returns the unique matching enabled action")
  {
    const auto actions =
      std::vector<Action*>{&nonMatchingAction, &matchingAction, &disabledMatchingAction};

    const auto match = findFallbackAction(event, context, actions);

    REQUIRE(std::holds_alternative<UniqueFallbackActionMatch>(match));
    CHECK(&std::get<UniqueFallbackActionMatch>(match).action == &matchingAction);
  }

  SECTION("returns the first matching action if the match is ambiguous")
  {
    auto otherMatchingAction =
      makeAction("otherMatching", QKeySequence{Qt::CTRL | Qt::Key_W});
    const auto actions =
      std::vector<Action*>{&nonMatchingAction, &matchingAction, &otherMatchingAction};

    const auto match = findFallbackAction(event, context, actions);

    REQUIRE(std::holds_alternative<AmbiguousFallbackActionMatch>(match));
    CHECK(&std::get<AmbiguousFallbackActionMatch>(match).action == &matchingAction);
  }
}


} // namespace tb::ui
