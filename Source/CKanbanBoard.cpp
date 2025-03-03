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
#include "CKanbanColumnGitlab.h"
#include "CKanbanColumnGithub.h"
#include "utf8.h"

//==============================================================================
CKanbanBoardComponent::CKanbanBoardComponent() : iGridWidth(0), iColumnsEditorEnabled(false), iListener(nullptr), iParentViewport(nullptr)
{
	setOpaque(false);
}

CKanbanBoardComponent::~CKanbanBoardComponent()
{
}

/*void CKanbanBoardComponent::createDefaultBoard()
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
	Grid::Px wpx2(w / 2 + 2 * m);

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
	iKanbanColumns[0]->setGridItem(gi1);
	GridItem gi2(iKanbanColumns[1]);
	gi2.setArea(1, 2, 3, 2);
	iGrid.items.add(gi2);
	iKanbanColumns[1]->setGridItem(gi2);
	GridItem gi3(iKanbanColumns[2]);
	gi3.setArea(1, 3, 2, 3);
	iGrid.items.add(gi3);
	iKanbanColumns[2]->setGridItem(gi3);
	GridItem gi4(iKanbanColumns[3]);
	gi4.setArea(2, 3, 3, 3);
	iGrid.items.add(gi4);
	iKanbanColumns[3]->setGridItem(gi4);
	GridItem gi5(iKanbanColumns[4]);
	gi5.setArea(1, 4, 3, 4);
	iGrid.items.add(gi5);
	iKanbanColumns[4]->setGridItem(gi5);

	updateSize();
}*/

void CKanbanBoardComponent::createDefaultBoard()
{
	iKanbanColumns.add(new CKanbanColumnComponent(1, "Backlog", *this));
	addAndMakeVisible(iKanbanColumns[0]);

	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	Grid::Px wpx(w + 2 * m);
	Grid::Px wpx2(w / 2 + 2 * m);

	iGrid.rowGap = Grid::Px(m);
	iGrid.columnGap = Grid::Px(m);

	iGrid.templateColumns.add(Grid::TrackInfo(wpx));

	iGrid.templateRows.add(Grid::TrackInfo(50_fr));
	iGrid.templateRows.add(Grid::TrackInfo(50_fr));

	iGrid.autoFlow = Grid::AutoFlow::column;

	GridItem gi1(iKanbanColumns[0]);
	gi1.setArea(1, 1, 3, 1);
	iGrid.items.add(gi1);
	iKanbanColumns[0]->setGridItem(gi1);

	updateSize();
}

void CKanbanBoardComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//	g.fillAll(juce::Colours::red);   // clear the background
//	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component



	//g.setColour(juce::Colours::red);
	//g.drawLine(0, 0, getWidth(), getHeight());
}

void CKanbanBoardComponent::resized()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	if (getWidth() < iGridWidth)
	{
		setSize(iGridWidth, getHeight());
		return;
	}

	Rectangle<int> r(m,m, iGridWidth, getHeight() - 2 * m);
	iGrid.performLayout(r);
}

