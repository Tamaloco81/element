/*
    GraphModel.cpp - This file is part of Element
    Copyright (C) 2014-2017  Kushview, LLC.  All rights reserved.
*/

#include "ElementApp.h"
#include "session/GraphModel.h"

namespace Element {
    
Graph::Graph (const Identifier& t) : Node()
{
    objectData.setProperty (Slugs::type, t.toString(), nullptr);
}

Graph::~Graph() { }
void Graph::addPlugin () { }
void Graph::addSubGraph (const GraphModel& graph) { }

}
