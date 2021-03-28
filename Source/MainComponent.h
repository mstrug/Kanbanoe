#pragma once

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnComponent.h"
#include "CKanbanBoard.h"
#include "CTimer.h"
#include "CMyMdi.h"

using namespace juce;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component, public MenuBarModel, public DragAndDropContainer, public ApplicationCommandTarget
{
public:

	enum CommandIDs
	{
		menuFile = 1,
		menuFileNew,
		menuFileOpen,
		menuFileClose,
		menuFileSave,
		menuFileSaveAs,
		menuFileSaveAll,
		menuFileSaveGroup,
		menuFileSaveGroupAs,
		menuFileOpenRecent,
		menuFileExit,
		menuViewArchive,
		menuConfigSearchDynamic,
		menuConfigSearchCaseInsensitive,
		menuHelpAbout,
		menubarSearch,
		menubarSearchClear,
		mdiNextDoc,
		mdiPrevDoc
	};

    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void updateTimer24h();
	void setSearchText(const String& aString, bool aUpdateSearchField);

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

	bool openFile(File& aFn);
	bool saveFile(CKanbanBoardComponent* aBoard);

	bool openGroupFile(File& aFn);
	bool saveGroupFile(File& aFn);

	void textFindCallbackFcn();
	void updateFindCallbacks();

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

	CTimer iTimer24h;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
