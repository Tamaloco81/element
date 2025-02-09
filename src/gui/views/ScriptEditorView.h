/*
    This file is part of Element.
    Copyright (C) 2021  Kushview, LLC.  All rights reserved.

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

#include <element/ui/view.hpp>
#include "gui/widgets/ScriptEditorComponent.h"
#include "scripting/scriptsource.hpp"
#include <element/node.hpp>
#include <element/script.hpp>

namespace element {

class ScriptNode;

class BaseScriptEditorView : public View
{
protected:
    BaseScriptEditorView();

public:
    virtual ~BaseScriptEditorView();

    ScriptEditorComponent& getEditor() { return *editor; }

    /** Returns the code document used by this view */
    CodeDocument& getCodeDocument() { return code; }

    /** Returns the code document used by this view */
    const CodeDocument& getCodeDocument() const { return code; }

    /** Reload the buffer. */
    void reload();

    /** Reset the buffer, undo history, and save point */
    void reset();

    /** @internal */
    virtual void resized() override;

    //=========================================================================
    /** @internal */

#if 0
    void initializeView (Services&) override;
    virtual void willBeRemoved() { }
    virtual void willBecomeActive() { }
    virtual void didBecomeActive() { }
    virtual void stabilizeContent() { }

    /** Save state to user settings */
    virtual void saveState (PropertiesFile*) {}

    /** Restore state from user settings */
    virtual void restoreState (PropertiesFile*) {}

    /** Get state attached to session */
    virtual void getState (String&) {}

    /** Apply state attached to session */
    virtual void setState (const String&) {}
#endif

protected:
    virtual String getScriptContent() const = 0;

private:
    std::unique_ptr<ScriptEditorComponent> editor;
    LuaTokeniser tokens;
    CodeDocument code;
};

//==============================================================================
class ScriptEditorView : public BaseScriptEditorView
{
public:
    ScriptEditorView() = delete;
    ScriptEditorView (const Script& s);
    virtual ~ScriptEditorView();

    /** @internal */
    void resized() override;

protected:
    String getScriptContent() const override;

private:
    Script script;
    TextButton saveButton;
};

//==============================================================================
class ScriptNodeScriptEditorView : public BaseScriptEditorView
{
public:
    ScriptNodeScriptEditorView (const Node& n, bool editUI);
    ~ScriptNodeScriptEditorView() = default;

    bool isEditingUI() const { return editingUI; }

    /** @internal */
    void resized() override;

protected:
    String getScriptContent() const override;

private:
    Node node;
    bool editingUI;
    TextButton applyButton;
};

} // namespace element
