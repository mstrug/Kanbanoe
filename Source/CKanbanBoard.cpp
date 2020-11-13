/*
  ==============================================================================

    CKanbanBoard.cpp
    Created: 12 Nov 2020 4:50:24pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanBoard.h"
#include "CConfiguration.h"

//==============================================================================
CKanbanBoardComponent::CKanbanBoardComponent()
{
}

CKanbanBoardComponent::~CKanbanBoardComponent()
{
}

void CKanbanBoardComponent::createDefaultBoard()
{
	iKanbanColumns.add(new CKanbanColumnComponent(1, "Backlog", *this));
	addAndMakeVisible(iKanbanColumns[0]);
	iKanbanColumns.add(new CKanbanColumnComponent(2, "To Do", *this));
	addAndMakeVisible(iKanbanColumns[1]);
	iKanbanColumns.add(new CKanbanColumnComponent(3, "In progress", *this));
	addAndMakeVisible(iKanbanColumns[2]);
	iKanbanColumns.add(new CKanbanColumnComponent(4, "Blocked", *this));
	addAndMakeVisible(iKanbanColumns[3]);
	iKanbanColumns.add(new CKanbanColumnComponent(5, "Done", *this));
	addAndMakeVisible(iKanbanColumns[4]);


	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	Grid::Px wpx(w + 2 * m);

	iGrid.rowGap = Grid::Px(m);
	iGrid.columnGap = Grid::Px(m);

	iGrid.templateColumns.add(Grid::TrackInfo(wpx));
	iGrid.templateColumns.add(Grid::TrackInfo(wpx));
	iGrid.templateColumns.add(Grid::TrackInfo(wpx));
	iGrid.templateColumns.add(Grid::TrackInfo(wpx));

	iGrid.templateRows.add(Grid::TrackInfo(50_fr));
	iGrid.templateRows.add(Grid::TrackInfo(50_fr));

	iGrid.autoFlow = Grid::AutoFlow::column;

	GridItem gi1(iKanbanColumns[0]);
	gi1.setArea(1, 1, 3, 1);
	iGrid.items.add(gi1);
	GridItem gi2(iKanbanColumns[1]);
	gi2.setArea(1, 2, 3, 2);
	iGrid.items.add(gi2);
	GridItem gi3(iKanbanColumns[2]);
	gi3.setArea(1, 3, 2, 3);
	iGrid.items.add(gi3);
	GridItem gi4(iKanbanColumns[3]);
	gi4.setArea(2, 3, 3, 3);
	iGrid.items.add(gi4);
	GridItem gi5(iKanbanColumns[4]);
	gi5.setArea(1, 4, 3, 4);
	iGrid.items.add(gi5);
}

void CKanbanBoardComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

}

void CKanbanBoardComponent::resized()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	Rectangle<int> r(getLocalBounds());
	r.reduce(m, m);
	iGrid.performLayout(r);
	
}

CKanbanCardComponent* CKanbanBoardComponent::createCard()
{
	auto c = new CKanbanCardComponent(nullptr);
	iKanbanCards.add(c);
	c->setText( "Empty card " + String(iKanbanCards.size()) );
	return c;
}

CKanbanBoardComponent* CKanbanBoardComponent::fromJson(var& aFile, String& aReturnErrorMessage)
{
	return nullptr;
}

bool CKanbanBoardComponent::saveFile(File& aFile, String& aReturnErrorMessage)
{
	FileOutputStream f(aFile);
	if (f.openedOk())
	{
		f.setPosition(0);
		f.truncate();

		f << "{\n\"version\":\"0.1\",\n\n\"board\":\n{\n";

		f << "\"rows\":" + String(iGrid.templateRows.size()) + ", \"columns\":" + String(iGrid.templateColumns.size()) + ", \n";

		f << "\"def\":[\n";
		int j = 0;
		for (auto i : iGrid.items)
		{
			if (j > 0) f << ",\n";
			int id = static_cast<CKanbanColumnComponent*>(i.associatedComponent)->getColumnId();
			f << " { \"idx\":" + String(j) + ", \"columnId\":" + String(id) + ", \"columnStart\":" + String(i.column.start.getNumber()) + ", \"columnEnd\":" + String(i.column.end.getNumber()) + ", \"rowStart\":" + String(i.row.start.getNumber()) + ", \"rowEnd\":" + String(i.row.end.getNumber()) + " }";
			j++;
		}

		f << "]\n";
		f << "},\n\n\"columns\":\n[\n";

		j = 0;
		for (auto i : iKanbanColumns)
		{
			if (j > 0) f << ",\n";
			f << "{ \"title\":\"" + i->getTitle() + "\", \"id\":" + String(i->getColumnId()) + " }";
			j++;
		}

		f << "\n],\n\n\"cards\":\n[\n";

		j = 0;
		for (auto i : iKanbanCards)
		{
			if (j > 0) f << ",\n";
			f << i->toJson();
			j++;
		}

		f << "\n]\n}\n";

/*		for (int i = 0; i <= iBarsCount; i++)
		{
			int pos, lane, len;
			if (getBarPlacement(i, lane, pos, len))
			{
				auto b = iController.getBarComponent(i);
				if (b)
				{
					f << "{\n\"lane\":" << lane << ",\n\"pos\":" << pos << ",\n\"len\":" << len << ",\n\"color\":" << b->getColour() << ",\n\"properties\":\n{\n";
					auto p = b->getProperties();
					for (int j = 0; j < p.size(); j++)
					{
						f << "\"" << p.getName(j).toString() << "\":\"" << p.getValueAt(j).toString() << "\"";
						if (j < p.size() - 1)
						{
							f << ",";
						}
						f << "\n";
					}
					f << "}\n}";
					if (i < iBarsCount) f << ",";
				}
			}
		}
		f << "\n],\n";

		f << "\"custom_properties\": [ ";
		for (int i = 0; i < iBarPropertiesList.size(); i++)
		{
			f << "\"" << iBarPropertiesList[i] << "\"";
			if (i < iBarPropertiesList.size() - 1)
			{
				f << ",\n";
			}
		}

		f << "]\n}\n";*/

		return true;
	}

	aReturnErrorMessage = "Error opening file for write.";
	return false;
}

