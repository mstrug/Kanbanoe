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

void CKanbanBoardComponent::search(const String & aString)
{
	String s = aString.trim();
	String stag;
	bool tagmode = false, colormode = false;

	if (s.isQuotedString())
	{
		s = s.unquoted();
	}
	else
	{
		if (s.startsWith("tag:"))
		{
			stag = s.substring(4);
			tagmode = true;
		}
		else if (s.startsWith("t:"))
		{
			stag = s.substring(2);
			tagmode = true;
		}
		else if (s.startsWith("color:"))
		{
			stag = s.substring(6);
			colormode = true;
		}
		else if (s.startsWith("colour:"))
		{
			stag = s.substring(7);
			colormode = true;
		}
		else if (s.startsWith("c:"))
		{
			stag = s.substring(2);
			colormode = true;
		}
		stag = stag.trim();
	}

	if (tagmode)
	{
		if (stag.length() != 0)
		{
			Logger::outputDebugString("search tag: " + stag);

			for (auto c : iKanbanCards)
			{
				String tgs = c->getProperties()["tags"];
				if (!tgs.contains(stag))
				{
					c->getOwner()->hideCard(c);
				}
			}
		}
		else // empty search
		{
			for (auto c : iKanbanCards)
			{
				String tgs = c->getProperties()["tags"];
				if (!tgs.isEmpty())
				{
					c->getOwner()->hideCard(c);
				}
			}
		}
	}
	else if (colormode)
	{
		// todo
	}
	else
	{
		Logger::outputDebugString("search: " + s);

		for (auto c : iKanbanCards)
		{
			if (!c->getText().containsIgnoreCase(s) && !c->getNotes().containsIgnoreCase(s))
			{
				c->getOwner()->hideCard(c);
			}
		}
	}
}

