/*
    Author: Jatin Chowdhury (jatin@ccrma.stanford.edu)
    
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

#include <element/juce/gui_basics.hpp>
#include <element/juce/audio_processors.hpp>

namespace element {

class KnobsComponent : public juce::Component
{
public:
    KnobsComponent (juce::AudioProcessor& proc, std::function<void()> paramLambda = {});
    ~KnobsComponent() {}

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::ComboBox> boxes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobsComponent)
};

} // namespace element
