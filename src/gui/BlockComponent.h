// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

#pragma once

#include <element/node.hpp>
#include "gui/Buttons.h"

namespace element {

class Services;
class GraphEditorComponent;

//=============================================================================
class PortComponent : public Component,
                      public SettableTooltipClient
{
public:
    PortComponent() = delete;
    PortComponent (const Node& graph, const Node& node, const uint32 nodeId, const uint32 index, const bool isInput, const PortType type, const bool vertical);
    virtual ~PortComponent();

    /** Returns true if this is for an input port */
    bool isInput() const noexcept;

    /** Returns the node ID of this port */
    uint32 getNodeId() const noexcept;

    /** Returns this port's index */
    uint32 getPortIndex() const noexcept;

    /** Returns the color this port is painted */
    virtual Colour getColor() const noexcept;

    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    Node graph;
    Node node;
    uint32 nodeID { 0 };
    uint32 port { 0 };
    PortType type { PortType::Unknown };
    bool input { true };
    bool vertical { false };

    GraphEditorComponent* getGraphEditor() const noexcept;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PortComponent)
};

//=============================================================================
class BlockComponent : public Component,
                       public Button::Listener,
                       private AsyncUpdater,
                       private Value::Listener,
                       private ChangeListener
{
public:
    BlockComponent() = delete;
    BlockComponent (const Node& graph_, const Node& node_, const bool vertical_);
    ~BlockComponent() noexcept;

    enum DisplayMode
    {
        Compact,
        Small,
        Normal,
        Embed
    };

    inline static DisplayMode getDisplayModeFromString (const String& str)
    {
        auto s = str.toLowerCase().trim();
        if (str == "normal")
            return Normal;
        if (str == "compact")
            return Compact;
        if (str == "small")
            return Small;
        if (str == "embed")
            return Embed;
        return Normal;
    }

    inline static String getDisplayModeKey (BlockComponent::DisplayMode mode)
    {
        String key = "";
        switch (mode)
        {
            case Normal:
                key = "normal";
                break;
            case Compact:
                key = "compact";
                break;
            case Small:
                key = "small";
                break;
            case Embed:
                key = "embed";
                break;
        }
        return key;
    }

    inline static String getDisplayModeName (BlockComponent::DisplayMode mode)
    {
        String name = "";
        switch (mode)
        {
            case Normal:
                name = "Normal";
                break;
            case Compact:
                name = "Compact";
                break;
            case Small:
                name = "Small";
                break;
            case Embed:
                name = "Embed";
                break;
        }
        return name;
    }

    void setDisplayMode (DisplayMode mode);
    DisplayMode getDisplayMode() const noexcept { return displayMode; }

    //=========================================================================
    void moveBlockTo (double x, double y);

    //=========================================================================
    bool isSelected() const noexcept;

    //=========================================================================
    /** Change the power button's visibility */
    void setPowerButtonVisible (bool);

    /** Returns the power button */
    SettingButton& getPowerButton() { return powerButton; }

    //=========================================================================
    /** Change the config button's visibility */
    void setConfigButtonVisible (bool);

    /** Returns the config button */
    SettingButton& getConfigButton() { return configButton; }

    //=========================================================================
    /** Change the mute button's visibility */
    void setMuteButtonVisible (bool);

    /** Returns the config button */
    SettingButton& getMuteButton() { return muteButton; }

    //=========================================================================
    /** Gets the coordinate of the port index 
        Returns true if the coords were acquired.
     */
    bool getPortPos (const int index, const bool isInput, float& x, float& y);

    /** @internal */
    void buttonClicked (Button* b) override;
    /** @internal */
    bool hitTest (int x, int y) override;
    /** @internal */
    void mouseDown (const MouseEvent& e) override;
    /** @internal */
    void mouseDrag (const MouseEvent& e) override;
    /** @internal */
    void mouseUp (const MouseEvent& e) override;
    /** @internal */
    void mouseMove (const MouseEvent& e) override;
    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void paintOverChildren (Graphics& g) override;
    /** @internal */
    void resized() override;

protected:
    inline void forEachSibling (std::function<void (BlockComponent&)> callback)
    {
        if (! callback)
            return;
        if (auto* parent = getParentComponent())
            for (int i = 0; i < parent->getNumChildComponents(); ++i)
                if (auto* block = dynamic_cast<BlockComponent*> (parent->getChildComponent (i)))
                    if (block != nullptr && block != this)
                        callback (*block);
    }

private:
    friend class GraphEditorComponent;
    friend class GraphEditorView;
    friend class juce::ChangeBroadcaster;

    const uint32 filterID;
    Node graph;
    Node node;

    Value nodeEnabled,
        nodeName,
        hiddenPorts,
        displayModeValue;

    int numIns = 0, numOuts = 0;

    double relativeX = 0.5f;
    double relativeY = 0.5f;

    int pinSize = 9;
    int pinSpacing = 9;
    Font font;

    Point<int> originalPos;
    Rectangle<int> originalBounds;
    int customWidth = 0,
        customHeight = 0;
    bool selectionMouseDownResult = false;
    bool vertical = true;
    bool dragging = false;
    bool blockDrag = false;
    bool collapsed = false;

    int lastDragDeltaX = 0;
    int lastDragDeltaY = 0;

    SettingButton configButton;
    PowerButton powerButton;
    SettingButton muteButton;

    juce::OptionalScopedPointer<CallOutBox> ioBox;

    DropShadowEffect shadow;
    std::unique_ptr<Component> embedded;

    Colour color { 0x00000000 };
    class BlockColorSelector : public juce::ColourSelector
    {
    public:
        BlockColorSelector() : ColourSelector (ColourSelector::showColourAtTop | ColourSelector::editableColour | ColourSelector::showColourspace)
        {
            colors.addArray ({
                Colour (0xFF800000), // Dark Red
                Colour (0xFFE24B00), // Dark Orange
                Colour (0xFFEDED00), // Dark Yellow
                Colour (0xFF247D21), // Pastel Green
                Colour (0xFF2969BE), // Pastel Blue
                Colour (0xFF5627A1), // Purple
                Colour (0xFF363636), // Jet
                Colour (0xFFFFFCFF) // Snow
            });

            colors.addArray ({
                Colour (0xFFE03636), // Light red
                Colour (0xFFDB3069),
                // Colour (0xFFF95738), // Orange Soda
                Colour (0xFFEAC435), // Saffron
                Colour (0xFF90BE6D),
                Colour (0xFF43AA8B),
                Colour (0xFF017798), // CG Blue
                Colour (0xFF8C9A9E), // Cadet Grey
                Colour (0xFF475B63),
            });
        }

        ~BlockColorSelector() = default;

        int getNumSwatches() const { return colors.size(); }

        Colour getSwatchColour (int index) const
        {
            return isPositiveAndBelow (index, colors.size())
                       ? colors.getReference (index)
                       : fallback;
        }

        void setSwatchColour (int index, const Colour& color)
        {
            if (isPositiveAndBelow (index, colors.size()))
                colors.set (index, color);
        }

    private:
        Array<Colour> colors;
        Colour fallback { 0xFF000000 };
    };

    BlockColorSelector colorSelector;

    DisplayMode displayMode { Normal };
    bool selected { false };

    void changeListenerCallback (ChangeBroadcaster*) override;

    void deleteAllPins();

    bool mouseInCornerResize = false;
    Rectangle<int> getBoxRectangle() const;
    Rectangle<int> getCornerResizeBox() const;

    GraphEditorComponent* getGraphPanel() const noexcept;
    void setButtonVisible (Button&, bool);

    void getMinimumSize (int& width, int& height);
    void setCustomSize (int width, int height);
    void setNodePosition (const int x, const int y);
    template <typename T>
    void setNodePosition (const Point<T>& pt)
    {
        const auto pti = pt.toInt();
        setNodePosition (pti.x, pti.y);
    }

    Point<double> getNodePosition() const noexcept;

    void setSelectedInternal (bool status);

    void makeEditorActive();

    void update (const bool doPosition = true, const bool forcePins = false);
    void updateSize();
    void updatePins (bool force);
    void updatePosition();

    void handleAsyncUpdate() override;
    void valueChanged (Value& value) override;

    void addDisplaySubmenu (PopupMenu& menuToAddTo);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlockComponent);
};

//=============================================================================
class BlockFactory
{
public:
    virtual ~BlockFactory() = default;
    virtual BlockComponent* createBlockComponent (Services& app, const Node& node) = 0;

protected:
    BlockFactory() = default;
};

} // namespace element
