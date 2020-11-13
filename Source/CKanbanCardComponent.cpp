/*
  ==============================================================================

    CKanbanCardComponent.cpp
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "Cconfiguration.h"
#include "CKanbanCardPropertiesComponent.h"


//==============================================================================
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnContentComponent* aOwner) : iIsDragging(false), iFlexItem(nullptr), iOwner(aOwner)
{
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	//iEditButton.setButtonText("Edit");
	//addAndMakeVisible(iEditButton);

	setSize(w, h);
}

CKanbanCardComponent::~CKanbanCardComponent()
{
}

void CKanbanCardComponent::paint (juce::Graphics& g)
{
	if (iIsDragging) return;
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

    g.setColour (juce::Colours::grey);
//	g.drawRect(getLocalBounds().reduced(m/2), 1);   // draw an outline around the component
	g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText (name.length() == 0 ? "CKanbanCardComponent" : name, getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void CKanbanCardComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

	//iEditButton.setBounds(getBounds().removeFromRight(30));
}

void CKanbanCardComponent::mouseDown(const MouseEvent& event)
{
	/*if (event.mods.isLeftButtonDown())
	{
		//iDragger.startDraggingComponent(this, event);
		if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
		{
			dragContainer->startDragging(KanbanCardComponentDragDescription, this);
			iIsDragging = true;
			repaint();
		}
	}*/
}

void CKanbanCardComponent::mouseDrag(const MouseEvent& event)
{
	//iDragger.dragComponent(this, event, nullptr);
	if ( !iIsDragging && event.mods.isLeftButtonDown())
	{
		//iDragger.startDraggingComponent(this, event);
		if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
		{
			dragContainer->startDragging(KanbanCardComponentDragDescription, this);
			iIsDragging = true;
			repaint();
		}
	}
}

void CKanbanCardComponent::mouseUp(const MouseEvent& event)
{
	if (iIsDragging)
	{
		iIsDragging = false;
		repaint();
	}
	else if (event.mods.isLeftButtonDown())
	{
		showProperties();

		/*PopupMenu menu;
		menu.addItem("Edit card", [&]()
		{
			auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
			CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
			box->setAlwaysOnTop(true);
		});
		menu.addItem("Remove card", [&]()
		{
		});
		menu.show();*/
	}
}

void CKanbanCardComponent::mouseDoubleClick(const MouseEvent& event)
{
	Logger::outputDebugString("DB");
}

CKanbanColumnContentComponent* CKanbanCardComponent::getOwner()
{
	return iOwner;
}

void CKanbanCardComponent::setOwner(CKanbanColumnContentComponent* aOwner)
{
	iOwner = aOwner;
}

void CKanbanCardComponent::showProperties()
{
	auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
	CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
	box->setAlwaysOnTop(true);
}



