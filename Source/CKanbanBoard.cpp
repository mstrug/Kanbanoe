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
	setOpaque(true);
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

	updateSize();
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
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component



	//g.setColour(juce::Colours::red);
	//g.drawLine(0, 0, getWidth(), getHeight());
}

void CKanbanBoardComponent::resized()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	updateSize();

	Rectangle<int> r(getLocalBounds());
	r.reduce(m, m);
	iGrid.performLayout(r);
}

void CKanbanBoardComponent::updateSize()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int ww = iGrid.getNumberOfColumns() * (w + 3 * m) + m;
	if ( getWidth() < ww ) setSize(ww, getHeight());
}

CKanbanCardComponent* CKanbanBoardComponent::createCard()
{
	auto c = new CKanbanCardComponent(nullptr);
	iKanbanCards.add(c);
	c->setText( "Empty card " + String(iKanbanCards.size()) );
	return c;
}

void CKanbanBoardComponent::removeCard(CKanbanCardComponent* aCard)
{
	iKanbanCards.removeObject(aCard, true);
}

CKanbanBoardComponent* CKanbanBoardComponent::fromJson(var& aFile, String& aReturnErrorMessage)
{
	auto obj = aFile.getDynamicObject();

	CKanbanBoardComponent* ret = new CKanbanBoardComponent();
	if (!ret)
	{
		aReturnErrorMessage = "Out of memory";
		return nullptr;
	}
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	Grid::Px wpx(w + 2 * m);
	ret->iGrid.rowGap = Grid::Px(m);
	ret->iGrid.columnGap = Grid::Px(m);
	ret->iGrid.autoFlow = Grid::AutoFlow::column;

	var version = obj->getProperty("version");
	if (version.isString())
	{
		if (version.toString() != "0.1")
		{
			aReturnErrorMessage = "Not supported file version";
			delete ret;
			return nullptr;
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [version]";
		delete ret;
		return nullptr;
	}

	var columns = obj->getProperty("columns");
	if (columns.isArray())
	{
		auto ar = columns.getArray();
		for (auto& i : *ar)
		{
			auto obj2 = i.getDynamicObject();
			var title = obj2->getProperty("title");
			var id = obj2->getProperty("id");
			if (title.isString() && id.isInt())
			{
				ret->iKanbanColumns.add(new CKanbanColumnComponent(id, title, *ret));
				ret->addAndMakeVisible(ret->iKanbanColumns.getLast());
			}
			else
			{
				aReturnErrorMessage = "Not supported file type [columns array]";
				delete ret;
				return nullptr;
			}
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [columns]";
		delete ret;
		return nullptr;
	}

	var board = obj->getProperty("board");
	if (board.isObject())
	{
		auto obj2 = board.getDynamicObject();
		var rows = obj2->getProperty("rows");
		var cols = obj2->getProperty("columns");
		var def = obj2->getProperty("def");
		if (rows.isInt() && cols.isInt() && def.isArray())
		{
			for (int i = 0; i < (int)cols; i++) ret->iGrid.templateColumns.add(Grid::TrackInfo(wpx));
			for (int i = 0; i < (int)rows; i++) ret->iGrid.templateRows.add(Grid::TrackInfo(50_fr));

			auto ar = def.getArray();
			for (auto& i : *ar)
			{
				auto obj3 = i.getDynamicObject();
				var idx = obj3->getProperty("idx");
				var columnId = obj3->getProperty("columnId");
				var columnStart = obj3->getProperty("columnStart");
				var columnEnd = obj3->getProperty("columnEnd");
				var rowStart = obj3->getProperty("rowStart");
				var rowEnd = obj3->getProperty("rowEnd");
				if (idx.isInt() && columnId.isInt() && columnStart.isInt() && columnEnd.isInt() && rowStart.isInt() && rowEnd.isInt() && (int)idx < ret->iKanbanColumns.size())
				{
					int iidx = idx;
					GridItem gi(ret->iKanbanColumns[iidx]);
					gi.setArea((int)rowStart, (int)columnStart, (int)rowEnd, (int)columnEnd);
					ret->iGrid.items.add(gi);
				}
				else
				{
					aReturnErrorMessage = "Not supported file type [board def array]";
					delete ret;
					return nullptr;
				}
			}
		}
		else
		{
			aReturnErrorMessage = "Not supported file type [rows,columns,def]";
			delete ret;
			return nullptr;
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [board]";
		delete ret;
		return nullptr;
	}

	var cards = obj->getProperty("cards");
	if (cards.isArray())
	{
		auto ar = cards.getArray();
		for (auto& i : *ar)
		{
			auto obj2 = i.getDynamicObject();
			var text = obj2->getProperty("text");
			var notes = obj2->getProperty("notes"); // opt
			var colour = obj2->getProperty("colour");
			var columnId = obj2->getProperty("columnId");
			var url = obj2->getProperty("url"); // opt
			if (text.isString() && colour.isString() && columnId.isInt() )
			{
				String s = colour;
				CKanbanCardComponent* card = ret->createCard();
				NamedValueSet vs;
				vs.set("text", text);
				vs.set("notes", notes);
				vs.set("colour", s);
				vs.set("url", url);
				card->setupFromJson(vs);
				//card->setText(text);
				//card->setNotes(notes);				
				//card->setColour(Colour::fromString(s));
				for (auto j : ret->iKanbanColumns)
				{
					if (j->getColumnId() == (int)columnId)
					{
						j->addCard(card);
						break;
					}
				}
			}
			else
			{
				aReturnErrorMessage = "Not supported file type [cards]";
				delete ret;
				return nullptr;
			}
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [cards array]";
		delete ret;
		return nullptr;
	}

	ret->updateSize();
	return ret;
}

bool CKanbanBoardComponent::saveFile(String& aReturnErrorMessage)
{
	FileOutputStream f(iFile);
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
		for (auto k : iKanbanColumns)
		{
			auto& cld = k->cardsLayout().getLayout().items;
			for (auto p : cld)
			{
				for (auto i : iKanbanCards)
				{
					if (i == p.associatedComponent)
					{
						if (j > 0) f << ",\n";
						f << i->toJson();
						j++;
					}
				}
			}
		}

		f << "\n]\n}\n";
		
		setName(iFile.getFileName());
		return true;
	}

	aReturnErrorMessage = "Error opening file for write.";
	return false;
}

void CKanbanBoardComponent::setFile(File& aFile)
{
	iFile = aFile;
}

File& CKanbanBoardComponent::getFile()
{
	return iFile;
}

