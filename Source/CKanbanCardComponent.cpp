/*
  ==============================================================================

    CKanbanCardComponent.cpp
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnContentComponent.h"
#include "CKanbanColumnComponent.h"
#include "CKanbanBoard.h"
#include "Cconfiguration.h"
#include "CKanbanCardPropertiesComponent.h"

// clipboard card
static std::unique_ptr<CKanbanCardComponent> clipboardCard; // = nullptr;

const int KAssigneeLength = 3;


//==============================================================================
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnContentComponent* aOwner) : iIsDragging(false), iOwner(aOwner), iMouseActive(false), iIsUrlSet(false), iIsUrlMouseActive(false), iIsDueDateSet(false), iIsDone(false), iCreationDate(juce::Time::getCurrentTime()), iReadOnly(false)
{
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	iColorBar = CConfiguration::getColourPalette().getLastColor();
	//	juce::Colours::coral;// gold);
	//iColorBar.fromRGBA(0, 0, 0, 0);

	iLabel.setText("test", NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	iLabel.addMouseListener(this,true);
	iLabel.setMinimumHorizontalScale(1);

	iButtonUrl.setButtonText("+");
	addAndMakeVisible(iButtonUrl);
	

	//setOpaque(true);

	setSize(w, h);

	iLastUpdateDate = iCreationDate;
}

CKanbanCardComponent::~CKanbanCardComponent()
{
}

void CKanbanCardComponent::duplicateDataFrom(const CKanbanCardComponent & aCard, bool aDuplicateDates )
{
	iLabel.setText(aCard.iLabel.getText(), NotificationType::dontSendNotification);
	iColorBar = aCard.iColorBar;
	iNotes = aCard.iNotes;
	iAssigne.setText(aCard.iAssigne.getText(), NotificationType::dontSendNotification);
	iButtonUrl.setButtonText(aCard.iButtonUrl.getButtonText());
	iIsUrlSet = aCard.iIsUrlSet;
	iIsDueDateSet = aCard.iIsDueDateSet;
	iDueDate = aCard.iDueDate;
	if (aDuplicateDates)
	{
		iCreationDate = aCard.iCreationDate;
		iLastUpdateDate = aCard.iLastUpdateDate;
	}

	for ( int i = 0; i < aCard.getProperties().size(); i++)
	{
		getProperties().set(aCard.getProperties().getName(i), aCard.getProperties().getValueAt(i));		
	}
}

void CKanbanCardComponent::paint (juce::Graphics& g)
{
	if (iIsDragging)
	{
		//g.fillAll(juce::Colours::darkred);
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}
	else
	{
		if (iMouseActive)
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));   // clear the background
		}
		else
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
		}

		g.setColour (juce::Colours::grey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

		g.setColour(iColorBar);
		if (getHeight()>8) g.fillRect(4,4,2,getHeight()-8);

		//g.setColour(Colours::red);
		//g.fillRect(iRectAssigne);
		if (!iAssigne.getText().isEmpty())
		{
			g.setColour(Colours::lightgrey);
			g.setFont(juce::Font(12.0f));
			g.drawText(iAssigne.getText().substring(0, KAssigneeLength), iRectAssigne, Justification::centredRight, false);
		}

		if (iIsUrlSet)
		{
			if (iIsUrlMouseActive) g.setColour(juce::Colours::lightgrey);
			else g.setColour(juce::Colours::grey);
			
			PathStrokeType pt(2, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::butt);
			g.strokePath(iUrlPath, pt);

			//g.drawArrow(iLineUrl, 1, 5, 5);
		}

		if (iIsDueDateSet)
		{
			auto b = getLocalBounds();
			auto b1 = b.removeFromRight(50);
			auto b2 = b1.removeFromTop(20);
			b2.translate(-1, 0);
			g.setFont(juce::Font(12.0f));

			juce::Colour col;
			String s = getDueDateAsString(&col);
			g.setColour(col);

			g.drawText( s, b2, Justification::topRight, false);
		}
	}
}

void CKanbanCardComponent::resized()
{
	Rectangle<int> r(getLocalBounds());
	if (r.getHeight() == 0) return;
		
	r.removeFromLeft(10);
	r.removeFromRight(20);
	iLabel.setBounds(r);

	iRectUrl = getLocalBounds().removeFromRight(15);
	iRectUrl = iRectUrl.removeFromBottom(15);
	iRectUrl -= Point<int>(1, 1);

	iRectAssigne = getLocalBounds().removeFromRight(30);
	iRectAssigne.removeFromTop(14);
	iRectAssigne.removeFromBottom(15);
	iRectAssigne.translate(-1, 0);

	//iLineUrl.setStart(iRectUrl.getTopLeft().x + 2, iRectUrl.getBottom() - 2);
	//iLineUrl.setEnd(iRectUrl.getRight() - 2, iRectUrl.getTopLeft().y + 2);

	iUrlPath = generateUrlPath(iRectUrl);

	/*auto r2 = iRectUrl.reduced(4).translated(-1, 2);
	iUrlPath.clear();
	iUrlPath.startNewSubPath(r2.getTopLeft().x + 3, r2.getTopLeft().y);
	iUrlPath.lineTo(r2.getTopLeft().x, r2.getTopLeft().y);
	iUrlPath.lineTo(r2.getTopLeft().x, r2.getBottom());
	iUrlPath.lineTo(r2.getRight(), r2.getBottom());
	iUrlPath.lineTo(r2.getRight(), r2.getBottom() - 3);

	float aw = 5, aa = 5;
	iUrlPath.startNewSubPath(r2.getCentreX() + 1, r2.getCentreY());
	iUrlPath.lineTo(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw);
	iUrlPath.lineTo(r2.getCentreX() + 1 + aw - aa, r2.getCentreY() - aw);
	iUrlPath.startNewSubPath(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw);
	iUrlPath.lineTo(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw + aa);*/
}

