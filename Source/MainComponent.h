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
		menuViewColumnsEdit,
		menuConfigSearchDynamic,
		menuConfigSearchCaseInsensitive,
		menuConfigAutosave,
		menuConfigCardViewComplex,
		menuHelpAbout,
		menuHelpCheckUpdate,
		menubarSearch,
		menubarSearchClear,
		mdiNextDoc,
		mdiPrevDoc,
		statusbarMessage
	};

    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	bool keyPressed(const KeyPress& key) override;

	void updateTimer24h();
	void setSearchText(const String& aString, bool aUpdateSearchField);
	void showStatusBarMessage(StringRef aString);
	void requestedApplicationExit();

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

	bool openGroupFile(File& aFn);
	bool saveGroupFile(File& aFn);

	void textFindCallbackFcn();
	void updateFindCallbacks();

	void checkForUpdates();

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
	
	SharedResourcePointer<TooltipWindow> iTooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
