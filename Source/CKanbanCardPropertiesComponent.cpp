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
#include "CConfiguration.h"
#include "ColourPalette.h"


//==============================================================================
CKanbanCardPropertiesComponent::CKanbanCardPropertiesComponent(CKanbanCardComponent &aOwner) : iOwner(aOwner)
{
	setViewportIgnoreDragFlag(true);

	int w = 300;
	int wm = 20;

	//iBar->getProperties().set("name", "test");
	addAndMakeVisible(iTextName);
	iTextName.setBounds(10, 10, w, 24);
	iTextName.setSelectAllWhenFocused(true);
	iTextName.setText(aOwner.getText());
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
	iTextEditor.setText( aOwner.getNotes() );
	addAndMakeVisible(iTextEditor);
	iTextEditor.setBounds(10 + 50 + 10, yofs, w - 50 - 10, 24 + 24 * 2);
	iTextEditor.onTextChange = [this, &aOwner]
	{
		aOwner.setNotes(this->iTextEditor.getText());
	};
	iTextEditor.onReturnKey = [this]
	{
		//this->iBar->deselect();
		//this->iController->closePropertiesCallout();
		this->getParentComponent()->exitModalState(0);
	};

	yofs = iTextEditor.getBottom() + 12;

	iColours.reset(new ColoursComponent( 6, 1, CConfiguration::getColourPalette(), CConfiguration::getColourPalette().getColourIndex( aOwner.getColour() )));
	addAndMakeVisible(*iColours);
	iColours->setTopLeftPosition( w + wm - iColours->getWidth() - 10, yofs);
	iColours->setListener(this);

	yofs = iColours->getBottom() + 12;

	setSize(w + wm, yofs);
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

void CKanbanCardPropertiesComponent::ColorChanged(int aSelectedColorIdx)
{
	iOwner.setColour(CConfiguration::getColourPalette().getColor(aSelectedColorIdx));
}

void CKanbanCardPropertiesComponent::changesApply()
{
	/*iBar->getProperties().set("name", iTextName.getText());
	iBar->setColour(this->iColours->getSelectedColourIdx());
	iBar->repaint();*/

	iOwner.setText(iTextName.getText());
	iOwner.repaint();
}

