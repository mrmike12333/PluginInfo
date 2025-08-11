#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
    m_pluginFormatManager.addDefaultFormats();

    juce::Logger::outputDebugString (
        "---- Checking for available plugin formats: ----");
    for (const auto* format : m_pluginFormatManager.getFormats())
    {
        juce::Logger::outputDebugString (format->getName());
    }

    setupLookAndFeel();

    addChildComponent (m_toasterNotification);
    addAndMakeVisible (m_idleInfo);
    addChildComponent (m_descriptionView);
    addAndMakeVisible (m_clearButton);
    addAndMakeVisible (m_copyButton);
    addAndMakeVisible (m_saveButton);

    m_clearButton.onClick = [this]() { m_state = idle; };

    m_copyButton.onClick = [this]()
    {
        const auto copiedString = m_descriptionView.getCurrentDescription();

        if (copiedString.isNotEmpty())
        {
            juce::SystemClipboard::copyTextToClipboard (copiedString);
        }

        m_toasterNotification.queueNotification (
            "Copied plugin info to clipboard");
    };

    m_saveButton.onClick = [this]()
    {
        const auto pluginDescription =
            m_descriptionView.getCurrentDescription();
        savePluginDescriptionToFile (pluginDescription);
    };

    m_idleInfo.setFont (juce::FontOptions (20.0f));
    m_idleInfo.setText ("Drag in a plugin to get started",
                        juce::dontSendNotification);
    m_idleInfo.setJustificationType (juce::Justification::centred);

    m_state.addListener (this);
    m_state = idle;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (
        juce::ResizableWindow::backgroundColourId));

    // TODO: Change ui feedback for a drag and drop action
    switch (
        static_cast<FileDroppedState> (static_cast<int> (m_state.getValue())))
    {
        case idle:
        case fileDropped:
        default:
            g.setColour (juce::Colours::transparentBlack);
            break;
        case dragEnterValid:
            g.setColour (juce::Colours::lightgreen);
            break;
        case dragEnterInvalid:
            g.setColour (juce::Colours::red);
            break;
    }

    g.drawRect (getLocalBounds(), 2);
}

void MainComponent::resized()
{
    constexpr int padding = 5;
    auto bounds = getLocalBounds().reduced (padding);
    m_idleInfo.setBounds (bounds);
    m_descriptionView.setBounds (bounds);

    m_toasterNotification.setBounds (bounds.withSizeKeepingCentre (
        static_cast<int> (static_cast<float> (getWidth()) * 0.66f),
        static_cast<int> (static_cast<float> (getHeight()) * 0.25f)));

    constexpr int buttonSize = 75;
    auto buttonArea = bounds.removeFromBottom (buttonSize);

    // Defined from right to left
    const std::vector<std::reference_wrapper<juce::TextButton>> buttonsInBar {
        m_clearButton, m_saveButton, m_copyButton
    };

    // Buttons will overrun bounds
    jassert (static_cast<int> (buttonsInBar.size() * buttonSize)
             <= bounds.getHeight());

    for (const auto button : buttonsInBar)
    {
        button.get().setBounds (buttonArea.removeFromRight (buttonSize));
        buttonArea.removeFromRight (padding);
    }
}

bool MainComponent::isInterestedInFileDrag (const juce::StringArray& files)
{
    // only handle a single file for now
    if (files.size() > 1 || files.isEmpty())
    {
        m_state = dragEnterInvalid;
        return false;
    }

    const auto fileToDrop = juce::File (files[0]);
    const juce::StringArray availableFormats = { ".vst3", ".component" };

    if (availableFormats.contains (fileToDrop.getFileExtension(), true))
    {
        m_state = dragEnterValid;
        return true;
    }

    m_state = dragEnterInvalid;
    return false;
}

void MainComponent::fileDragEnter (const juce::StringArray& files, int x, int y)
{
    FileDragAndDropTarget::fileDragEnter (files, x, y);
}

