/*
    This file is part of Element
    Copyright (C) 2019  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#pragma once

#include "ElementApp.h"
#include <element/ui/content.hpp>

#define EL_VIEW_KEYMAP_EDITOR "KeymapEditorView"

namespace element {
class KeymapEditorView : public ContentView, public Button::Listener
{
public:
    KeymapEditorView();
    virtual ~KeymapEditorView() {}

    void resized() override;

    inline void willBecomeActive() override {}
    inline void didBecomeActive() override { stabilizeContent(); }
    void stabilizeContent() override;

    void buttonClicked (Button*) override;

private:
    std::unique_ptr<KeyMappingEditorComponent> editor;
    TextButton closeButton;

    void saveMappings();
};
} // namespace element
