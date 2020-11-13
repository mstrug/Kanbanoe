/*
  ==============================================================================

    PropertiesComponent.cpp
    Created: 13 Dec 2018 12:00:05am
    Author:  Michał

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardPropertiesComponent.h"
#include "CKanbanCardComponent.h"


//==============================================================================
CKanbanCardPropertiesComponent::CKanbanCardPropertiesComponent(CKanbanCardComponent &aOwner) : iOwner(aOwner)
{
	setViewportIgnoreDragFlag(true);

	int w = 300;

	//iBar->getProperties().set("name", "test");
	addAndMakeVisible(iTextName);
	iTextName.setBounds(10, 10, w, 24);
	iTextName.setSelectAllWhenFocused(true);
	iTextName.setText(aOwner.name);
	iTextName.onTextChange = [this]
	{
		this->changesApply();
	};
	iTextName.onReturnKey = [this]
	{
//		this->iBar->deselect();
//		this->iController->closePropertiesCallout();
		this->getParentComponent()->exitModalState(0);
	};

	int yofs = iTextName.getBottom() + 12;

	iLabel.setText("notes", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	iLabel.setBounds(10, yofs, 50, 24);
	
	iTextEditor.setMultiLine(true);
	iTextEditor.setReturnKeyStartsNewLine(true);
	iTextEditor.setScrollbarsShown(true);	
	//iTextEditor.setText(vp[p].toString());
	addAndMakeVisible(iTextEditor);
	iTextEditor.setBounds(10 + 50 + 10, yofs, w - 50 - 10, 24 + 24 * 2);
	iTextEditor.onTextChange = [this]
	{
		// todo
	};
	iTextEditor.onReturnKey = [this]
	{
		//this->iBar->deselect();
		//this->iController->closePropertiesCallout();
		this->getParentComponent()->exitModalState(0);
	};

	
	setSize(w+20, yofs + 24+48 +12);
}

CKanbanCardPropertiesComponent::~CKanbanCardPropertiesComponent()
{
}

void CKanbanCardPropertiesComponent::paint (Graphics& g)
{
}

void CKanbanCardPropertiesComponent::resized()
{
}

void CKanbanCardPropertiesComponent::mouseDown(const MouseEvent& event)
{
}

void CKanbanCardPropertiesComponent::mouseUp(const MouseEvent& event)
{
}

void CKanbanCardPropertiesComponent::changesApply()
{
	/*iBar->getProperties().set("name", iTextName.getText());
	iBar->setColour(this->iColours->getSelectedColourIdx());
	iBar->repaint();*/

	iOwner.name = iTextName.getText();
	iOwner.repaint();
}

