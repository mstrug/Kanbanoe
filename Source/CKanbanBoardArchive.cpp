/*
  ==============================================================================

    CKanbanBoardArchive.cpp
    Created: 29 Mar 2021 1:35:44am
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanBoardArchive.h"
#include "CKanbanBoard.h"
#include "CKanbanCardComponent.h"
#include "CConfiguration.h"


const int KColId_Id = 1;
const int KColId_Name = 2;
const int KColId_Cards = 3;



//==============================================================================
CKanbanBoardArchive::CKanbanBoardArchive(CKanbanBoardComponent &aKanbanBoard) : iKanbanBoard(aKanbanBoard)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	addAndMakeVisible(iTable);
	iTable.setModel(this);
	iTable.getHeader().addColumn("ID", KColId_Id, 40);
	iTable.getHeader().addColumn("Name", KColId_Name, 150);
	iTable.getHeader().addColumn("Cards", KColId_Cards, 500);
	iTable.getHeader().setSortColumnId(1, true);

	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	iTable.setRowHeight(h + 2 * m + 10);

	//setBounds(0,0, 100, 100);
}

CKanbanBoardArchive::~CKanbanBoardArchive()
{
}

void CKanbanBoardArchive::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

/*    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("CKanbanBoardArchive", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
				*/
}

void CKanbanBoardArchive::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	auto r(getLocalBounds());
	r.reduce(m, m);
	
	iTable.setBounds(r);
	iTable.getHeader().setColumnWidth(KColId_Cards, r.getWidth() - 40 - 150);
}

int CKanbanBoardArchive::getNumRows()
{
	auto& arch = iKanbanBoard.getArchives();
	return arch.size();
}

void CKanbanBoardArchive::paintRowBackground(Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
	auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
		.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
	/*if (rowIsSelected)
		g.fillAll(Colours::lightblue);
	else */
		if (rowNumber % 2)
		g.fillAll(alternateColour);
}

void CKanbanBoardArchive::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	auto& arch = iKanbanBoard.getArchives();

	String text;

	switch (columnId)
	{
	case KColId_Id:
		text = String(arch[rowNumber]->iId);
		break;
	case KColId_Name:
		text = arch[rowNumber]->iName;
		break;
	case KColId_Cards:
		break;
	default:
		break;
	}

	g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
	g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);

}

class CTmp : public Component
{
	FlexBox iLayout;
	OwnedArray <CKanbanCardComponent> iCards;
public:
	CTmp()
	{
		iLayout.alignContent = FlexBox::AlignContent::center;
		iLayout.alignItems = FlexBox::AlignItems::flexStart;
		iLayout.flexDirection = FlexBox::Direction::row;
		iLayout.flexWrap = FlexBox::Wrap::wrap;
		iLayout.justifyContent = FlexBox::JustifyContent::flexStart;
	}
	virtual ~CTmp()
	{
		iCards.clear(true);
	}
	void resized()
	{
		iLayout.performLayout(getLocalBounds());
	}
	void updateSizeFromCards()
	{
		int w = CConfiguration::getIntValue("KanbanCardWidth");
		int h = CConfiguration::getIntValue("KanbanCardHeight");
		int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
		int width = iCards.size() * (w + m) ;
		int height = h + 2 * m;
		setSize(width, height);
	}
	void addCard(CKanbanCardComponent* aCard)
	{
		iCards.add(aCard);
		addAndMakeVisible(aCard);

		int w = CConfiguration::getIntValue("KanbanCardWidth");
		int h = CConfiguration::getIntValue("KanbanCardHeight");
		int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

		FlexItem fi(w, h);
		fi.associatedComponent = aCard;
		fi.margin = m / 2;

		iLayout.items.add(fi);
	}
};

Component * CKanbanBoardArchive::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component * existingComponentToUpdate)
{
	if (columnId == KColId_Id || columnId == KColId_Name) // The ID and Length columns do not have a custom component
	{
		jassert(existingComponentToUpdate == nullptr);
		return nullptr;
	}
	if (columnId == KColId_Cards)
	{
		auto& arch = iKanbanBoard.getArchives();

		Viewport* v = static_cast<Viewport*> (existingComponentToUpdate);

		if (v == nullptr)
		{
			v = new Viewport();
			CTmp* cards = new CTmp();

			for (String& s : arch[rowNumber]->iKanbanCards)
			{
				var d = JSON::parse(s);
				CKanbanCardComponent* card = new CKanbanCardComponent(nullptr);
				card->setupFromArchive(d);
				card->setReadOnly(true);
				cards->addCard(card);
			}

			cards->updateSizeFromCards();
			v->setViewedComponent(cards);
		}

		return v;
	}
	return nullptr;
}
