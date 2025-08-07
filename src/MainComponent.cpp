#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // TODO: have minimum bounds on width/height
    setSize (600, 400);
    m_pluginFormatManager.addDefaultFormats();

    juce::Logger::outputDebugString("---- Checking for available plugin formats:");
    for (const auto* format : m_pluginFormatManager.getFormats())
    {
        juce::Logger::outputDebugString(format->getName());
    }

    addChildComponent(m_descriptionView);
    addAndMakeVisible(m_clearButton);
    addAndMakeVisible(m_copyButton);
    addAndMakeVisible(m_saveButton);

    m_clearButton.onClick = [this]()
    {
        m_state = idle;
    };

    m_copyButton.onClick = [this]()
    {
        const auto copiedString = m_descriptionView.getCurrentDescription();

        if (copiedString.isNotEmpty())
        {
            juce::SystemClipboard::copyTextToClipboard(copiedString);
        }

        // TODO: Tell user their string is copied to clipboard
    };

    m_saveButton.onClick = [this]()
    {
        const auto pluginDescription = m_descriptionView.getCurrentDescription();
        savePluginDescriptionToFile(pluginDescription);
    };

    m_state.addListener(this);
    m_state = idle;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // TODO: Change ui feedback for a drag and drop action
    switch (static_cast<FileDroppedState>(static_cast<int>(m_state.getValue())))
    {
        case idle:
        default:
            g.setColour(juce::Colours::transparentBlack);
            break;
        case dragEnterValid:
            g.setColour(juce::Colours::lightgreen);
            break;
        case dragEnterInvalid:
            g.setColour(juce::Colours::red);
            break;
        case fileDropped:
            g.setColour(juce::Colours::cyan);
            break;
    }

    g.drawRect(getLocalBounds(), 2);

    if (m_state == idle)
    {
        g.setFont (juce::FontOptions (16.0f));
        g.setColour (juce::Colours::white);
        g.drawText("Drag in a plugin to get started!", getLocalBounds(), juce::Justification::centred);
    }
}

void MainComponent::resized()
{
    constexpr int padding = 4;
    auto bounds = getLocalBounds().reduced(padding);
    m_descriptionView.setBounds(bounds);

    constexpr int buttonSize = 75;
    auto buttonArea = bounds.removeFromBottom(buttonSize);

    // Defined from right to left
    const std::vector<std::reference_wrapper<juce::TextButton>> buttonsInBar
    {
        m_clearButton,
        m_saveButton,
        m_copyButton
    };

    // Buttons will overrun bounds
    jassert(static_cast<int>(buttonsInBar.size() * buttonSize) <= bounds.getHeight());

    for (const auto button : buttonsInBar)
    {
        button.get().setBounds(buttonArea.removeFromRight(buttonSize));
        buttonArea.removeFromRight(padding);
    }
}

bool MainComponent::isInterestedInFileDrag(const juce::StringArray &files)
{
    // only handle a single file for now
    if (files.size() > 1 || files.isEmpty())
    {
        m_state = dragEnterInvalid;
        return false;
    }

    const auto fileToDrop = juce::File(files[0]);
    const juce::StringArray availableFormats = { ".vst3", ".component" };

    if (availableFormats.contains(fileToDrop.getFileExtension(), true))
    {
        m_state = dragEnterValid;
        return true;
    }

    m_state = dragEnterInvalid;
    return false;
}

void MainComponent::fileDragEnter(const juce::StringArray &files, int x, int y)
{
    FileDragAndDropTarget::fileDragEnter(files, x, y);
}

void MainComponent::filesDropped(const juce::StringArray &files, int, int)
{
    if (files.isEmpty())
    {
        return;
    }

    m_descriptions.clear();

    // TODO: Provide error handling and user feedback
    m_pluginList.scanAndAddDragAndDroppedFiles(m_pluginFormatManager, files, m_descriptions);

    m_lastDroppedFile = juce::File(files[0]);

    if (const auto* lastDescription = m_descriptions.getLast())
    {
        if (auto xml = lastDescription->createXml())
        {
            m_descriptionView.setDescription(std::move(xml));
        }
    }

    m_state = fileDropped;
}

void MainComponent::fileDragExit(const juce::StringArray &)
{
    m_state = idle;
}

void MainComponent::valueChanged(juce::Value &value)
{
    if (value.refersToSameSourceAs(m_state))
    {
        const auto state = static_cast<FileDroppedState>(static_cast<int>(value.getValue()));

        switch (state)
        {
            case idle:
                m_lastDroppedFile = juce::File();
                m_descriptionView.setVisible(false);
                m_clearButton.setEnabled(false);
                m_copyButton.setEnabled(false);
                m_saveButton.setEnabled(false);
                break;
            case dragEnterValid:
            case dragEnterInvalid:
                break;
            case fileDropped:
                m_descriptionView.setVisible(true);
                m_clearButton.setEnabled(true);
                m_copyButton.setEnabled(true);
                m_saveButton.setEnabled(true);
                break;
        }

        repaint();
    }
}

void MainComponent::savePluginDescriptionToFile(const juce::String &description)
{
    juce::FileChooser fileChooser = {"Save Plugin description",
    juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
    "*.xml"
    };

    if (fileChooser.browseForFileToSave(true))
    {
        const juce::File locationToWrite = fileChooser.getResult();

        if (locationToWrite != juce::File())
        {
            if (locationToWrite.replaceWithText(description))
            {
                // TODO: Tell user that it worked
            }
            else
            {
                // TODO: Tell user that it didn't work
            }
        }
    }
}