void CKanbanCardComponent::mouseDown(const MouseEvent& event)
{
	//Logger::outputDebugString("entered mouseDown");
}

void CKanbanCardComponent::mouseDrag(const MouseEvent& event)
{
	if ( !iIsDragging && event.mods.isLeftButtonDown() && !iReadOnly )
	{
		//Logger::outputDebugString("entered mouseDrag");
		if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
		{
			dragContainer->startDragging(KanbanCardComponentDragDescription, this);
			iIsDragging = true;
			setChildrenVisibility(iIsDragging);
			repaint();
			beginDragAutoRepeat(80);
		}
	}
	else if (iIsDragging)
	{
		// autoscroll for whole board
		auto v = iOwner->getOwner().kanbanBoard().getParentViewport();
		if (v)
		{
			MouseEvent re(event.getEventRelativeTo( v->getParentComponent() ));
			//Logger::outputDebugString("mouse: (" + String(re.x) + "," + String(re.y) + ") ");
			v->autoScroll(re.x, re.y, 30, 10 );
		}
	}
}

void CKanbanCardComponent::mouseUp(const MouseEvent& event)
{
	if (iIsDragging)
	{
		Logger::outputDebugString("entered iisdragging");
		iIsDragging = false;
		setChildrenVisibility(iIsDragging);
		repaint();
	}
	else if (event.mods.isLeftButtonDown())
	{
		URL u(getProperties()["url"]);
		if (iIsUrlSet &&
			((event.eventComponent == this && iRectUrl.contains(event.getPosition())) ||
			(event.eventComponent != this && iRectUrl.contains(event.getEventRelativeTo(this).getPosition()))))
		{
			//u.isWellFormed() // todo
			u.launchInDefaultBrowser();
		}
		else
		{
			Logger::outputDebugString("entered showProperties");
			showProperties();
		}

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
	else if (event.mods.isRightButtonDown())
	{
		PopupMenu menu;
		menu.addItem("Remove", [&]()
		{
			int ret = 1;
			if (!isEmpty())
			{
				ret = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon, "Confirmation", "Do you really want to remove this card?");
			}
			if (ret == 1)
			{
				MessageManager::callAsync([&]()
				{
					this->getOwner()->getOwner().removeCard(this);
				});				
			}
		});
		menu.addItem("Duplicate", [&]()
		{
			//deselect();
			iMouseActive = false;
			this->getOwner()->getOwner().duplicateCard(this);
		});
		menu.addItem("Copy", [&]()
		{
			deselect();
			setClipboardCard(this);
		});
		/*menu.addItem("Cut", [&]()
		{
			this->getOwner()->getOwner().removeCard(this);
			setClipboardCard(this);
		});*/
		menu.addSeparator();
		menu.addItem("Move top", [&]()
		{
			this->deselect();
			this->getOwner()->moveCardTop(this);
			this->repaint();
		});
		menu.addItem("Move bottom", [&]()
		{
			this->deselect();
			this->getOwner()->moveCardBottom(this);
			this->repaint();
		});
		menu.show();
		deselect();
	}
}