void CKanbanBoardComponent::updateSize()
{ // setting up columns sizes
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int wmin = CConfiguration::getIntValue("KanbanColumnMinimizedWidth");
	Grid::Px wpx(w + 2 * m);
	Grid::Px wpx_min(wmin + 2 * m);

	/*if (iColumnsEditorEnabled)
	{
		wpx.pixels += CKanbanColumnComponent::getEditModeMargin();
		wpx_min.pixels += CKanbanColumnComponent::getEditModeMargin();
	}*/

	int coln = iGrid.getNumberOfColumns();
	if (coln == 0)
	{
		iGridWidth = 0;
		return;
	}
	iGrid.templateColumns.clearQuick();
	int ww = 2 * m;
	for (int i = 1; i <= coln; i++)
	{
		for (int j = 0; j < iKanbanColumns.size(); j++)
		{
			if (iKanbanColumns[j]->isGridColumn(i, i))
			{
				int emadd = 0;
				if (iColumnsEditorEnabled)
				{
					if (iKanbanColumns[j]->isEditModeLeftVisible()) emadd += CKanbanColumnComponent::getEditModeMargin();
					if (iKanbanColumns[j]->isEditModeRightVisible()) emadd += CKanbanColumnComponent::getEditModeMargin();
				}

				if (iKanbanColumns[j]->isMinimized())
				{
					Grid::Px wpx_min2(wpx_min);
					wpx_min2.pixels += emadd;
					iGrid.templateColumns.add(Grid::TrackInfo(wpx_min2));
					ww += wpx_min2.pixels + m;
				}
				else
				{
					Grid::Px wpx2(wpx);
					wpx2.pixels += emadd;
					iGrid.templateColumns.add(Grid::TrackInfo(wpx2));
					ww += wpx2.pixels + m;
				}
				break;
			}
		}
	}
	ww -= m;
	iGridWidth = ww;
	if (getHeight() != 0 ) setSize(iGridWidth, getHeight());
}

void CKanbanBoardComponent::updateColumnSize(CKanbanColumnComponent * aColumn, bool aMinimized)
{
	// sync other columns in the same grid col
	auto gi = aColumn->getGridItem();
	for (int j = 0; j < iKanbanColumns.size(); j++)
	{
		if (iKanbanColumns[j] == aColumn) continue;

		if (iKanbanColumns[j]->isGridColumn(gi.column.start.getNumber(), gi.column.end.getNumber()))
		{
			iKanbanColumns[j]->setMinimized(aMinimized, false);
			iKanbanColumns[j]->setEditModeRightVisible(aColumn->isEditModeRightVisible());
			iKanbanColumns[j]->setEditModeLeftVisible(aColumn->isEditModeLeftVisible());
		}
	}

	updateSize();
	repaint();
}

void CKanbanBoardComponent::updateComunsTitles()
{
	for (auto c : iKanbanColumns)
	{
		c->updateColumnTitle();
	}
}

void CKanbanBoardComponent::search(const String & aString)
{
	iSearchText = aString;

	String s = aString.trim();
	String stag;
	bool tagmode = false, colormode = false, assignemode = false;
	bool caseInsensitive = CConfiguration::getBoolValue(KConfigSearchCase);

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
		else if (s.startsWith("assignee:"))
		{
			stag = s.substring(9);
			assignemode = true;
		}
		else if (s.startsWith("a:"))
		{
			stag = s.substring(2);
			assignemode = true;
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
				if (caseInsensitive ? (utf8casestr(tgs.toRawUTF8(), stag.toRawUTF8()) == nullptr) : !tgs.contains(stag))
				//if (caseInsensitive ? !tgs.containsIgnoreCase(stag) : !tgs.contains(stag))
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
		if (stag.length() != 0)
		{
			auto& pal = CConfiguration::getColourPalette();

			if (stag.length() == 1)
			{
				int val = stag.getIntValue();
				if (val >= 1 && val <= 6)
				{
					for (auto c : iKanbanCards)
					{
						Colour col = c->getColour();
						int idx = pal.getColourIndex(col);
						idx++; // 0..5 -> 1..6

						if ( idx != val )
						{
							c->getOwner()->hideCard(c);
						}
					}
				}
			}
			else
			{
				for (auto c : iKanbanCards)
				{
					Colour col = c->getColour();
					String name = pal.getColourName(col);
					
					if (caseInsensitive ? (utf8casestr(name.toRawUTF8(), stag.toRawUTF8()) == nullptr) : !name.contains(stag))
					//if (caseInsensitive ? !name.containsIgnoreCase(stag) : !name.contains(stag))
					{
						c->getOwner()->hideCard(c);
					}
				}
			}
		}
		else // empty search
		{
			auto& pal = CConfiguration::getColourPalette();
			for (auto c : iKanbanCards)
			{
				Colour col = c->getColour();
				String name = pal.getColourName(col);

				if (!name.contains("none") && !name.isEmpty())
				{
					c->getOwner()->hideCard(c);
				}
			}
		}
	}
	else if (assignemode)
	{
		if (stag.length() != 0)
		{
			for (auto c : iKanbanCards)
			{
				String as = c->getAssigne();
				
				if (caseInsensitive ? (utf8casestr(as.toRawUTF8(), stag.toRawUTF8()) == nullptr) : !as.contains(stag))
				//if (caseInsensitive ? !as.containsIgnoreCase(stag) : !as.contains(stag))
				{
					c->getOwner()->hideCard(c);
				}
			}
		}
		else // empty search
		{
			for (auto c : iKanbanCards)
			{
				String as = c->getAssigne();
				if (!as.isEmpty())
				{
					c->getOwner()->hideCard(c);
				}
			}
		}
	}
	else
	{
		Logger::outputDebugString("search: " + s);

		for (auto c : iKanbanCards)
		{			
			if ( (utf8casestr(c->getText().toRawUTF8(), s.toRawUTF8()) == nullptr) &&
				 (utf8casestr(c->getNotes().toRawUTF8(), s.toRawUTF8()) == nullptr) &&
				 (utf8casestr(c->getAssigne().toRawUTF8(), s.toRawUTF8()) == nullptr) )
			//if (!c->getText().containsIgnoreCase(s) && !c->getNotes().containsIgnoreCase(s) && !c->getAssigne().containsIgnoreCase(s))
			{
				c->getOwner()->hideCard(c);
			}
		}
	}

	updateComunsTitles();
}

