// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

#include "gui/SessionImportWizard.h"
#include "gui/GuiCommon.h"
#include <element/context.hpp>

namespace element {

class SessionImportListBox : public ListBox,
                             public ListBoxModel
{
public:
    SessionImportListBox (SessionImportWizard& w)
        : wizard (w)
    {
        setModel (this);
    }

    ~SessionImportListBox()
    {
        setModel (nullptr);
    }

    int getNumRows() override
    {
        if (auto session = wizard.session())
            return session->getNumGraphs();
        return 0;
    }

    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        auto session = wizard.session();
        if (session == nullptr)
            return;
        const auto graph (session->getGraph (rowNumber));
        ViewHelpers::drawBasicTextRow (graph.getName(), g, width, height, rowIsSelected);
    }

    Node getSelectedGraph()
    {
        auto session = wizard.session();
        if (session == nullptr)
            return Node();
        return session->getGraph (getSelectedRow());
    }
#if 0
    virtual Component* refreshComponentForRow (int rowNumber, bool isRowSelected,
                                               Component* existingComponentToUpdate);
    virtual void listBoxItemClicked (int row, const MouseEvent&);
    virtual void listBoxItemDoubleClicked (int row, const MouseEvent&);
    virtual void backgroundClicked (const MouseEvent&);
    virtual void selectedRowsChanged (int lastRowSelected);
    virtual void deleteKeyPressed (int lastRowSelected);
    virtual void returnKeyPressed (int lastRowSelected);
    virtual void listWasScrolled();
    virtual var getDragSourceDescription (const SparseSet<int>& rowsToDescribe);
    virtual String getTooltipForRow (int row);
    virtual MouseCursor getMouseCursorForRow (int row);
#endif

private:
    SessionImportWizard& wizard;
};

class SessionImportWizard::Content : public Component
{
public:
    Content (SessionImportWizard& w)
        : graphsList (w)
    {
        setOpaque (true);
        addAndMakeVisible (instructions);
        instructions.setFont (Font (12.f));
        instructions.setColour (Label::textColourId, Colors::textColor);
        instructions.setText ("Chose a graph to import", dontSendNotification);
        instructions.setJustificationType (Justification::centred);
        addAndMakeVisible (graphsList);
        addAndMakeVisible (cancelButton);
        cancelButton.setButtonText ("Cancel");
        cancelButton.onClick = [this]() {
            if (auto* dialog = findParentComponentOfClass<SessionImportWizardDialog>())
                dialog->closeButtonPressed();
            else
            {
                jassertfalse;
            }
        };

        addAndMakeVisible (importButton);
        importButton.setButtonText ("Import");
        importButton.onClick = [this]() {
            if (auto* dialog = findParentComponentOfClass<SessionImportWizardDialog>())
            {
                const auto data = graphsList.getSelectedGraph().data();
                const Node graph (data.createCopy(), false);
                if (dialog->onGraphChosen)
                    dialog->onGraphChosen (graph);
                dialog->closeButtonPressed();
            }
            else
            {
                jassertfalse;
            }
        };
    }

    ~Content() {}

    void paint (Graphics& g) override
    {
        g.fillAll (Colors::contentBackgroundColor);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (4, 0);
        instructions.setBounds (r.removeFromTop (40));
        r.removeFromBottom (4);
        auto r2 = r.removeFromBottom (22);
        r2.removeFromRight (2);
        importButton.setBounds (r2.removeFromRight (80));
        r2.removeFromRight (4);
        cancelButton.setBounds (r2.removeFromRight (80));
        r.removeFromBottom (4);
        graphsList.setBounds (r);
    }

private:
    friend class SessionImportWizard;
    Label instructions;
    SessionImportListBox graphsList;
    TextButton importButton;
    TextButton cancelButton;
};

SessionImportWizard::SessionImportWizard()
{
    setOpaque (true);
    content.reset (new Content (*this));
    addAndMakeVisible (content.get());
    setSize (300, 500);
}

SessionImportWizard::~SessionImportWizard()
{
    content.reset();
}

void SessionImportWizard::loadSession (const File& file)
{
    SessionPtr newSession;
    bool loaded = false;
    if (auto e = XmlDocument::parse (file))
    {
        ValueTree newData (ValueTree::fromXml (*e));
        if (newData.isValid() && newData.hasType (types::Session))
        {
            newSession = new Session();
            loaded = newSession->loadData (newData);
        }
    }

    if (newSession != nullptr && loaded)
    {
        _session = newSession;
        content->graphsList.updateContent();
        content->graphsList.selectRow (0);
    }
}

SessionPtr SessionImportWizard::session()
{
    return _session;
}

void SessionImportWizard::paint (Graphics& g)
{
    g.fillAll (Colors::widgetBackgroundColor.darker());
}

void SessionImportWizard::resized()
{
    content->setBounds (getLocalBounds());
}

/*
const String& name,
                  Colour backgroundColour,
                  bool escapeKeyTriggersCloseButton,
                  bool addToDesktop = true);
                  */
SessionImportWizardDialog::SessionImportWizardDialog (std::unique_ptr<Component>& h, const File& file)
    : DialogWindow ("Import Session", Colors::widgetBackgroundColor.darker(), true, true),
      holder (h)
{
    holder.reset (this);
    setUsingNativeTitleBar (true);
    setTitleBarButtonsRequired (0, true);
    auto* wizard = new SessionImportWizard();
    wizard->loadSession (file);
    setContentOwned (wizard, true);
    setAlwaysOnTop (true);
    centreWithSize (240, 250);
    setVisible (true);
}

SessionImportWizardDialog::~SessionImportWizardDialog() {}

bool SessionImportWizardDialog::escapeKeyPressed()
{
    closeButtonPressed();
    return true;
}

void SessionImportWizardDialog::closeButtonPressed()
{
    holder.reset();
}

} // namespace element