void CKanbanCardComponent::mouseDoubleClick(const MouseEvent& event)
{
	Logger::outputDebugString("DB");
}

void CKanbanCardComponent::mouseEnter(const MouseEvent& event)
{
	iMouseActive = true;
	repaint();
}

void CKanbanCardComponent::mouseMove(const MouseEvent & event)
{
	if (iMouseActive && iIsUrlSet)
	{
		bool tmp = iIsUrlMouseActive;
		//Logger::outputDebugString("pos: " + String(event.getPosition().x) + ", " + String(event.getPosition().y));
		//Logger::outputDebugString("x: " + String(iRectUrl.getTopLeft().x) + "  y:" + String(iRectUrl.getTopLeft().y));
		//Logger::outputDebugString(event.eventComponent == this ? "ok" : "nok");
		if (event.eventComponent == this)
		{
			iIsUrlMouseActive = iRectUrl.contains(event.getPosition());
		}
		else
		{
			auto p = event.getEventRelativeTo(this).getPosition();
			//Logger::outputDebugString("pos2: " + String(p.x) + ", " + String(p.y));
			iIsUrlMouseActive = iRectUrl.contains(p);	
		}
		if ( tmp != iIsUrlMouseActive ) repaint();
	}
}

void CKanbanCardComponent::mouseExit(const MouseEvent& event)
{
	iMouseActive = false;
	iIsUrlMouseActive = false;
	repaint();
}


CKanbanColumnContentComponent* CKanbanCardComponent::getOwner()
{
	return iOwner;
}

void CKanbanCardComponent::setOwner(CKanbanColumnContentComponent* aOwner)
{
	if ( aOwner != iOwner )
	{
		auto oldOwner = iOwner;
		iOwner = aOwner;
		//if ( oldOwner != nullptr )
		{
			notifyListeners();
		}
	}
}

void CKanbanCardComponent::openPropertiesWindow()
{
	showProperties();
}

void CKanbanCardComponent::setupFromArchive(const juce::var & aArchive)
{
	auto obj = aArchive.getDynamicObject();

	String ret;
	CKanbanBoardComponent::fromJsonCard(obj, nullptr, ret, nullptr, false, this);
}

void CKanbanCardComponent::setupFromJson(const NamedValueSet& aValues, const StringPairArray& aCustomProps)
{
	iLabel.setText(URL::removeEscapeChars(aValues["text"]), NotificationType::dontSendNotification);
	iColorBar = Colour::fromString(URL::removeEscapeChars(aValues["colour"]));
	iNotes = URL::removeEscapeChars(aValues["notes"]);
	setUrl(URL::removeEscapeChars(aValues["url"]));
	setTags(URL::removeEscapeChars(aValues["tags"]));
	setAssigne(URL::removeEscapeChars(aValues["assignee"]));
	iIsDueDateSet = aValues["dueDateSet"];
	iIsDone = aValues["isDone"];
	iCreationDate = Time(aValues["creationDate"].toString().getLargeIntValue());
	iDueDate = Time(aValues["dueDate"].toString().getLargeIntValue());
	iLastUpdateDate = Time(aValues["lastUpdateDate"].toString().getLargeIntValue());
	
	for (auto &k : aCustomProps.getAllKeys())
	{
		getProperties().set(k, aCustomProps[k]);
	}

	// todo:  wa for versions prev 0.25: (remove in future)
	if (iCreationDate.toMilliseconds() == 0) iCreationDate = Time(2021, 0, 1, 12, 00);
	if (iLastUpdateDate.toMilliseconds() == 0) iLastUpdateDate = Time(2021, 0, 1, 12, 00);
	if (iIsDueDateSet && iDueDate.toMilliseconds() == 0) iDueDate = Time(2021, 0, 1, 12, 00);
}