void MainComponent::filesDropped (const juce::StringArray& files, int, int)
{
    if (files.isEmpty())
    {
        return;
    }

    m_descriptions.clear();

    // TODO: Provide error handling and user feedback
    m_pluginList.scanAndAddDragAndDroppedFiles (
        m_pluginFormatManager, files, m_descriptions);

    if (m_descriptions.isEmpty())
    {
        // Loading the plugin failed - TODO: Make this a lambda callback
        m_toasterNotification.queueNotification ("Error: Plugin load failed");
        m_state = idle;
        return;
    }

    m_lastDroppedFile = juce::File (files[0]);

    if (const auto* lastDescription = m_descriptions.getLast())
    {
        if (auto xml = lastDescription->createXml())
        {
            m_descriptionView.setDescription (std::move (xml));
        }
    }
    else
    {
        // Loading the plugin failed
        m_toasterNotification.queueNotification ("Error: Plugin load failed");
        m_state = idle;
        return;
    }

    m_state = fileDropped;
}

void MainComponent::fileDragExit (const juce::StringArray&) { m_state = idle; }

void MainComponent::valueChanged (juce::Value& value)
{
    if (value.refersToSameSourceAs (m_state))
    {
        const auto state =
            static_cast<FileDroppedState> (static_cast<int> (value.getValue()));

        switch (state)
        {
            case idle:
                m_idleInfo.setVisible (true);
                m_lastDroppedFile = juce::File();
                m_descriptionView.setVisible (false);
                m_clearButton.setEnabled (false);
                m_copyButton.setEnabled (false);
                m_saveButton.setEnabled (false);
                break;
            case dragEnterValid:
            case dragEnterInvalid:
                break;
            case fileDropped:
                m_idleInfo.setVisible (false);
                m_descriptionView.setVisible (true);
                m_clearButton.setEnabled (true);
                m_copyButton.setEnabled (true);
                m_saveButton.setEnabled (true);
                break;
        }

        repaint();
    }
}

void MainComponent::setupLookAndFeel() const
{
    auto& lnf = getLookAndFeel();
    const juce::Colour background = juce::Colour::fromRGB (125, 145, 126);
    const juce::Colour backgroundDark = juce::Colour::fromRGB (84, 112, 86);
    const juce::Colour backgroundLight = juce::Colour::fromRGB (187, 198, 188);
    const juce::Colour primary = juce::Colour::fromRGB (255, 255, 255);
    const juce::Colour textMuted = juce::Colour::fromRGB (214, 210, 210);

    // Apply colours
    lnf.setColour (juce::ResizableWindow::ColourIds::backgroundColourId,
                   background);

    lnf.setColour (juce::TextButton::ColourIds::buttonColourId,
                   backgroundLight);
    lnf.setColour (juce::TextButton::ColourIds::textColourOffId,
                   backgroundDark);
    lnf.setColour (juce::TextButton::ColourIds::textColourOnId, backgroundDark);
    lnf.setColour (juce::ComboBox::outlineColourId, backgroundDark);

    lnf.setColour (ToasterNotification::ColourIds::BackgroundColourId,
                   backgroundDark);
    lnf.setColour (ToasterNotification::ColourIds::TextColourId, textMuted);

    // Piggybacking off existing colour IDs for description view
    lnf.setColour (juce::Label::ColourIds::textColourId, textMuted);
    lnf.setColour (juce::Label::ColourIds::textWhenEditingColourId, primary);
}

void MainComponent::savePluginDescriptionToFile (
    const juce::String& description)
{
    juce::FileChooser fileChooser = { "Save Plugin description",
                                      juce::File::getSpecialLocation (
                                          juce::File::userDocumentsDirectory),
                                      "*.xml" };

    if (fileChooser.browseForFileToSave (true))
    {
        const juce::File locationToWrite = fileChooser.getResult();

        if (locationToWrite != juce::File())
        {
            if (locationToWrite.replaceWithText (description))
            {
                m_toasterNotification.queueNotification (
                    "Saved plugin info to:\n"
                    + locationToWrite.getFullPathName());
            }
            else
            {
                m_toasterNotification.queueNotification (
                    "Error: failed to save file");
            }
        }
    }
}
