/*
  ==============================================================================

    CMyMdi.cpp
    Created: 23 Mar 2021 11:15:44pm
    Author:  michal.strug

  ==============================================================================
*/

#include "CMyMdi.h"
#include "MainComponent.h"
#include "CKanbanBoard.h"


CMyMdiDoc::CMyMdiDoc(CKanbanBoardComponent* board) :iNext(nullptr), iPrev(nullptr) 
{ 
	addAndMakeVisible(iViewport); 
	iViewport.setViewedComponent(board, false); 
	setName(board->getName());
}

CMyMdiDoc::~CMyMdiDoc() 
{ 
	if (iPrev) iPrev->iNext = iNext; 
	if (iNext) iNext->iPrev = iPrev; 
}

void CMyMdiDoc::resized()
{ 
	auto r(getLocalBounds()); 
	iViewport.setBounds(r);
	r.removeFromBottom(8); 
	iViewport.getViewedComponent()->setBounds(r); 
}

CMyMdiDoc::operator CKanbanBoardComponent*() const 
{ 
	return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); 
}

CKanbanBoardComponent* CMyMdiDoc::getKanbanBoard() 
{ 
	return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); 
}

void CMyMdiDoc::setSearchText(const String& aText) 
{ 
	iSearchText = aText; 
}

String& CMyMdiDoc::getSearchText() 
{ 
	return iSearchText;
}







CMyMdi::CMyMdi(MainComponent& aOwner) : iOwner(aOwner) 
{
}

bool CMyMdi::tryToCloseDocument(Component* component)
{
	/*CMyMdiDoc* doc = (CMyMdiDoc*)component;
	if (doc->iPrev) doc->iPrev->iNext = doc->iNext;
	if (doc->iNext) doc->iNext->iPrev = doc->iPrev;*/
	return true;
}

bool CMyMdi::addDocument(CKanbanBoardComponent* board)
{
	CMyMdiDoc* doc = new CMyMdiDoc(board);
	doc->iPrev = getLastDocument();
	if (doc->iPrev) doc->iPrev->iNext = doc;
	return MultiDocumentPanel::addDocument(doc, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), true);
}

void CMyMdi::activeDocumentChanged()
{
	auto mdi = static_cast<CMyMdiDoc*>(getActiveDocument());
	if (mdi) iOwner.setSearchText(mdi->getSearchText(), true);
}

CMyMdiDoc* CMyMdi::getLastDocument()
{
	CMyMdiDoc* ad = (CMyMdiDoc*)getActiveDocument();
	if (ad && ad->iNext != nullptr)
	{
		CMyMdiDoc* i = ad;
		while (i->iNext) i = i->iNext;
		return i;
	}
	else return ad;
}

void CMyMdi::activateNextPrevDocument(bool aNext)
{
	CMyMdiDoc* ad = (CMyMdiDoc*)getActiveDocument();
	if (aNext)
	{
		if (ad->iNext == nullptr)
		{
			CMyMdiDoc* i = ad;
			while (i->iPrev) i = i->iPrev;
			setActiveDocument(i);
		}
		else setActiveDocument(ad->iNext);
	}
	else
	{
		if (ad->iPrev == nullptr)
		{
			CMyMdiDoc* i = ad;
			while (i->iNext) i = i->iNext;
			setActiveDocument(i);
		}
		else setActiveDocument(ad->iPrev);
	}
}

bool CMyMdi::openFile(File & aFn, Array<String>& aRet)
{
	var d = JSON::parse(aFn);
	if (d == var())
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Error parsing file", "Close");
	}
	else
	{
		String errorMessage;

		auto obj = d.getDynamicObject();

		var version = obj->getProperty("version");
		if (version.isString())
		{
			if (version.toString() != "0.1" )
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Not supported file version", "Close");
				return false;
			}
		}
		else
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Not supported file type [version]", "Close");
			return false;
		}

		var files = obj->getProperty("files");
		if (files.isArray())
		{
			auto ar = files.getArray();
			for (auto& i : *ar)
			{
				if (i.isString())
				{
					aRet.add( URL::removeEscapeChars(i));
				}
				else
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Not supported file type [files/array]", "Close");
					return false;
				}
			}
		}
		else
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Not supported file type [files]", "Close");
			return false;
		}

		return true;
	}
	return false;
}

File & CMyMdi::getFile()
{
	return iGroupFile;
}

bool CMyMdi::saveFile(File & aFn)
{
	FileOutputStream f(aFn);
	if (f.openedOk())
	{
		f.setPosition(0);
		f.truncate();

		f << "{\n\"version\":\"0.1\",\n\n\"files\":\n[\n";

		for (int i = 0; i < getNumDocuments(); i++)
		{
			if (i > 0) f << ",\n";
			CMyMdiDoc* ad = (CMyMdiDoc*)getDocument(i);
			f << "\"" + URL::addEscapeChars( ad->getKanbanBoard()->getFile().getFullPathName(), false ) + "\"";
		}

		f << "\n]\n";
		f << "}\n";

		iGroupFile = aFn;
		return true;
	}

	return false;
}
