#pragma once
#include <juce_animation/juce_animation.h>
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * @brief A Toaster notification popup - add this as a child component to a top level component (such as an editor)
 *          and set the bounds of where this should appear.
 */
class ToasterNotification final : public juce::Component
{
public:
    ToasterNotification();

    enum ColourIds
    {
        BackgroundColourId = 10,
        TextColourId,
    };

    /**
     * @brief Queue a popup notification which will appear on screen briefly.
     * @param message The message to display
     *
     * TODO: Have "type" input field param for displaying info, error etc.
     */
    void queueNotification (const juce::String& message);

private:
    void paint (juce::Graphics& g) override;

    juce::VBlankAnimatorUpdater m_updater { this };
    const float m_numSecondsToShow { 1750.0f };
    const float m_fadeTimeSeconds { 250.0f };
    float m_alpha { 0.0f };
    juce::String m_popupString;

    // Animation should be quick fade in, hold for x seconds, then fade out.
    juce::Animator m_fadeIn =
        juce::ValueAnimatorBuilder {}
            .withOnStartCallback ([this]() { setVisible (true); })
            .withEasing (juce::Easings::createEaseIn())
            .withValueChangedCallback (
                [this] (const float value)
                {
                    m_alpha = value;
                    repaint();
                })
            .build();

    juce::Animator m_fadeOut = juce::ValueAnimatorBuilder {}
                                   .withEasing (juce::Easings::createEaseIn())
                                   .withValueChangedCallback (
                                       [this] (const float value)
                                       {
                                           m_alpha = 1.0f - value;
                                           repaint();
                                       })
                                   .build();

    juce::Animator m_fullAnimation =
        juce::AnimatorSetBuilder { m_fadeIn }
            .togetherWith (m_fadeIn)
            .followedBy (m_numSecondsToShow)
            .followedBy (m_fadeOut)
            .togetherWith (m_fadeTimeSeconds)
            .followedBy ([this]() { setVisible (false); })
            .build();
};
