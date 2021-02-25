/*
  ==============================================================================

    CKanbanBoard.h
    Created: 12 Nov 2020 4:50:24pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnComponent.h"

using namespace juce;

//==============================================================================
/*http://davidbau.com/colors/
*/
class CKanbanBoardComponent  : public juce::Component
{
public:
	CKanbanBoardComponent();
    ~CKanbanBoardComponent() override;

	static CKanbanBoardComponent* fromJson(var& aFile, String& aReturnErrorMessage);

	void createDefaultBoard();

    void paint (juce::Graphics&) override;
    void resized() override;

	void updateSize();

	void search(const String& aString);
	void searchClear();

	CKanbanCardComponent* createCard();
	void removeCard(CKanbanCardComponent* aCard);

	bool saveFile(String& aReturnErrorMessage);
	void setFile(File& aFile);
	File& getFile();

private:

	Grid iGrid;

	File iFile;

	OwnedArray< CKanbanCardComponent > iKanbanCards;

	OwnedArray< CKanbanColumnComponent > iKanbanColumns;

	Component iContent;

	Viewport iViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanBoardComponent)
};
