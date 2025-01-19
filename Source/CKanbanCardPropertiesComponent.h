/*
  ==============================================================================

    PropertiesComponent.h
    Created: 13 Dec 2018 12:00:05am
    Author:  Michał

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColoursComponent.h"

using namespace juce;
//==============================================================================
/*
*/

class CKanbanCardComponent;

class CKanbanCardPropertiesComponent : public juce::Component, public ColoursComponentListener, public KeyListener
{
public:
	CKanbanCardPropertiesComponent(CKanbanCardComponent& aOwner);
    ~CKanbanCardPropertiesComponent();

    void paint (Graphics&) override;
    void resized() override;
	void mouseDown(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;

	bool wasContentUpdated();
	
public: // from ColoursComponentListener

	void ColorChanged(int aSelectedColorIdx);

public: // from KeyListener

	bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

private:

	void layout();

	void changesApply();

	void setDueDate( double aSliderVal );

private:

	CKanbanCardComponent& iOwner;

	TextEditor iTextName;
	Label iLabel;
	
	TextEditor iTextEditor;

	TextEditor iTextUrl;
	Label iLabelUrl;
	DrawableButton iButtonUrl;

	TextEditor iTextTags;
	Label iLabelTags;

	TextEditor iTextAssigne;
	Label iLabelAssigne;

	Label iLabelCreationDate;
	Label iLabelCustomDates;

	Slider iSliderDueDate;
	Label iLabelSlider;

	std::unique_ptr<ColoursComponent> iColours;

	TextButton iButtonMaximize;

	bool iMaximized;

	bool iUpdated;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanCardPropertiesComponent)
};