void CKanbanBoardComponent::searchClear()
{
	for (auto c : iKanbanColumns)
	{
		c->cardsLayout().unhideAllCards();
	}
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

bool CKanbanBoardComponent::archiveColumn(CKanbanColumnComponent * aColumn, const String & aArchiveName, bool aClearColumn)
{
	int id = aColumn->getColumnId();
	int archiveId = iArchive.size() + 1;

	SArchive* arch = new SArchive();
	iArchive.add(arch);
	arch->iId = archiveId;
	arch->iName = aArchiveName;

	for (auto& c : iKanbanCards)
	{
		if (c->getOwnerColumnId() == id)
		{
			arch->iKanbanCards.add(c->toJson());
		}
	}

	if (aClearColumn && iKanbanCards.size() > 0)
	{
		for (int i = iKanbanCards.size() - 1; i >= 0; i--)
		{
			if (iKanbanCards[i]->getOwnerColumnId() == id)
			{
				aColumn->removeCard(iKanbanCards[i]);
			}
		}
	}
	
	return true;
}

void CKanbanBoardComponent::logArchives()
{
	for (auto a : iArchive)
	{
		Logger::outputDebugString("Arcive: [" + String(a->iId) + "]  [" + a->iName + "]    cards:");
		for (auto c : a->iKanbanCards)
		{
			Logger::outputDebugString(c);
		}
	}

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
		if (version.toString() != "0.1" && version.toString() != "0.2")
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
			var dueDateDone = obj2->getProperty("dueDateDone");
			if (title.isString() && id.isInt())
			{
				auto col = new CKanbanColumnComponent(id, URL::removeEscapeChars(title), *ret);
				ret->iKanbanColumns.add( col );
				if (dueDateDone.isBool()) col->setColumnDueDateDone(true);
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
	if (!fromJsonCardList(cards, ret, aReturnErrorMessage, nullptr))
	{
	//	aReturnErrorMessage = "Not supported file type [cards array]";
		delete ret;
		return nullptr;
	}

	var arch = obj->getProperty("archives");
	if (arch.isArray())
	{
		auto ar = arch.getArray();
		for (auto& i : *ar)
		{
			auto obj2 = i.getDynamicObject();
			var name = obj2->getProperty("name");
			var id = obj2->getProperty("id"); 
			var cards = obj2->getProperty("cards");
			if (name.isString() && id.isInt() && cards.isArray())
			{
				SArchive* archiveObject = new SArchive();
				ret->iArchive.add(archiveObject);
				archiveObject->iId = (int)id;
				archiveObject->iName = URL::removeEscapeChars(name);

				if (!fromJsonCardList(cards, ret, aReturnErrorMessage, archiveObject))
				{
					aReturnErrorMessage += "[archives array]";
					delete ret;
					return nullptr;
				}
			}

		}
	}
	else
	{
		if (version.toString() == "0.2")
		{
			aReturnErrorMessage = "Not supported file type [archives array]";
			delete ret;
			return nullptr;
		}
		// else -> vesrion 0.1 doesn't supports archives
	}

	for (auto c : ret->iKanbanColumns) c->scrollToTop();

	ret->updateSize();
	return ret;
}

bool CKanbanBoardComponent::fromJsonCardList(juce::var& cards, CKanbanBoardComponent* aKanbanBoard, String& aReturnErrorMessage, SArchive* aArchiveObject)
{
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
			var tags = obj2->getProperty("tags"); // opt
			var assigne = obj2->getProperty("assignee"); // opt
			var dueDateSet = obj2->getProperty("dueDateSet");
			var isDone = obj2->getProperty("isDone");
			var crDate = obj2->getProperty("creationDate");
			var dueDate = obj2->getProperty("dueDate");
			var luDate = obj2->getProperty("lastUpdateDate");
			if (text.isString() && colour.isString() && columnId.isInt())
			{
				String s = colour;

				if (aArchiveObject)
				{
					String _t = text;
					String _n = notes;
					String _tg = tags;
					String _as = assigne;
					String _u = url;
					bool _dds = dueDateSet;
					bool _id = isDone;
					String _crd(((juce::int64)crDate));
					String _dd(((juce::int64)dueDate));
					String _ld(((juce::int64)luDate));
					String ss("{ \"text\":\"" + _t + "\", \"colour\":\"" + s + "\", \"columnId\":" + String((int)columnId) + ", \"notes\":\"" + _n + "\", \"url\":\"" + _u + "\", \"tags\":\"" + _tg + "\", \"assignee\":\"" + _as + "\", \"dueDateSet\":" + (_dds ? "true" : "false") + ", \"isDone\":" + (_id ? "true" : "false") + ", \"creationDate\":" + _crd + "\, \"dueDate\":" + _dd +
						", \"lastUpdateDate\":" + _ld + "}");
					aArchiveObject->iKanbanCards.add(ss);
				}
				else
				{
					NamedValueSet vs;
					vs.set("text", text);
					vs.set("notes", notes);
					vs.set("colour", s);
					vs.set("url", url);
					vs.set("tags", tags);
					vs.set("assignee", assigne);
					vs.set("dueDateSet", dueDateSet);
					vs.set("isDone", isDone);
					vs.set("creationDate", crDate);
					vs.set("dueDate", dueDate);
					vs.set("lastUpdateDate", luDate);
					//card->setText(text);
					//card->setNotes(notes);				
					//card->setColour(Colour::fromString(s));

					CKanbanCardComponent* card = aKanbanBoard->createCard();
					card->setupFromJson(vs);
					for (auto j : aKanbanBoard->iKanbanColumns)
					{
						if (j->getColumnId() == (int)columnId)
						{
							j->addCard(card);
							break;
						}
					}
				}
			}
			else
			{
				aReturnErrorMessage = "Not supported file type [cards]";
				return false;
			}
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [cards array]";
		return false;
	}
	return true;
}


bool CKanbanBoardComponent::saveFile(String& aReturnErrorMessage)
{
	FileOutputStream f(iFile);
	if (f.openedOk())
	{
		f.setPosition(0);
		f.truncate();

		f << "{\n\"version\":\"0.2\",\n\n\"board\":\n{\n";

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
			f << "{ \"title\":\"" + URL::addEscapeChars( i->getTitle(), false ) + "\", \"id\":" + String(i->getColumnId());
			if (i->isColumnDueDateDone()) f << ", \"dueDateDone\":true ";
			f << " }";
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

		f << "\n],\n\n\"archives\":\n[\n";

		j = 0;
		for (auto l : iArchive)
		{
			if (j > 0) f << ",\n";
			f << "{ \"id\":" + String(l->iId) + ", \"name\":\"" + URL::addEscapeChars( l->iName, false ) + "\", \"cards\": [\n";

			for (int i = 0; i < l->iKanbanCards.size(); i++)
			{
				if (i > 0) f << ",\n";
				f << " " << l->iKanbanCards[i];
			}

			f << "\n]}";
			j++;
		}

		f << "\n]\n";
		f << "}\n";

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

bool CKanbanBoardComponent::isFileSet()
{
	return iFile.exists();
}

const Array<CKanbanCardComponent*> CKanbanBoardComponent::getCardsForColumn(CKanbanColumnComponent * aColumn)
{
	Array<CKanbanCardComponent*> ret;

	for (int i = 0; i < iKanbanCards.size(); i++)
	{
		if (iKanbanCards[i]->getOwnerColumnId() == aColumn->getColumnId())
		{
			ret.add(iKanbanCards[i]);
		}
	}
	return ret;
}

