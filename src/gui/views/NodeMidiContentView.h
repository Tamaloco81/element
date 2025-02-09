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

#include <element/ui/content.hpp>
#include "gui/widgets/MidiChannelSelectComponent.h"
#include "gui/widgets/NodeMidiProgramComponent.h"

namespace element {

class NodeMidiContentView : public ContentView
{
public:
    NodeMidiContentView();
    ~NodeMidiContentView();

    void stabilizeContent() override;

    void resized() override;
    void paint (Graphics& g) override;

private:
    Node node;
    SignalConnection selectedNodeConnection;
    SignalConnection midiProgramChangedConnection;

    class SignalLabel : public Label
    {
    public:
        SignalLabel() {}
        ~SignalLabel() {}

        inline void mouseDoubleClick (const MouseEvent& ev) override
        {
            if (onDoubleClicked)
                onDoubleClicked (ev);
        }

        std::function<void (const MouseEvent&)> onDoubleClicked;
    };

    PropertyPanel props;
    NodeObjectSync nodeSync;
    void updateProperties();
    void updateMidiProgram();
};

} // namespace element
