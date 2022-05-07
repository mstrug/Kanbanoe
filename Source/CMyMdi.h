/*
  ==============================================================================

    CMyMdi.h
    Created: 23 Mar 2021 11:15:44pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanBoard.h"


using namespace juce;


class MainComponent;
class CKanbanBoardArchive;


class CMyMdiDocBase : public Component
{
protected:
	String iSearchText;
	Viewport iViewport;
	bool iEdited;
public:
	CMyMdiDocBase();
	virtual ~CMyMdiDocBase();
	void resized() override;
	void paint(juce::Graphics& g) override;
	void setSearchText(const String& aText);
	String& getSearchText();
	String getTabName();
	bool isUnsaved();
	virtual void updateCardsView() { }

public:
	CMyMdiDocBase *iNext, *iPrev;
};


class CMyMdiDoc : public CMyMdiDocBase, public CKanbanBoardComponent::Listener
{
public:
	CMyMdiDoc(CKanbanBoardComponent* board);
	~CMyMdiDoc();
	operator CKanbanBoardComponent*() const;
	CKanbanBoardComponent* getKanbanBoard();
	bool save();
	const String& getFilePath();
	void updateCardsView();

public: // from CKanbanBoardComponent::Listener
	void KanbanBoardChanged();
	void KanbanBoardStored();
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

	void tryToCloseDocumentAsync(Component* component, std::function<void(bool)> callback) override;

	CMyMdiDocBase* getDocByFile(File& aFn);

public:

	CMyMdi(MainComponent& aOwner);

	virtual ~CMyMdi();

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

	void closeAllDocumentsAndVerifyStore();

	void updateDocuments();
};
	

