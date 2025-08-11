#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "DescriptionView.h"
#include "ToasterNotification.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final : public juce::Component, public juce::FileDragAndDropTarget, public juce::Value::Listener
{
public:
    MainComponent();

    enum FileDroppedState
    {
        idle,
        dragEnterValid,
        dragEnterInvalid,
        fileDropped,
    };

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    bool isInterestedInFileDrag(const juce::StringArray &files) override;

    void fileDragEnter(const juce::StringArray &files, int x, int y) override;

    void filesDropped(const juce::StringArray &files, int x, int y) override;

    void fileDragExit(const juce::StringArray &files) override;

    void valueChanged(juce::Value &value) override;

    void setupLookAndFeel();

    void savePluginDescriptionToFile(const juce::String &description);

    // Plugin format functionality
    juce::KnownPluginList m_pluginList;
    juce::OwnedArray<juce::PluginDescription> m_descriptions;
    juce::AudioPluginFormatManager m_pluginFormatManager;

    juce::Label m_idleInfo;
    DescriptionView m_descriptionView;
    juce::TextButton m_clearButton{"Clear"};
    juce::TextButton m_copyButton{"Copy"};
    juce::TextButton m_saveButton{"Save"};
    ToasterNotification m_toasterNotification;

    juce::File m_lastDroppedFile;
    juce::Value m_state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
