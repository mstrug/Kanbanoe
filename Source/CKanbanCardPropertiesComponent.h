/*
  ==============================================================================

    PropertiesComponent.h
    Created: 13 Dec 2018 12:00:05am
    Author:  Michał

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


using namespace juce;
//==============================================================================
/*
*/

class CKanbanCardComponent;

class CKanbanCardPropertiesComponent : public juce::Component
{
public:
	CKanbanCardPropertiesComponent(CKanbanCardComponent& aOwner);
    ~CKanbanCardPropertiesComponent();

    void paint (Graphics&) override;
    void resized() override;
	void mouseDown(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	
private:

	void changesApply();

private:

	CKanbanCardComponent& iOwner;

	TextEditor iTextName;
	
	Label iLabel;
	
	TextEditor iTextEditor;
		
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanCardPropertiesComponent)
};
