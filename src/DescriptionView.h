#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

class DescriptionView final : public juce::Component {

    public:
    DescriptionView() = default;

    /**
     * Set the plugin description object associated with this component. NOTE: This will take ownership of the plugin description
     * @param description
     */
    void setDescription(std::unique_ptr<juce::XmlElement> description);

    /**
     * @return Grab the currently loaded plugin description - NOTE: May be empty
     */
    juce::String getCurrentDescription() const;

private:
    void paint (juce::Graphics&) override;

    juce::AttributedString convertDescriptionToText();

    juce::AttributedString m_formattedDescription;
    std::unique_ptr<juce::XmlElement> m_description;
};