void CKanbanCardComponent::setText(const String& aString)
{
	updateLastUpdateDate();
	iLabel.setText(aString, NotificationType::dontSendNotification);
}

String CKanbanCardComponent::getText()
{
	return iLabel.getText();
}

void CKanbanCardComponent::setUrl(const String& aString)
{
	updateLastUpdateDate();
	if ( aString.contains("://") || aString.indexOf("//") == 0 )
	{
		getProperties().set("url", aString);
		iIsUrlSet = true;
	}
	else if ( aString.length() > 0 )
	{
		getProperties().set("url", "http://"+aString);
		iIsUrlSet = true;
	}
	else
	{
		getProperties().remove("url");
		iIsUrlSet = false;
	}
	/*iButtonUrl.setVisible(aString.length() > 0);
	iButtonUrl.setURL(aString);
	resized();*/
}

void CKanbanCardComponent::setTags(const String& aString)
{
	updateLastUpdateDate();
	getProperties().set("tags", aString);
}

bool CKanbanCardComponent::isDueDateSet()
{
	return iIsDueDateSet;
}

void CKanbanCardComponent::setDueDate(bool aIsSet, juce::Time& aDueDate)
{
	if ( iIsDueDateSet != aIsSet )
	{
		iIsDueDateSet = aIsSet;
		// resized();
	}
	updateLastUpdateDate();
	if (aIsSet)
	{
		//Time d(aDueDate.getYear(), aDueDate.getMonth(), aDueDate.getDayOfMonth(), iCreationDate.getHours(), iCreationDate.getMinutes(), iCreationDate.getSeconds(), iCreationDate.getMilliseconds());
		iDueDate = aDueDate;
	}
	else iDueDate = Time(0);
}

bool CKanbanCardComponent::isDone()
{
	return iIsDone;
}

void CKanbanCardComponent::setDone(bool aDone)
{
	iIsDone = aDone;
}

String CKanbanCardComponent::getDueDateAsString(juce::Colour* aColour, bool aLongForm)
{
	String s;
	if (iIsDueDateSet)
	{
		auto ct = Time::getCurrentTime();
		RelativeTime d = Time(iDueDate.getYear(), iDueDate.getMonth(), iDueDate.getDayOfMonth(), 0, 0) - Time(ct.getYear(), ct.getMonth(), ct.getDayOfMonth(), 0, 0);
		double dval = d.inDays();
		if (aColour) *aColour = Colours::lightgrey;

		if (iIsDone)
		{
			s = "done";
		}
		else
		{
			if (dval > 14) s = String((int)ceil(d.inWeeks())) + "wks";
			else if (dval == 0)
			{
				if (aColour) *aColour = Colours::whitesmoke;
				s = "today"; // tdy
			}
			else if (dval > 0 && dval <= 1) s = (aLongForm ? "tomorrow" : "tmrw" );
			else if (dval < 0 && dval >= -1)
			{
				if (aColour) *aColour = Colours::tomato;
				s = (aLongForm ? "yesterday" : "yday" );
			}
			else if (dval < -365)
			{
				if (aColour) *aColour = Colours::red;
				s = String((int)ceil(d.inWeeks()) / 54) + "y";
			}
			else if (dval < -14)
			{
				if (aColour) *aColour = Colours::red;
				s = String((int)ceil(d.inWeeks())) + "wks";
			}
			else
			{
				if ( dval < 0 ) *aColour = Colours::tomato;
				s = String((int)ceil(dval)) + "d";
			}
		}
	}

	return s;
}

