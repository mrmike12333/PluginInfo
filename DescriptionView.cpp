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

    const juce::String fullString = m_description->toString();
    juce::Logger::outputDebugString(fullString);

    // TODO: Change to a nicer colourScheme + introduce look and feel
    const juce::Colour nameColour = juce::Colours::white;
    const juce::Colour valueColour = juce::Colours::white.withAlpha(0.8f);
    constexpr auto fontSize = 20.0f; // TODO: Make font size variable
    const juce::Font nameFont = juce::FontOptions(fontSize, juce::Font::bold);
    const juce::Font valueFont = juce::FontOptions(fontSize, juce::Font::bold);

    const int numAttributes = m_description->getNumAttributes();
    for (int i = 0; i < numAttributes; ++i)
    {
        const juce::String attributeName = m_description->getAttributeName(i);
        const juce::String attributeValue = m_description->getAttributeValue(i);

        juce::String formattedValue = attributeValue;
        // True/False flags replace 0 and 1 (but not num ins/outs!)
        if (!attributeName.contains("num"))
        {
            if (formattedValue == "1")
            {
                formattedValue = "True";
            } else if (formattedValue == "0")
            {
                formattedValue = "False";
            }
        }

        formattedString.append(attributeName + " : ", nameFont, nameColour);
        formattedString.append(formattedValue + "\n", valueFont, valueColour);
    }

    return formattedString;
}
