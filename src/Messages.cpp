/*
    This file is part of Element
    Copyright (C) 2018-2019 Kushview, LLC.  All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Signals.h"
#include "controllers/EngineController.h"
#include "Messages.h"

namespace Element {

class AddPluginAction : public UndoableAction
{
public:
    AddPluginAction (AppController& _app, const AddPluginMessage& msg)
        : app (_app), graph (msg.graph), description (msg.description), builder (msg.builder), verified (msg.verified) {}
    ~AddPluginAction() noexcept {}

    bool perform() override
    {
        addedNode = Node();
        if (auto* ec = app.findChild<EngineController>())
            if (graph.isGraph())
                addedNode = ec->addPlugin (graph, description, builder, verified);
        return addedNode.isValid();
    }

    bool undo() override
    {
        if (! addedNode.isValid())
            return false;
        if (! havePosition)
            addedNode.getRelativePosition (x, y);
        havePosition = true;
        if (auto* ec = app.findChild<EngineController>())
            ec->removeNode (addedNode);
        addedNode = Node();
        return true;
    }

private:
    AppController& app;
    const Node graph;
    const PluginDescription description;
    const ConnectionBuilder builder;
    const bool verified = true;
    double x, y;
    bool havePosition = false;
    Node addedNode;
};

class RemoveNodeAction : public UndoableAction
{
public:
    explicit RemoveNodeAction (AppController& a, const Node& node)
        : app (a), targetGraph (node.getParentGraph()), nodeUuid (node.getUuid())
    {
        node.getArcs (arcs);
        Node mutableNode (node);
        mutableNode.savePluginState();
        node.getRelativePosition (x, y);
        nodeData = node.getValueTree().createCopy();
        Node::sanitizeRuntimeProperties (nodeData);
    }

    bool perform() override
    {
        auto& ec = *app.findChild<EngineController>();
        ec.removeNode (nodeUuid);
        return true;
    }

    bool undo() override
    {
        auto& ec = *app.findChild<EngineController>();
        bool handled = true;

        const Node newNode (nodeData, false);
        auto createdNode (ec.addNode (newNode, targetGraph, builder));
        createdNode.setRelativePosition (x, y); // TODO: GraphManager should handle this

        for (const auto* arc : arcs)
            ec.addConnection (arc->sourceNode, arc->sourcePort, arc->destNode, arc->destPort, targetGraph);

        return handled;
    }

private:
    AppController& app;
    ValueTree nodeData;
    const Node targetGraph;
    const Uuid nodeUuid;
    ConnectionBuilder builder;
    OwnedArray<Arc> arcs;
    double x = 0.5;
    double y = 0.5;
    bool isDataValid() const
    {
        return targetGraph.isGraph() && ! nodeUuid.isNull() && nodeData.isValid();
    }
};

class AddConnectionAction : public UndoableAction
{
public:
    AddConnectionAction (AppController& a, const Node& targetGraph, const uint32 sn, const uint32 sp, const uint32 dn, const uint32 dp)
        : app (a), graph (targetGraph), arc (sn, sp, dn, dp)
    {
    }

    bool perform() override
    {
        auto& ec = *app.findChild<EngineController>();
        if (! graph.isValid())
            ec.addConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort);
        else
            ec.addConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort, graph);
        return true;
    }

    bool undo() override
    {
        auto& ec = *app.findChild<EngineController>();
        if (! graph.isValid())
            ec.removeConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort);
        else
            ec.removeConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort, graph);
        return true;
    }

private:
    AppController& app;
    const Node graph;
    const Arc arc;
};

class RemoveConnectionAction : public UndoableAction
{
public:
    RemoveConnectionAction (AppController& a, const Node& targetGraph, const uint32 sn, const uint32 sp, const uint32 dn, const uint32 dp)
        : app (a), graph (targetGraph), arc (sn, sp, dn, dp)
    {
    }

    bool perform() override
    {
        auto& ec = *app.findChild<EngineController>();
        if (! graph.isValid())
            ec.removeConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort);
        else
            ec.removeConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort, graph);
        return true;
    }

    bool undo() override
    {
        auto& ec = *app.findChild<EngineController>();
        if (! graph.isValid())
            ec.addConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort);
        else
            ec.addConnection (arc.sourceNode, arc.sourcePort, arc.destNode, arc.destPort, graph);
        return true;
    }

private:
    AppController& app;
    const Node graph;
    const Arc arc;
};

//=============================================================================

void AddPluginMessage::createActions (AppController& app, OwnedArray<UndoableAction>& actions) const
{
    actions.add (new AddPluginAction (app, *this));
}

void RemoveNodeMessage::createActions (AppController& app, OwnedArray<UndoableAction>& actions) const
{
    if (node.isValid())
        actions.add (new RemoveNodeAction (app, node));
    for (const auto& n : nodes)
        actions.add (new RemoveNodeAction (app, n));
}

void AddConnectionMessage::createActions (AppController& app, OwnedArray<UndoableAction>& actions) const
{
    jassert (usePorts()); // channel-ports not yet supported
    actions.add (new AddConnectionAction (app, target, sourceNode, sourcePort, destNode, destPort));
}

void RemoveConnectionMessage::createActions (AppController& app, OwnedArray<UndoableAction>& actions) const
{
    jassert (usePorts()); // channel-ports not yet supported
    actions.add (new RemoveConnectionAction (app, target, sourceNode, sourcePort, destNode, destPort));
}

} // namespace Element
