// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

#pragma once

#include "ElementApp.h"
#include "gui/ViewHelpers.h"

namespace element {

class BlockComponent;
class BlockFactory;
class ConnectorComponent;
class PortComponent;
class PluginWindow;

/** A panel that displays and edits a Graph. */
class GraphEditorComponent : public Component,
                             public ChangeListener,
                             public DragAndDropTarget,
                             public FileDragAndDropTarget,
                             private ValueTree::Listener,
                             public ViewHelperMixin,
                             public LassoSource<uint32>
{
public:
    GraphEditorComponent();
    virtual ~GraphEditorComponent();

    /** Set the currently displayed graph */
    void setNode (const Node& n);

    /** Returns the displayed graph */
    Node getGraph() const { return graph; }

    //=========================================================================
    void findLassoItemsInArea (Array<uint32>& itemsFound, const Rectangle<int>& area) override;
    SelectedItemSet<uint32>& getLassoSelection() override { return selectedNodes; }

    /** Selects the given node */
    void selectNode (const Node& n);
    void selectAllNodes();

    /** Removes the selected nodes */
    void deleteSelectedNodes();

    //=========================================================================
    void setZoomScale (float scale);
    float getZoomScale() const noexcept { return zoomScale; }

    //=========================================================================
    /** Returns true if the layout is vertical */
    bool isLayoutVertical() const { return verticalLayout; }

    /** Changes the layout to vertical or not */
    void setVerticalLayout (const bool isVertical);

    //=========================================================================
    Rectangle<int> getRequiredSpace() const;

    //=========================================================================
    /** Stabilize all nodes without changing position */
    void stabilizeNodes();

    void updateComponents (const bool doNodePositions = true);

    //=========================================================================
    void changeListenerCallback (ChangeBroadcaster*) override;

    void paint (Graphics& g) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

    bool isInterestedInDragSource (const SourceDetails&) override;
    void itemDropped (const SourceDetails& details) override;
    bool shouldDrawDragImageWhenOver() override { return true; }

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;

    //=========================================================================
    std::function<bool (const StringArray&, int, int)> onFilesDropped;
    std::function<void (BlockComponent&)> onBlockMoved;
    std::function<void()> onZoomChanged;

protected:
    virtual Component* wrapAudioProcessorEditor (AudioProcessorEditor* ed, ProcessorPtr editorNode);
    void createNewPlugin (const PluginDescription* desc, int x, int y);

private:
    friend class ConnectorComponent;
    friend class BlockComponent;
    friend class PortComponent;

    Node graph;
    ValueTree data;

    float lastDropX = 0.5f;
    float lastDropY = 0.5f;

    std::unique_ptr<ConnectorComponent> draggingConnector;
    std::unique_ptr<BlockFactory> factory;

    bool verticalLayout = true;

    LassoComponent<uint32> lasso;
    friend class SelectedNodes;
    class SelectedNodes : public SelectedItemSet<uint32>
    {
    public:
        SelectedNodes (GraphEditorComponent& owner)
            : editor (owner) {}
        void itemSelected (uint32 nodeId) override;
        void itemDeselected (uint32 nodeId) override;
        GraphEditorComponent& editor;
    } selectedNodes;

    bool ignoreNodeSelected = false;

    float zoomScale = 1.0;

    void selectNode (const Node& node, ModifierKeys mods);
    void setSelectedNodesCompact (bool selected);

    Component* createContainerForNode (ProcessorPtr node, bool useGenericEditor);
    AudioProcessorEditor* createEditorForNode (ProcessorPtr node, bool useGenericEditor);
    PluginWindow* getOrCreateWindowForNode (ProcessorPtr f, bool useGeneric);

    void updateBlockComponents (const bool doPosition = true);
    void updateConnectorComponents (bool async = false);

    void beginConnectorDrag (const uint32 sourceFilterID, const int sourceFilterChannel, const uint32 destFilterID, const int destFilterChannel, const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endDraggingConnector (const MouseEvent& e);

    BlockComponent* createBlock (const Node&);

    BlockComponent* getComponentForFilter (const uint32 filterID) const;
    ConnectorComponent* getComponentForConnection (const Arc& conn) const;
    PortComponent* findPinAt (const int x, const int y) const;

    void updateSelection();

    void valueTreePropertyChanged (ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override {}
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override {}
    void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex,
                                     int newIndex) override {}
    void valueTreeParentChanged (ValueTree& treeWhoseParentHasChanged) override {}
    void valueTreeRedirected (ValueTree& treeWhichHasBeenChanged) override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorComponent)
};

} // namespace element
