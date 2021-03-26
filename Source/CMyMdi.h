/*
  ==============================================================================

    CMyMdi.h
    Created: 23 Mar 2021 11:15:44pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


using namespace juce;


class MainComponent;
class CKanbanBoardComponent;

class CMyMdiDoc : public Component
{
	Viewport iViewport;
	String iSearchText;
public:
	CMyMdiDoc(CKanbanBoardComponent* board);
	virtual ~CMyMdiDoc();
	void resized() override;
	operator CKanbanBoardComponent*() const;
	CKanbanBoardComponent* getKanbanBoard();
	void setSearchText(const String& aText);
	String& getSearchText();

public:
	CMyMdiDoc *iNext, *iPrev;
};


class CMyMdi : public MultiDocumentPanel
{
	MainComponent& iOwner;

	File iGroupFile;

	bool tryToCloseDocument(Component* component) override;

public:

	CMyMdi(MainComponent& aOwner);

	bool addDocument(CKanbanBoardComponent* board) ;
	
	void activeDocumentChanged() override;
	
	CMyMdiDoc* getLastDocument();
	
	void activateNextPrevDocument(bool aNext);

	bool openFile(File & aFn, Array<String>& aRet);

	bool isFileSet();

	File& getFile();

	bool saveFile(File & aFn);
};
	

