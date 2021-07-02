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
class CKanbanBoardArchive;


class CMyMdiDocBase : public Component
{
protected:
	String iSearchText;
	Viewport iViewport;
public:
	CMyMdiDocBase();
	virtual ~CMyMdiDocBase();
	void resized() override;
	void setSearchText(const String& aText);
	String& getSearchText();

public:
	CMyMdiDocBase *iNext, *iPrev;
};


class CMyMdiDoc : public CMyMdiDocBase
{
public:
	CMyMdiDoc(CKanbanBoardComponent* board);
	~CMyMdiDoc();
	operator CKanbanBoardComponent*() const;
	CKanbanBoardComponent* getKanbanBoard();
};



class CMdiDocArchives : public CMyMdiDocBase
{
public:
	CMdiDocArchives(CKanbanBoardArchive* archive);
	~CMdiDocArchives();
};



class CMyMdi : public MultiDocumentPanel
{
	MainComponent& iOwner;

	File iGroupFile;

	bool tryToCloseDocument(Component* component) override;

	CMyMdiDocBase* getDocByFile(File& aFn);

public:

	CMyMdi(MainComponent& aOwner);

	bool addDocument(CMyMdiDocBase* cmp, CMyMdiDocBase * docAfter = nullptr);

	bool addDocument(CKanbanBoardComponent* board, CMyMdiDocBase * docAfter = nullptr);

	void activeDocumentChanged() override;
	
	CMyMdiDocBase* getLastDocument();
	
	void activateNextPrevDocument(bool aNext);

	void activateDocumentByFileName(File & aFn);

	void activateDocumentByTabName(const String& aTabName);

	bool openFile(File & aFn, Array<String>& aRet);

	bool isFileSet();

	File& getFile();

	bool saveFile(File & aFn);

	bool isAlreadyOpened(File& aFn);

	bool isAlreadyOpened(const String& aTabName);
};
	

