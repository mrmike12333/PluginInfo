#include "ToasterNotification.h"

ToasterNotification::ToasterNotification()
{
    setInterceptsMouseClicks(false, false);
    setAlwaysOnTop(true);
    m_updater.addAnimator(m_fullAnimation);
}

void ToasterNotification::queueNotification(const juce::String &message)
{
    m_popupString = message;
    m_fullAnimation.start();
}

void ToasterNotification::paint(juce::Graphics &g)
{
    g.setColour(findColour(ColourIds::BackgroundColourId).withAlpha(m_alpha));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);

    g.setColour(findColour(ColourIds::TextColourId).withAlpha(m_alpha));
    g.drawFittedText(m_popupString, getLocalBounds(), juce::Justification::centred, 10);
}
