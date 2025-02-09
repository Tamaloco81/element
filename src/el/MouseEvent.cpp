// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

/// A mouse event
// @classmod el.MouseEvent
// @pragma nostrip

#include <element/juce/gui_basics.hpp>
#include <element/element.h>
#include "sol_helpers.hpp"

#define EL_TYPE_NAME_MOUSE_EVENT "MouseEvent"

using namespace juce;

// clang-format off
EL_PLUGIN_EXPORT
int luaopen_el_MouseEvent (lua_State* L)
{
    sol::state_view lua (L);
    auto M = lua.create_table();
    M.new_usertype<MouseEvent> (EL_TYPE_NAME_MOUSE_EVENT, sol::no_constructor,
        /// Attributes.
        // @section attributes

        /// Position as @{el.Point}.
        // @field MouseEvent.position
        "position", sol::readonly_property ([] (MouseEvent& self) {
            return self.position.toFloat();
        }),

        /// X position.
        // @tfield int MouseEvent.x
        "x", &MouseEvent::x,

        /// Y position.
        // @tfield int MouseEvent.y
        "y", &MouseEvent::y,

        /// Pressure (0.0-1.0).
        // @tfield number MouseEvent.pressure
        "pressure",  &MouseEvent::pressure,

        /// Orientation in radians.
        // @tfield number MouseEvent.orientation
        "orientation", &MouseEvent::orientation,

        /// Rotation in radians.
        // @tfield number MouseEvent.rotation
        "rotation", &MouseEvent::rotation,

        /// Tilt X position (-1.0-1.0).
        // @tfield number MouseEvent.tiltX
        "tiltX", &MouseEvent::tiltX,

        /// Tilt Y (-1.0-1.0).
        // @tfield number MouseEvent.tiltY
        "tiltY", &MouseEvent::tiltY);

    sol::stack::push (L, element::lua::removeAndClear (M, EL_TYPE_NAME_MOUSE_EVENT));
    return 1;
}
