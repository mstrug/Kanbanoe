#pragma once

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnComponent.h"
#include "CKanbanBoard.h"

using namespace juce;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public Component, public MenuBarModel, public DragAndDropContainer, public ApplicationCommandTarget
{
	enum CommandIDs
	{
		menuFile = 1,
		menuFileNew,
		menuFileOpen,
		menuFileClose,
		menuFileSave,
		menuFileSaveAs,
		menuFileSaveAll,
		menuFileOpenRecent1,
		menuFileExit,
		menuEditAddCard,
		menuEditViewArchive,
		menuHelpAbout,
		menubarSearch,
		menubarSearchClear,
		mdiNextDoc,
		mdiPrevDoc
	};

public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	ApplicationCommandManager& getApplicationCommandManager();

private: // from MenuBarModel
	
	StringArray getMenuBarNames() override;
	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

private: // from ApplicationCommandTarget

	ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(Array<CommandID>& commands) override;
	void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
	bool perform(const InvocationInfo& info) override;


private: 

	void setSearchText(const String& aString, bool aUpdateSearchField);

	bool openFile(File& aFn);
	bool saveFile(CKanbanBoardComponent* aBoard);

private:
	class testc : public Component
	{
	public:
		testc() { setSize(100, 100); }
		void paint(juce::Graphics& g)
		{
			g.setColour(juce::Colours::red);
			g.drawLine(0, 0, getWidth(), getHeight());
		}

	};
	class CMyMdiDoc : public Component
	{
		Viewport iViewport;
		String iSearchText;
	public:
		CMyMdiDoc(CKanbanBoardComponent* board):iNext(nullptr),iPrev(nullptr) { addAndMakeVisible(iViewport); iViewport.setViewedComponent(board, false); setName(board->getName()); }
		virtual ~CMyMdiDoc() { if (iPrev) iPrev->iNext = iNext; if (iNext) iNext->iPrev = iPrev;	}
		void resized() { auto r(getLocalBounds()); iViewport.setBounds(r); r.removeFromBottom(8); iViewport.getViewedComponent()->setBounds(r); }
		operator CKanbanBoardComponent*() const { return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); }
		CKanbanBoardComponent* getKanbanBoard() { return static_cast<CKanbanBoardComponent*>(iViewport.getViewedComponent()); }
		void setSearchText(const String& aText) { iSearchText = aText; }
		String& getSearchText() { return iSearchText; }
		CMyMdiDoc *iNext, *iPrev;
	};
	class CMyMdi : public MultiDocumentPanel
	{
		MainComponent& iOwner;
		bool tryToCloseDocument(Component* component)
		{
			/*CMyMdiDoc* doc = (CMyMdiDoc*)component;
			if (doc->iPrev) doc->iPrev->iNext = doc->iNext; 
			if (doc->iNext) doc->iNext->iPrev = doc->iPrev;*/
			return true;
		}
	public:
		CMyMdi(MainComponent& aOwner) : iOwner(aOwner) { }
		bool addDocument(CKanbanBoardComponent* board)
		{
			CMyMdiDoc* doc = new CMyMdiDoc(board);
			doc->iPrev = getLastDocument();
			if (doc->iPrev) doc->iPrev->iNext = doc;
			return MultiDocumentPanel::addDocument(doc, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), true);
		}
		void activeDocumentChanged() override
		{
			auto mdi = static_cast<CMyMdiDoc*>(getActiveDocument());
			if (mdi) iOwner.setSearchText(mdi->getSearchText(), true);
		}
		CMyMdiDoc* getLastDocument()
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
		void activateNextPrevDocument(bool aNext)
		{
			CMyMdiDoc* ad = (CMyMdiDoc*) getActiveDocument();
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

	};

private:
    //==============================================================================
    // Your private member variables go here...

	Label iStatuBarR;
	Label iStatuBarL;

	TextEditor iTextSearch;
	Label iLabelSearch;

	CMyMdi iMdiPanel;

	ApplicationCommandManager iCommandManager;

	OwnedArray<CKanbanCardComponent> iKanbanCards;

	std::unique_ptr<MenuBarComponent> iMenuBar;
	
	OwnedArray<CKanbanBoardComponent> iKanbanBoards;

	std::unique_ptr<FileChooser> iFileDialog;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
