#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    switch (m_state)
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

    g.setFont (juce::FontOptions (16.0f));
    g.setColour (juce::Colours::white);

    const juce::String textToDraw = m_lastDroppedFile != juce::File() ?
        m_lastDroppedFile.getFullPathName() : "Drag in a plugin to get started!";

    g.drawText (textToDraw, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{

}

bool MainComponent::isInterestedInFileDrag(const juce::StringArray &files)
{
    // only handle a single file for now
    if (files.size() > 1 || files.isEmpty())
    {
        repaint();
        m_state = dragEnterInvalid;
        return false;
    }

    const auto fileToDrop = juce::File(files[0]);
    const juce::StringArray availableFormats = {".vst3", ".dll", ".component", ".so", ".lv2"};

    if (availableFormats.contains(fileToDrop.getFileExtension(), true))
    {
        m_state = dragEnterValid;
        repaint();
        return true;
    }

    m_state = dragEnterInvalid;
    repaint();
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

    m_state = fileDropped;

    // TODO: Replace with load file info logic
    m_lastDroppedFile = juce::File(files[0]);
    repaint();
}

void MainComponent::fileDragExit(const juce::StringArray &)
{
    m_state = idle;
    repaint();
}
