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
class MainComponent  : public Component, public MenuBarModel, public DragAndDropContainer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private: // from MenuBarModel
	
	StringArray getMenuBarNames() override;
	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

private: 

	void openFile(File& aFn);
	bool saveFile(CKanbanBoardComponent* aBoard);

private:

	class CMyMdiDoc : public CKanbanBoardComponent
	{
		Viewport iViewport;
	public:
		CMyMdiDoc(CKanbanBoardComponent* board) { iViewport.setViewedComponent( board ); }
		virtual ~CMyMdiDoc() {}
	};
	class CMyMdi : public MultiDocumentPanel
	{
		bool tryToCloseDocument(Component* component)
		{
			return true;
		}
	public:
		/*bool addDocument(CKanbanBoardComponent* board)
		{
			//addDocument(board, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
		}*/
	};

private:
    //==============================================================================
    // Your private member variables go here...

	Label iStatuBar;

	CMyMdi iMdiPanel;

	OwnedArray<CKanbanCardComponent> iKanbanCards;

	std::unique_ptr<MenuBarComponent> iMenuBar;
	
	OwnedArray<CKanbanBoardComponent> iKanbanBoards;

	std::unique_ptr<FileChooser> iFileDialog;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
