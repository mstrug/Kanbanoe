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
CKanbanCardPropertiesComponent::CKanbanCardPropertiesComponent(CKanbanCardComponent &aOwner) : iOwner(aOwner), iMaximized(false), iUpdated(false), iButtonUrl("Url", DrawableButton::ImageRaw)
{
	setViewportIgnoreDragFlag(true);
	addKeyListener(this);

	//iBar->getProperties().set("name", "test");
	addAndMakeVisible(iTextName);
	iTextName.addKeyListener(this);
	iTextName.setSelectAllWhenFocused(true);
	iTextName.setText(aOwner.getText());
	iTextName.onTextChange = [this]
	{
		this->changesApply();
	};

	iLabel.setText("Notes:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	
	iTextEditor.setMultiLine(true);
	iTextEditor.addKeyListener(this);
	iTextEditor.setReturnKeyStartsNewLine(true);
	iTextEditor.setScrollbarsShown(true);	
	iTextEditor.setText( aOwner.getNotes() );
	addAndMakeVisible(iTextEditor);
	iTextEditor.onTextChange = [this, &aOwner]
	{
		this->iUpdated = true;
		aOwner.setNotes(this->iTextEditor.getText());
	};

	iLabelAssigne.setText("Assignee:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelAssigne);

	addAndMakeVisible(iTextAssigne);
	iTextAssigne.addKeyListener(this);
	iTextAssigne.setText(aOwner.getAssigne());
	iTextAssigne.onTextChange = [this]
	{
		this->changesApply();
	};

	iLabelUrl.setText("Url:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelUrl);

	addAndMakeVisible(iTextUrl);
	iTextUrl.addKeyListener(this);
	//iTextUrl.setSelectAllWhenFocused(true);
	iTextUrl.setText(aOwner.getProperties()["url"]);
	iTextUrl.setSelectAllWhenFocused(true);
	iTextUrl.setScrollToShowCursor(true);
	iTextUrl.onTextChange = [this]
	{
		//iTextUrl.scroll
		this->changesApply();
	};
	iButtonUrl.onClick = [this]
	{
		URL u(iTextUrl.getText());
		if (u.isWellFormed())
		{
			u.launchInDefaultBrowser();
		}
	};
	addAndMakeVisible(iButtonUrl);

	iLabelTags.setText("Tags:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelTags);

	addAndMakeVisible(iTextTags);
	iTextTags.addKeyListener(this);
	//iTextTags.setSelectAllWhenFocused(true);
	iTextTags.setText(aOwner.getProperties()["tags"]);
	iTextTags.onTextChange = [this]
	{
		this->changesApply();
	};

	addAndMakeVisible(iLabelSlider);

	iSliderDueDate.setRange(-1, 365, 1);
	iSliderDueDate.setSliderStyle(Slider::IncDecButtons);
	iSliderDueDate.setIncDecButtonsMode(Slider::incDecButtonsDraggable_Horizontal); // incDecButtonsDraggable_AutoDirection);
	//iSliderDueDate.setTextBoxStyle(Slider::TextBoxLeft, true, 50, 20);
	iSliderDueDate.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
//	iSliderDueDate.setBounds(10, yofs, w - 10, 24);
	//iSliderDueDate.setVelocityBasedMode(true);
	//iSliderDueDate.setVelocityModeParameters(2, 1, 0);
	iSliderDueDate.setTextValueSuffix(" days");
	addAndMakeVisible(iSliderDueDate);
	if (!aOwner.isDueDateSet())
	{
		iSliderDueDate.setValue(-1, dontSendNotification);
		this->iLabelSlider.setText("Due date: -", dontSendNotification);
	}
	else
	{
		iSliderDueDate.setValue((aOwner.getDueDate().toMilliseconds() - aOwner.getCreationDate().toMilliseconds()) / (24 * 3600000), dontSendNotification);
		iLabelSlider.setText(aOwner.getDueDate().formatted("Due date: %d.%m.%Y %a"), dontSendNotification);
	}
	//this->setDueDate(-1);
	//iSliderDueDate.setValue()
	iSliderDueDate.onValueChange = [this]
	{
		this->iUpdated = true;
		auto sv = this->iSliderDueDate.getValue();
		this->setDueDate(sv);
	};

	iColours.reset(new ColoursComponent( 6, 1, CConfiguration::getColourPalette(), CConfiguration::getColourPalette().getColourIndex( aOwner.getColour() )));
	addAndMakeVisible(*iColours);
	iColours->setListener(this);

	//iLabelCreationDate.setText(aOwner.getCreationDate().toISO8601(true) + "  " + aOwner.getLastUpdateDate().toISO8601(true), dontSendNotification);
	iLabelCreationDate.setText(aOwner.getCreationDate().formatted("Created: %d.%m.%Y %H:%M    ") + aOwner.getLastUpdateDate().formatted("Updated: %d.%m.%Y %H:%M"), dontSendNotification);
	iLabelCreationDate.setColour(Label::textColourId, getLookAndFeel().findColour(Label::textColourId).darker());
	addAndMakeVisible(iLabelCreationDate);

	String customDatesText;
	int64 remoteLastUpdateDate = aOwner.getProperties()["remoteLastUpdateDate"].toString().getLargeIntValue();
	if (remoteLastUpdateDate > 0) 
	{
		customDatesText += Time(remoteLastUpdateDate).formatted("Remote update: %d.%m.%Y %H:%M    ");
	}
	int64 userReviewAddedDate = aOwner.getProperties()["userReviewAddedDate"].toString().getLargeIntValue();
	if (userReviewAddedDate > 0)
	{
		customDatesText += Time(userReviewAddedDate).formatted("Review: %d.%m.%Y %H:%M");
	}	
	iLabelCustomDates.setText(customDatesText, dontSendNotification);
	iLabelCustomDates.setColour(Label::textColourId, getLookAndFeel().findColour(Label::textColourId).darker());
	addChildComponent(iLabelCustomDates);

	iButtonMaximize.setButtonText("=");
	iButtonMaximize.setWantsKeyboardFocus(false);
	iButtonMaximize.onClick = [this]
	{
		if (this->iMaximized)
		{
			this->iButtonMaximize.setButtonText("=");
			this->iMaximized = false;
			this->iLabelCustomDates.setVisible(false);
		}
		else
		{
			this->iButtonMaximize.setButtonText("-");
			this->iMaximized = true;
			this->iLabelCustomDates.setVisible(true);
		}
		this->layout();
	};
	addAndMakeVisible(iButtonMaximize);

	layout();
}


void CKanbanCardPropertiesComponent::layout()
{
	int w = 370;
	int wl = 65;
	int wm = 20;

	if (iMaximized)
	{
		w = 800;
	}

	iTextName.setBounds(10, 10, w - 30, 24);
	iButtonMaximize.setBounds(iTextName.getRight() + 10, 12, 20, 20);

	int yofs = iTextName.getBottom() + 12;

	iLabel.setBounds(10, yofs, wl, 24);
	iTextEditor.setBounds(10 + wl + 10, yofs, w - wl - 10, 24 + 24 * (iMaximized ? 12 : 3 ));

	yofs = iTextEditor.getBottom() + 12;

	iLabelAssigne.setBounds(10, yofs, wl + 10, 24);
	iTextAssigne.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);

	yofs = iTextAssigne.getBottom() + 12;

	iLabelUrl.setBounds(10, yofs, wl, 24);
	iTextUrl.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);
	iButtonUrl.setBounds(10 + wl - 25, yofs, 24, 24);
	DrawablePath btnImg;
	auto p = CKanbanCardComponent::generateUrlPath(Rectangle<int>(5, 4, 15, 15));
	btnImg.setPath(p);
	PathStrokeType pt(2, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::butt);
	btnImg.setStrokeType(pt);
	btnImg.setStrokeFill(FillType(Colours::grey));
	DrawablePath btnImgOver(btnImg);
	btnImgOver.setStrokeFill(FillType(Colours::lightgrey));
	DrawablePath btnImgPushed(btnImgOver);
	btnImgPushed.setStrokeFill(FillType(Colours::darkgrey));
	iButtonUrl.setImages(&btnImg, &btnImgOver, &btnImgPushed);

	yofs = iTextUrl.getBottom() + 12;

	iLabelTags.setBounds(10, yofs, wl, 24);
	iTextTags.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);

	yofs = iTextTags.getBottom() + 12;

	int slw = 180;
	iLabelSlider.setBounds(10, yofs, slw, 24);
	iSliderDueDate.setBounds(10 + slw + 10, yofs - 3, w - slw - 10, 30);

	yofs = iSliderDueDate.getBottom() + 12;

	iColours->setTopLeftPosition(w + wm - iColours->getWidth() - 10, yofs);

	yofs = iColours->getBottom() + 12;

	if (iMaximized)
	{
		iLabelCreationDate.setBounds(10, yofs, w / 2 - 10, 20);
		iLabelCustomDates.setBounds(w / 2, yofs, w - 10, 20);
	} 
	else
	{
		iLabelCreationDate.setBounds(10, yofs, w - 10, 20);

	}

	yofs = iLabelCreationDate.getBottom() + 12;


	setSize(w + wm, yofs);
}

CKanbanCardPropertiesComponent::~CKanbanCardPropertiesComponent()
{
	iOwner.deselect();
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

bool CKanbanCardPropertiesComponent::wasContentUpdated()
{
	return iUpdated;
}

void CKanbanCardPropertiesComponent::ColorChanged(int aSelectedColorIdx)
{
	iUpdated = true;
	iOwner.setColour(CConfiguration::getColourPalette().getColor(aSelectedColorIdx));
}

bool CKanbanCardPropertiesComponent::keyPressed(const KeyPress & key, Component * originatingComponent)
{
	if (key == key.escapeKey )
	{
		getParentComponent()->exitModalState(0);
		return true;
	}
	else if (!iMaximized && key == key.returnKey && originatingComponent != &iTextEditor)
	{
		getParentComponent()->exitModalState(0);
		return true;
	}
	return false;
}

void CKanbanCardPropertiesComponent::changesApply()
{
	/*iBar->getProperties().set("name", iTextName.getText());
	iBar->setColour(this->iColours->getSelectedColourIdx());
	iBar->repaint();*/

	iUpdated = true;

	iOwner.setTags(iTextTags.getText());
	iOwner.setUrl(iTextUrl.getText());
	iOwner.setText(iTextName.getText());
	iOwner.setAssigne(iTextAssigne.getText());
	iOwner.repaint();
}

void CKanbanCardPropertiesComponent::setDueDate(double aSliderVal)
{
	if (aSliderVal < 0)
	{
		this->iLabelSlider.setText("Due date: -", dontSendNotification);
		this->iOwner.setDueDate(false, Time::getCurrentTime() - RelativeTime(24 * 3600));
	}
	else
	{
		auto d = this->iOwner.getCreationDate();
		RelativeTime rt(aSliderVal * 24 * 3600);
		d += rt;
		this->iLabelSlider.setText(d.formatted("Due date: %d.%m.%Y %a"), dontSendNotification);
		this->iOwner.setDueDate(true, d);
	}
}

