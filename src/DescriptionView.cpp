#include "DescriptionView.h"

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

juce::String DescriptionView::getCurrentDescription() const
{
    juce::String returnString;

    if (!m_description)
    {
        return returnString;
    }

    returnString = m_description->toString();

    return returnString;
}

void DescriptionView::paint(juce::Graphics &g)
{
    m_formattedDescription.draw(g, getLocalBounds().toFloat());
}

juce::AttributedString DescriptionView::convertDescriptionToText() const
{
    juce::AttributedString formattedString;

    if (!m_description)
    {
        return formattedString;
    }

    const juce::String fullString = m_description->toString();
    juce::Logger::outputDebugString(fullString);

    // TODO: Change to a nicer colourScheme + introduce look and feel
    const juce::Colour nameColour = findColour(juce::Label::ColourIds::textWhenEditingColourId);
    const juce::Colour valueColour = findColour(juce::Label::ColourIds::textColourId);
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
        if (attributeName.containsIgnoreCase("time"))
        {
            if (attributeValue == "0")
            {
                // No use in showing 0 time.
                continue;
            }

            // Time is in hex - convert
            const auto time = attributeValue.getHexValue64();
            const juce::Time timeMs {time};
            formattedValue = timeMs.formatted("%H:%M: %Y/%m/%d");
        }
        else if (!attributeName.containsIgnoreCase("num"))
        {
            // Non-numerical values can be parsed as True/False
            if (formattedValue == "1")
            {
                formattedValue = "True";
            } else if (formattedValue == "0")
            {
                formattedValue = "False";
            }
        }

        formattedString.append(attributeName + ": ", nameFont, nameColour);
        formattedString.append(formattedValue + "\n", valueFont, valueColour);
    }

    return formattedString;
}
