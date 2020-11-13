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

	void MainComponent::openFile(File& aFn);

private:
    //==============================================================================
    // Your private member variables go here...

	OwnedArray<CKanbanCardComponent> iKanbanCards;

	std::unique_ptr<MenuBarComponent> iMenuBar;
	
	CKanbanBoardComponent *iKanbanBoard;

	std::unique_ptr<FileChooser> iFileDialog;
	
	File iOpenedFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
