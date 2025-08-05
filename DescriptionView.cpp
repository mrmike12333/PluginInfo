#include "DescriptionView.h"

DescriptionView::DescriptionView()
{
}

void DescriptionView::setDescription(std::unique_ptr<juce::XmlElement> description)
{
    if (!description)
    {
        jassertfalse;
        return;
    }

    m_description = std::move(description);

    m_formattedDescription = convertDescriptionToText();
}

void DescriptionView::paint(juce::Graphics &g)
{
    g.setFont(juce::FontOptions(15.0f));
    g.setColour(juce::Colours::white);

    m_formattedDescription.draw(g, getLocalBounds().toFloat());
}

juce::AttributedString DescriptionView::convertDescriptionToText()
{
    juce::AttributedString formattedString;

    if (!m_description)
    {
        return formattedString;
    }

    // TODO: Traverse the tree with the string and xml to format the attributed string.
    const juce::String fullString = m_description->toString();
    formattedString.setText(fullString);

    juce::Logger::outputDebugString(fullString);

    return formattedString;
}