void CKanbanCardComponent::setDates(Time& aCreateionDate, Time& aLastUpdateDate)
{
	iCreationDate = aCreateionDate;
	iLastUpdateDate = aLastUpdateDate;
}

juce::Time CKanbanCardComponent::getCreationDate()
{
	return iCreationDate;
}

juce::Time CKanbanCardComponent::getLastUpdateDate()
{
	return iLastUpdateDate;
}

juce::Time CKanbanCardComponent::getDueDate()
{
	return iDueDate;
}

void CKanbanCardComponent::updateLastUpdateDate()
{
	iLastUpdateDate = Time::getCurrentTime();
}

bool CKanbanCardComponent::isEmpty()
{
	return (iLabel.getText().indexOf(KanbanCardComponentDefaultTitle) == 0 &&
		iColorBar == CConfiguration::getColourPalette().getLastColor() &&
		iNotes.isEmpty() && !iIsDueDateSet && iAssigne.getText().isEmpty());
}

void CKanbanCardComponent::setColour(Colour aColor)
{
	updateLastUpdateDate();
	iColorBar = aColor;
	repaint();
}

Colour CKanbanCardComponent::getColour()
{
	return iColorBar;
}

void CKanbanCardComponent::setNotes(const String& aString)
{
	updateLastUpdateDate();
	iNotes = aString;
}

String CKanbanCardComponent::getNotes()
{
	return iNotes;
}

void CKanbanCardComponent::setAssigne(const String & aString)
{
	String s1 = aString.trim();
	getProperties().set("assignee", s1);
	if (s1.isEmpty())
	{
		iAssigne.setText("", NotificationType::dontSendNotification);
		return;
	}

	String out = s1.substring(0, 1);

	int len = KAssigneeLength - 2; // for len=2 set to 0, for len=3 set to 1;

	int spidx = s1.indexOfChar(' ');
	if (spidx == -1) spidx = s1.indexOfChar('.');		
	if (spidx != -1)
	{

		String s2 = s1.substring(spidx + 1);
		s2 = s2.trim();
		if ( s2.isEmpty() ) out += s1.substring(1, 2 + len);
		else out += s2.substring(0, 1 + len);
	}
	else
	{
		out += s1.substring(1, 2 + len);
	}

	if (KAssigneeLength == 3 && out.length() == KAssigneeLength && spidx != -1)
	{
		auto c = out[2];
		out = out.dropLastCharacters(1);
		out = out.toUpperCase();
		out += c;
	}
	iAssigne.setText(out, NotificationType::dontSendNotification);
}

String CKanbanCardComponent::getAssigne()
{
	return getProperties()["assignee"];
}

void CKanbanCardComponent::deselect()
{
	auto pos = Desktop::getMousePosition();
	if (!getScreenBounds().contains(pos))
	{
		iMouseActive = false;
		repaint();
	}
}

