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
#include "CKanbanBoardArchive.h"



/**********************************************************************************************************/
//
//		CMyMdiDocBase
//
/**********************************************************************************************************/


CMyMdiDocBase::CMyMdiDocBase() :iNext(nullptr), iPrev(nullptr)
{
	addAndMakeVisible(iViewport);
	setName("name");
}

CMyMdiDocBase::~CMyMdiDocBase()
{
	if (iPrev) iPrev->iNext = iNext;
	if (iNext) iNext->iPrev = iPrev;
}

void CMyMdiDocBase::resized()
{
	auto c = iViewport.getViewedComponent();
	if (c)
	{
		auto r(getLocalBounds());
		iViewport.setBounds(r);
		r.removeFromBottom(8);
		c->setBounds(r);
	}
}

void CMyMdiDocBase::setSearchText(const String& aText)
{
	iSearchText = aText;
}

String& CMyMdiDocBase::getSearchText()
{
	return iSearchText;
}



/**********************************************************************************************************/
//
//		CMyMdiDoc
//
/**********************************************************************************************************/


CMyMdiDoc::CMyMdiDoc(CKanbanBoardComponent* board)
{ 
	iViewport.setViewedComponent(board, false); 
	setName(board->getName());
}

CMyMdiDoc::~CMyMdiDoc() 
{ 
}

CMyMdiDoc::operator CKanbanBoardComponent*() const 
{ 
	return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); 
}

CKanbanBoardComponent* CMyMdiDoc::getKanbanBoard() 
{ 
	return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); 
}



/**********************************************************************************************************/
//
//		CMdiDocArchives
//
/**********************************************************************************************************/


CMdiDocArchives::CMdiDocArchives(CKanbanBoardArchive* archive)
{
	iViewport.setViewedComponent(archive, true);
}

CMdiDocArchives::~CMdiDocArchives()
{
}




/**********************************************************************************************************/
//
//		CMyMdi
//
/**********************************************************************************************************/


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

void CMyMdi::tryToCloseDocumentAsync(Component * component, std::function<void(bool)> callback)
{
	callback(tryToCloseDocument(component));
}

CMyMdiDocBase * CMyMdi::getDocByFile(File & aFn)
{
	CMyMdiDocBase* ad = (CMyMdiDocBase*)getActiveDocument();
	if (!ad) return nullptr;

	CMyMdiDoc* ptr = dynamic_cast<CMyMdiDoc*>(ad);
	if (ptr && ptr->getKanbanBoard()->getFile() == aFn)
	{
		return ptr;
	}

	CMyMdiDocBase* i = ad;
	while (i->iNext)
	{
		CMyMdiDoc* ptr = dynamic_cast<CMyMdiDoc*>(i->iNext);
		if (ptr && ptr->getKanbanBoard()->getFile() == aFn)
		{
			return ptr;
		}
		i = i->iNext;
	}

	i = ad;
	while (i->iPrev)
	{
		CMyMdiDoc* ptr = dynamic_cast<CMyMdiDoc*>(i->iPrev);
		if (ptr && ptr->getKanbanBoard()->getFile() == aFn)
		{
			return ptr;
		}
		i = i->iPrev;
	}
	return nullptr;
}

bool CMyMdi::addDocument(CMyMdiDocBase * doc, CMyMdiDocBase * docAfter)
{
	if (docAfter)
	{
		doc->iPrev = getLastDocument();
		//doc->iPrev = docAfter; // todo: requires refactorizatino of MultiDocumentPanel and adding insert document at index function
	}
	else
	{
		doc->iPrev = getLastDocument();
	}
	if (doc->iPrev) doc->iPrev->iNext = doc;
	return MultiDocumentPanel::addDocument(doc, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), true);
}

bool CMyMdi::addDocument(CKanbanBoardComponent* board, CMyMdiDocBase * docAfter)
{
	CMyMdiDoc* doc = new CMyMdiDoc(board);

	if (docAfter)
	{
		doc->iPrev = getLastDocument();
		//doc->iPrev = docAfter; // todo: requires refactorizatino of MultiDocumentPanel and adding insert document at index function
	}
	else
	{
		doc->iPrev = getLastDocument();
	}
	if (doc->iPrev) doc->iPrev->iNext = doc;
	return MultiDocumentPanel::addDocument(doc, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), true);
}

void CMyMdi::activeDocumentChanged()
{
	auto mdi = dynamic_cast<CMyMdiDocBase*>(getActiveDocument());
	if (mdi) iOwner.setSearchText(mdi->getSearchText(), true);
}

CMyMdiDocBase* CMyMdi::getLastDocument()
{
	CMyMdiDocBase* ad = (CMyMdiDocBase*)getActiveDocument();
	if (ad && ad->iNext != nullptr)
	{
		CMyMdiDocBase* i = ad;
		while (i->iNext) i = i->iNext;
		return i;
	}
	else return ad;
}

void CMyMdi::activateNextPrevDocument(bool aNext)
{
	CMyMdiDocBase* ad = (CMyMdiDocBase*)getActiveDocument();
	if (!ad) return;
	if (aNext)
	{
		if (ad->iNext == nullptr)
		{
			CMyMdiDocBase* i = ad;
			while (i->iPrev) i = i->iPrev;
			setActiveDocument(i);
		}
		else setActiveDocument(ad->iNext);
	}
	else
	{
		if (ad->iPrev == nullptr)
		{
			CMyMdiDocBase* i = ad;
			while (i->iNext) i = i->iNext;
			setActiveDocument(i);
		}
		else setActiveDocument(ad->iPrev);
	}
}

void CMyMdi::activateDocumentByFileName(File & aFn)
{
	auto doc = getDocByFile(aFn);
	if (doc)
	{
		setActiveDocument(doc);
	}
}

void CMyMdi::activateDocumentByTabName(const String & aTabName)
{
	for (int i = 0; i < getNumDocuments(); i++)
	{
		CMyMdiDoc* ad = (CMyMdiDoc*)getDocument(i);
		if (ad->getName().compare(aTabName) == 0)
		{
			setActiveDocument(ad);
		}
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

		iGroupFile = aFn;
		return true;
	}
	return false;
}

bool CMyMdi::isFileSet()
{
	return iGroupFile.exists();
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

bool CMyMdi::isAlreadyOpened(File & aFn)
{
	return getDocByFile( aFn ) != nullptr;
}

bool CMyMdi::isAlreadyOpened(const String & aTabName)
{
	for (int i = 0; i < getNumDocuments(); i++)
	{
		CMyMdiDoc* ad = (CMyMdiDoc*)getDocument(i);
		if (ad->getName().compare(aTabName) == 0)
		{
			return true;
		}
	}
	return false;
}