void CKanbanBoardComponent::searchClear()
{
	iSearchText.clear();

	for (auto c : iKanbanColumns)
	{
		c->cardsLayout().unhideAllCards();
	}

	updateComunsTitles();
}

void CKanbanBoardComponent::updateSearch()
{
	String tmp = iSearchText;
	searchClear();
	if (!tmp.isEmpty()) search(tmp); // this way we handle proper hidden cards count
}

CKanbanCardComponent* CKanbanBoardComponent::createCard()
{
	auto c = new CKanbanCardComponent(nullptr);
	iKanbanCards.add(c);
	c->setText(KanbanCardComponentDefaultTitle + String(iKanbanCards.size()) );
	c->addListener(this);
	return c;
}

void CKanbanBoardComponent::removeCard(CKanbanCardComponent* aCard)
{
	iKanbanCards.removeObject(aCard, true);
	if (iListener) iListener->KanbanBoardChanged();
}

void CKanbanBoardComponent::updateCardsView()
{
	for (auto& c : iKanbanCards)
	{
		c->updateCardView();
	}
}

bool CKanbanBoardComponent::archiveColumn(CKanbanColumnComponent * aColumn, const String & aArchiveName, bool aClearColumn)
{
	int id = aColumn->getColumnId();
	int archiveId = iArchive.size() + 1;

	SArchive* arch = new SArchive();
	iArchive.add(arch);
	arch->iId = archiveId;
	arch->iName = aArchiveName;
	arch->iDate = juce::Time::getCurrentTime();

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
	
	if (iListener) iListener->KanbanBoardChanged();

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
		if (version.toString() != "0.1" && version.toString() != "0.2" && version.toString() != "0.3" && version.toString() != "0.4")
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
			var minimized = obj2->getProperty("minimized");
			var wip = obj2->getProperty("wip");
			var type = obj2->getProperty("type");
			if (title.isString() && id.isInt())
			{
				int colType = 0;
				if (type.isInt()) colType = type;

				CKanbanColumnComponent *col = nullptr;
				if (colType == 0) col = new CKanbanColumnComponent(id, URL::removeEscapeChars(title), *ret);
				else if (colType == 1) col = CKanbanColumnGitlab::createFromJson(id, URL::removeEscapeChars(title), *ret, obj2);
				else if (colType == 2) col = CKanbanColumnGithub::createFromJson(id, URL::removeEscapeChars(title), *ret, obj2);
				else
				{
					aReturnErrorMessage = "Not supported column type [columns array]";
					delete ret;
					return nullptr;
				}
				if (col == nullptr)
				{
					aReturnErrorMessage = "Cannot create column [columns array]";
					delete ret;
					return nullptr;
				}
				ret->iKanbanColumns.add( col );
				if (dueDateDone.isBool()) col->setColumnDueDateDone(true);
				if (minimized.isBool()) col->setMinimized(true, false);
				if (wip.isInt()) col->setMaxWip(wip);
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
					ret->iKanbanColumns[iidx]->setGridItem(gi);
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
			var date = obj2->getProperty("date");
			if (name.isString() && id.isInt() && cards.isArray())
			{
				SArchive* archiveObject = new SArchive();
				ret->iArchive.add(archiveObject);
				archiveObject->iId = (int)id;
				archiveObject->iName = URL::removeEscapeChars(name);
				if (date.isInt64())
				{
					archiveObject->iDate = juce::Time((juce::int64)date);
				}

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
		if (version.toString() == "0.2" || version.toString() == "0.3" || version.toString() == "0.4")
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


bool CKanbanBoardComponent::fromJsonCardList(juce::var& cards, CKanbanBoardComponent* aKanbanBoard, String& aReturnErrorMessage, SArchive* aArchiveObject, CKanbanCardComponent* aCard)
{
	if (cards.isArray())
	{
		auto ar = cards.getArray();
		int cnt = ar->size();
		for (auto& i : *ar)
		{
			cnt--;

			auto obj2 = i.getDynamicObject();
			if (!CKanbanBoardComponent::fromJsonCard(obj2, aKanbanBoard, aReturnErrorMessage, aArchiveObject, cnt > 0, aCard))
			{
				aReturnErrorMessage = "Not supported file type [card]";
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


bool CKanbanBoardComponent::fromJsonCard(const juce::DynamicObject* obj2, CKanbanBoardComponent* aKanbanBoard, String& aReturnErrorMessage, SArchive* aArchiveObject, bool aLoadFromFile, CKanbanCardComponent* aCard)
{
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
	var customProp = obj2->getProperty("customProp");
	StringPairArray customPropArray;
	if (text.isString() && colour.isString() && columnId.isInt())
	{
		String s = colour;

		if (customProp.isArray())
		{
			auto ar2 = customProp.getArray();
			for (auto& cp : *ar2)
			{
				auto ns = cp.getDynamicObject()->getProperties();
				if (ns.size() > 0)
				{
					customPropArray.set(ns.getName(0).toString(), ns.getValueAt(0).toString());
				}
			}
		}

		if (aArchiveObject)
		{ // todo: add custom properties?
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
			String ss("{ \"text\":\"" + _t + "\", \"colour\":\"" + s + "\", \"columnId\":" + String((int)columnId) + ", \"notes\":\"" + _n + "\", \"url\":\"" + _u + "\", \"tags\":\"" + _tg + "\", \"assignee\":\"" + _as + "\", \"dueDateSet\":" + (_dds ? "true" : "false") + ", \"isDone\":" + (_id ? "true" : "false") + ", \"creationDate\":" + _crd + ", \"dueDate\":" + _dd +
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

			CKanbanCardComponent* card;
			if (aCard) card = aCard;
			else
			{
				jassert(aKanbanBoard);
				card = aKanbanBoard->createCard();
			}

			card->setupFromJson(vs, customPropArray);

			if (aKanbanBoard)
			{
				for (auto j : aKanbanBoard->iKanbanColumns)
				{
					if (j->getColumnId() == (int)columnId)
					{
						j->addCard(card, aLoadFromFile);
						break;
					}
				}
			}
		}
	}
	else
	{
		aReturnErrorMessage = "Not supported file type [cards]";
		return false;
	}
	return true;
}

void CKanbanBoardComponent::addListener(Listener * aListener)
{
	jassert(aListener);
	iListener = aListener;
}

void CKanbanBoardComponent::removeListener(Listener * aListener)
{
	if (iListener == aListener)
	{
		iListener = nullptr;
	}
}

void CKanbanBoardComponent::setParentViewport(Viewport * aParentViewport)
{
	iParentViewport = aParentViewport;
}

Viewport * CKanbanBoardComponent::getParentViewport()
{
	return iParentViewport;
}

void CKanbanBoardComponent::KanbanCardChanged()
{
	if (iListener) iListener->KanbanBoardChanged();
}

int CKanbanBoardComponent::updateGridWidth()
{
	float w = 0;
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	w += iGrid.getNumberOfColumns() * (3 * m) + m;

	for (auto ti : iGrid.templateColumns)
	{
		w += ti.getSize();
	}
	return (int)w;

	/*int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	Rectangle<int> r(getLocalBounds());
	r.setWidth(w);
	r.reduce(m, m);
	iGrid.performLayout(r);*/
}

bool CKanbanBoardComponent::isColumnLastInGrid(CKanbanColumnComponent * aColumn)
{
	int end = aColumn->getGridItem().column.end.getNumber();
	return (end >= iGrid.getNumberOfColumns());
}

bool CKanbanBoardComponent::isColumnHalfBeforeFull(CKanbanColumnComponent * aColumn)
{
	int end = aColumn->getGridItem().column.end.getNumber();
	int rc = iGrid.getNumberOfRows() + 1; // row end in grid item is larger by 1 from number of rows, so we need to add 1 to the count
	if (end < iGrid.getNumberOfColumns())
	{
		int cnt = 0;
		for (GridItem& gi : iGrid.items)
		{
			int cs = gi.column.start.getNumber();
			int rs = gi.row.start.getNumber();
			int re = gi.row.end.getNumber();
			if (cs == end + 1)
			{
				if ( rs == 1 && re == rc ) cnt++;
			}
		}
		return cnt == 1;
	}
	return false;
}

bool CKanbanBoardComponent::isColumnNextInGridSameSize(CKanbanColumnComponent * aColumn)
{
	int rs = aColumn->getGridItem().row.start.getNumber();
	int re = aColumn->getGridItem().row.end.getNumber();
	int ce = aColumn->getGridItem().column.end.getNumber();

	for (auto& gi : iGrid.items)
	{
		if (gi.column.start.getNumber() == ce + 1)
		{
			return (gi.row.start.getNumber() == rs && gi.row.end.getNumber() == re);
		}
	}

	return false;
}


bool CKanbanBoardComponent::saveFile(String& aReturnErrorMessage)
{
	FileOutputStream f(iFile);
	if (f.openedOk())
	{
		f.setPosition(0);
		f.truncate();

		f << "{\n\"version\":\"" << KKanbanBoardJsonFileVersion << "\",\n\n\"board\":\n{\n";

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
			f << i->outputAsJson();
			/*f << "{ \"title\":\"" + URL::addEscapeChars( i->getTitle(), false ) + "\", \"id\":" + String(i->getColumnId());
			if (i->isColumnDueDateDone()) f << ", \"dueDateDone\":true ";
			if (i->isMinimized()) f << ", \"minimized\":true ";
			if (i->getMaxWip() > 0) f << ", \"wip\":" + String(i->getMaxWip());
			f << " }";*/
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
			f << "{ \"id\":" + String(l->iId) + ", \"name\":\"" + URL::addEscapeChars( l->iName, false ) + "\", \"date\":" + String(l->iDate.toMilliseconds()) + ", \"cards\": [\n";

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
		if (iListener) iListener->KanbanBoardStored();
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

void CKanbanBoardComponent::setColumnsEditor(bool aEnabled)
{
	iColumnsEditorEnabled = aEnabled;

	for (int i = 0; i < iKanbanColumns.size(); i++)
	{
		bool right = false;
		if ( isColumnLastInGrid( iKanbanColumns[i]) )
		{
			right = true;
		}
		else
		{
			right = !isColumnNextInGridSameSize(iKanbanColumns[i]);
		}
		iKanbanColumns[i]->setEditMode(aEnabled, right && aEnabled);
	}
	updateSize();
}

bool CKanbanBoardComponent::isColumnsEditorEnabled()
{
	return iColumnsEditorEnabled;
}

void CKanbanBoardComponent::addColumn(CKanbanColumnComponent * aColumn, bool aBefore)
{
	int maxid = 0;
	for (auto k : iKanbanColumns)
	{
		if (k->getColumnId() >= maxid) maxid = k->getColumnId() + 1;
	}
	String colName = "Column " + String(maxid);

	// firstly choose column type
	AlertWindow aw("Create column", "Provide new column information", AlertWindow::QuestionIcon);
	aw.addTextEditor("text", colName, "Column name:");
	int ct = CConfiguration::getColumnTypesCount();
	if (ct > 1)
	{
		aw.addComboBox("option", CConfiguration::getColumnTypesNames(), "Column type:");
	}
	ToggleButton tb("Due date done column");
	tb.setComponentID("checkbox");
	tb.setSize(250, 36);
	tb.setName("");
	tb.setToggleState(false, NotificationType::dontSendNotification);
	aw.addCustomComponent(&tb);
	aw.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	aw.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

	auto txt = aw.getTextEditor("text");
	txt->setExplicitFocusOrder(1);

	if (aw.runModalLoop() != 0) // is they picked 'ok'
	{
		auto text = aw.getTextEditorContents("text");
		bool checked = tb.getToggleState();
		if (!text.isEmpty())
		{
			colName = text;
		}
	}
	else
	{
		return;
	}

	// create column object
	int optionIndexChosen = 0;
	if ( ct > 1 ) optionIndexChosen = aw.getComboBoxComponent("option")->getSelectedItemIndex();
	CKanbanColumnComponent* col = nullptr;
	if (optionIndexChosen == 0)
	{ // normal column
		col = new CKanbanColumnComponent(maxid, colName, *this);
	}
	else if (optionIndexChosen == 1)
	{ // gitlab integration
		col = CKanbanColumnGitlab::createWithWizard(maxid, colName, *this);
	}
	else if (optionIndexChosen == 2)
	{ // github integration
		col = CKanbanColumnGithub::createWithWizard(maxid, colName, *this);
	}
	if (col == nullptr)
	{
		return;
	}
	col->setEditMode(iColumnsEditorEnabled, !aBefore && ( isColumnLastInGrid(aColumn) || !isColumnNextInGridSameSize(aColumn)) );
	iKanbanColumns.add(col);
	addAndMakeVisible(col);

	if (iColumnsEditorEnabled && !aBefore) aColumn->setEditModeRightVisible(false); // hide right add button on previous column

	// update grid
	// todo: check if half column should be added to existing column with half column
	int gi1col = aColumn->getGridItem().column.start.getNumber();
	int gi1rows = aColumn->getGridItem().row.start.getNumber();
	int gi1rowe = aColumn->getGridItem().row.end.getNumber();

	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	Grid::Px wpx(w + 2 * m);
	iGrid.templateColumns.add(Grid::TrackInfo(wpx));

	if (!aBefore) gi1col++;

	for (auto& gi : iGrid.items)
	{
		int gi2col = gi.column.start.getNumber();
		if (gi2col >= gi1col)
		{
			gi.setArea(gi.row.start, gi.column.start.getNumber() + 1, gi.row.end, gi.column.end.getNumber() + 1);
			static_cast<CKanbanColumnComponent*>(gi.associatedComponent)->setGridItem(gi);
		}
	}

	GridItem gi(col);
	gi.setArea(gi1rows, gi1col, gi1rowe, gi1col);
	iGrid.items.add(gi);
	col->setGridItem(gi);

	//todo fix for err. aColumn->setEditModeRightVisible(isColumnLastInGrid(aColumn) || isColumnHalfBeforeFull(aColumn) );
	updateColumnSize(aColumn, aColumn->isMinimized()); // update rest items in the same column
	//aColumn->setEditModeRightVisible //todo

	updateSize();

	if (iListener) iListener->KanbanBoardChanged();
}

void CKanbanBoardComponent::removeColumn(CKanbanColumnComponent * aColumn)
{
	int ci = aColumn->getGridItem().column.start.getNumber();
	//int ri = aColumn->getGridItem().row.end.getNumber() - aColumn->getGridItem().row.end.getNumber();
	//bool divided_col = (ri != iGrid.getNumberOfRows());

	for (auto& gi : iGrid.items)
	{
		if (gi.associatedComponent == aColumn)
		{
			iGrid.items.remove(&gi);
			//iGrid.templateColumns.remove(ci - 1);
			break;
		}
	}
	removeChildComponent(aColumn);
	iKanbanColumns.removeObject(aColumn, true);

	// remove empty grid columns
	Array<int> cols;
	for (int i = 0; i < iGrid.getNumberOfColumns(); i++)
	{
		cols.add(i + 1);
	}
	for (auto& gi : iGrid.items)
	{
		cols.removeAllInstancesOf(gi.column.start.getNumber());
		cols.removeAllInstancesOf(gi.column.end.getNumber());
	}
	int cols_removed = cols.size();
	for (int i = cols.size() - 1; i >= 0; i--)
	{
		iGrid.templateColumns.remove(cols[i]);
	}

	// move columns on right of deleted column to left
	if (cols_removed > 0)
	{
		for (auto& gi : iGrid.items)
		{
			if (gi.column.start.getNumber() > ci && ci >= 0)
			{
				// assuming continous columns area
				gi.setArea(gi.row.start, gi.column.start.getNumber() - cols_removed, gi.row.end, gi.column.end.getNumber() - cols_removed);
				if (gi.associatedComponent) static_cast<CKanbanColumnComponent*>(gi.associatedComponent)->setGridItem(gi);
			}
		}
	}

	// update grid
	updateSize();

	if (iListener) iListener->KanbanBoardChanged();
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

const Array<CKanbanCardComponent*> CKanbanBoardComponent::getCardsByNameAndUrl(StringRef aName, StringRef aUrl)
{
	Array<CKanbanCardComponent*> ret;

	for (int i = 0; i < iKanbanCards.size(); i++)
	{
		if (iKanbanCards[i]->getText() == aName && (aUrl.isEmpty() || iKanbanCards[i]->getUrl() == aUrl))
		{
			ret.add(iKanbanCards[i]);
		}
	}
	return ret;
}

const Array< CKanbanCardComponent* > CKanbanBoardComponent::getCardsForDueDoneColumns()
{
	Array<CKanbanCardComponent*> ret;
	Array<int> cids;

	for (int i = 0; i < iKanbanColumns.size(); i++)
	{
		if (iKanbanColumns[i]->isColumnDueDateDone() )
		{
			cids.add(iKanbanColumns[i]->getColumnId());
		}
	}

	for (int i = 0; i < iKanbanCards.size(); i++)
	{
		int owner_col_id = iKanbanCards[i]->getOwnerColumnId();
		if (cids.indexOf(owner_col_id) >= 0)
		{
			ret.add(iKanbanCards[i]);
		}
	}
	return ret;
}

const OwnedArray<CKanbanColumnComponent>& CKanbanBoardComponent::getColumns()
{
	return iKanbanColumns;
}

const CKanbanColumnComponent* CKanbanBoardComponent::getColumnById(int aId)
{
	Array<CKanbanColumnComponent*> ret;

	for (int i = 0; i < iKanbanColumns.size(); i++)
	{
		if (iKanbanColumns[i]->getColumnId() == aId)
		{
			return iKanbanColumns[i];
		}
	}
	return nullptr;
}


const OwnedArray<CKanbanBoardComponent::SArchive>& CKanbanBoardComponent::getArchives()
{
	return iArchive;
}