String CKanbanCardComponent::toJson()
{
	String l = URL::addEscapeChars(iLabel.getText().toUTF8(), false);
	String c = URL::addEscapeChars(iColorBar.toString().toUTF8(), false);
	String a = URL::addEscapeChars(getProperties()["assignee"], false);
	//String::fromUTF8(URL::removeEscapedChars(url));
	String ret("{ \"text\":\"" + l + "\", \"colour\":\"" + c + "\", \"columnId\":" + String(getOwnerColumnId()));

	String n = iNotes.toUTF8();
	if (n.length() > 0)
	{
		n = URL::addEscapeChars(n, false);
		ret += ", \"notes\":\"" + n + "\"";
	}

	String url = getProperties()["url"];
	if (url.length() > 0)
	{
		url = URL::addEscapeChars(url, false);
		ret += ", \"url\":\"" + url + "\"";
	}

	String tags = getProperties()["tags"];
	if (tags.length() > 0)
	{
		tags = URL::addEscapeChars(tags, false);
		ret += ", \"tags\":\"" + tags + "\"";
	}

	if (a.length() > 0) ret += ", \"assignee\":\"" + a + "\"";

	ret += ", \"dueDateSet\":" + String(iIsDueDateSet ? "true" : "false");
	ret += ", \"isDone\":" + String(iIsDone ? "true" : "false");
	ret += ", \"creationDate\":" + String(iCreationDate.toMilliseconds());
	ret += ", \"dueDate\":" + String(iDueDate.toMilliseconds());
	ret += ", \"lastUpdateDate\":" + String(iLastUpdateDate.toMilliseconds());

	ret += ", \"customProp\":[";
	bool first = true;
	for (auto& p : getProperties())
	{
		String s1 = p.name.toString();
		if (s1 == "assignee" || s1 == "tags" || s1 == "url") continue;
		if (first) first = false;
		else ret += ",";
		ret += "{\"" + URL::addEscapeChars(p.name.toString(), false) + "\":\"" + URL::addEscapeChars(p.value.toString(), false) + "\"}";
	}
	ret += "]";

	ret += " }";
	return ret;
}

int CKanbanCardComponent::getOwnerColumnId() const
{
	if (!iOwner) return -1;
	return iOwner->getOwner().getColumnId();
}

void CKanbanCardComponent::setReadOnly(bool aReadOnly)
{
	iReadOnly = aReadOnly;
}

void CKanbanCardComponent::addListener(Listener * aListener)
{
	jassert(aListener);
	iListeners.addIfNotAlreadyThere(aListener);
}

void CKanbanCardComponent::removeListener(Listener * aListener)
{
	iListeners.removeAllInstancesOf(aListener);
}

Path CKanbanCardComponent::generateUrlPath(Rectangle<int> aBoundaryRect)
{
	Path p;
	auto r2 = aBoundaryRect.reduced(4).translated(-1, 2);
	p.clear();
	p.startNewSubPath(r2.getTopLeft().x + 3, r2.getTopLeft().y);
	p.lineTo(r2.getTopLeft().x, r2.getTopLeft().y);
	p.lineTo(r2.getTopLeft().x, r2.getBottom());
	p.lineTo(r2.getRight(), r2.getBottom());
	p.lineTo(r2.getRight(), r2.getBottom() - 3);

	float aw = 5, aa = 5;
	p.startNewSubPath(r2.getCentreX() + 1, r2.getCentreY());
	p.lineTo(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw);
	p.lineTo(r2.getCentreX() + 1 + aw - aa, r2.getCentreY() - aw);
	p.startNewSubPath(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw);
	p.lineTo(r2.getCentreX() + 1 + aw, r2.getCentreY() - aw + aa);
	p.closeSubPath();
	return p;
}


void CKanbanCardComponent::notifyListeners()
{
	for (auto l : iListeners)
	{
		l->KanbanCardChanged();
	}
}

CKanbanCardComponent * CKanbanCardComponent::getClipboardCard()
{
	return clipboardCard.get();
}

void CKanbanCardComponent::setClipboardCard(CKanbanCardComponent * aCard)
{
	clipboardCard = std::make_unique<CKanbanCardComponent>(nullptr);
	clipboardCard->duplicateDataFrom(*aCard);
}

void CKanbanCardComponent::cleanupClipboardCard()
{
	clipboardCard.reset();
}


void CKanbanCardComponent::showProperties()
{
	iMouseActive = true;
	auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
	CallOutBox* box = new CallOutBox((juce::Component&)std::move(*comp), this->getScreenBounds(), nullptr);
	box->setEnabled(!iReadOnly);
	box->runModalLoop();

	if (comp->wasContentUpdated())
	{
		notifyListeners();
	}

	delete box;

	// this was disabled to let stay the callout window on the screen when changing to another app
	//CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
	//box->setEnabled(!iReadOnly);
	//box->setAlwaysOnTop(true);
}

void CKanbanCardComponent::setChildrenVisibility(bool aHidden)
{
	iLabel.setVisible(!aHidden);
}


