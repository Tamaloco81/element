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

#if 0

    //==============================================================================
    GraphPort::GraphPort (const PortType type_, bool isInputPort)
        : portType (type_),
          graph (nullptr),
          portIsInput (isInputPort)
    {
        if (portType.id() == PortType::Audio)
        {
            isInput() ? setPlayConfigDetails (0, 1, getSampleRate(), getBlockSize())
                      : setPlayConfigDetails (1, 0, getSampleRate(), getBlockSize());
        }
    }

    GraphPort::~GraphPort() { }

    uint32 GraphPort::getNumPorts()
    {
        return uint32 (1);
    }

    PortType GraphPort::getPortType (uint32) const
    {
        return portType;
    }

    const String GraphPort::getName() const
    {
        String name = portType.getName();
        isInput() ? name << String(" In") : name << String(" Out");
        return name;
    }

    void GraphPort::fillInPluginDescription (PluginDescription& d) const
    {
        d.name = getName();
        d.fileOrIdentifier = portType.getURI();
        d.uniqueId  = d.fileOrIdentifier.hashCode();
        d.category = "Ports";
        d.pluginFormatName = "Internal";
        d.manufacturerName = "Element Project";
        d.version = "1.0";
        d.isInstrument = false;
        d.numInputChannels  = getTotalNumInputChannels();
        d.numOutputChannels = getTotalNumOutputChannels();
    }

    void GraphPort::prepareToPlay (double, int)
    {
        jassert (graph != nullptr);
    }

    void GraphPort::releaseResources()
    {
    }

    void GraphPort::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
    {
        jassert (graph != nullptr);

        switch (portType.id())
        {
            case PortType::Audio:
            {
                if (isOutput())
                {
                    for (int i = jmin (graph->currentAudioOutputBuffer.getNumChannels(),
                                       buffer.getNumChannels()); --i >= 0;)
                    {
                        graph->currentAudioOutputBuffer.addFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
                    }
                }
                else
                {
                    for (int i = jmin (graph->currentAudioInputBuffer->getNumChannels(),
                                       buffer.getNumChannels()); --i >= 0;)
                    {
                        buffer.copyFrom (i, 0, *graph->currentAudioInputBuffer, i, 0, buffer.getNumSamples());
                    }

                    break;
                }
            }
            case PortType::Atom:
            {
                if (isOutput())
                {
                    graph->currentMidiOutputBuffer.addEvents (midiMessages, 0, buffer.getNumSamples(), 0);
                }
                else
                {
                    midiMessages.addEvents (*graph->currentMidiInputBuffer, 0, buffer.getNumSamples(), 0);
                }
            }
            default:
                break;
        }
    }

    bool GraphPort::silenceInProducesSilenceOut() const
    {
        return isOutput();
    }

    double GraphPort::getTailLengthSeconds() const
    {
        return 0;
    }

    bool GraphPort::acceptsMidi() const
    {
        return portType == PortType::Atom && isOutput();
    }

    bool GraphPort::producesMidi() const
    {
        return portType ==  PortType::Atom && isInput();
    }

    const String GraphPort::getInputChannelName (int channelIndex) const
    {
        if (PortType::Audio == portType.id() && isOutput())
            return getName() + String (channelIndex + 1);

        return String();
    }

    const String GraphPort::getOutputChannelName (int channelIndex) const
    {
        if (PortType::Audio == portType.id() && isInput())
            return getName() + String (channelIndex + 1);

        return String();
    }

    bool GraphPort::isInputChannelStereoPair (int /*index*/) const
    {
        return false;
    }

    bool GraphPort::isOutputChannelStereoPair (int /*index*/) const
    {
        return false;
    }

    bool GraphPort::isInput() const   { return portIsInput; }
    bool GraphPort::isOutput() const  { return ! portIsInput; }

    #if 1
    bool GraphPort::hasEditor() const                  { return false; }
    AudioProcessorEditor* GraphPort::createEditor()    { return nullptr; }
    #endif
    int GraphPort::getNumParameters()                  { return 0; }
    const String GraphPort::getParameterName (int)     { return String(); }

    float GraphPort::getParameter (int)                { return 0.0f; }
    const String GraphPort::getParameterText (int)     { return String(); }
    void GraphPort::setParameter (int, float)          { }

    int GraphPort::getNumPrograms()                    { return 0; }
    int GraphPort::getCurrentProgram()                 { return 0; }
    void GraphPort::setCurrentProgram (int)            { }

    const String GraphPort::getProgramName (int)       { return String(); }
    void GraphPort::changeProgramName (int, const String&) { }

    void GraphPort::getStateInformation (MemoryBlock&) { }
    void GraphPort::setStateInformation (const void*, int) { }

    void GraphPort::setGraph (GraphProcessor* const newGraph)
    {
        graph = newGraph;

        if (graph != nullptr)
        {
    #if 0
            setPlayConfigDetails (portType == audioOutputNode ? graph->getNumOutputChannels() : 0,
                                  portType == audioInputNode ? graph->getNumInputChannels() : 0,
                                  getSampleRate(),
                                  getBlockSize());
    #endif
            updateHostDisplay();
        }
    }

#endif
