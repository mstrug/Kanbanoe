/*
  ==============================================================================

    CKanbanBoardArchive.h
    Created: 29 Mar 2021 1:35:44am
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


using namespace juce;

class CKanbanBoardComponent;

//==============================================================================
/*
*/
class CKanbanBoardArchive  : public juce::Component, public TableListBoxModel
{
public:
    CKanbanBoardArchive(CKanbanBoardComponent &aKanbanBoard );
    ~CKanbanBoardArchive() override;

    void paint (juce::Graphics&) override;
    void resized() override;

public: // from TableListBoxModel

	int getNumRows() override;

	void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

	void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	
	Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

private:

	CKanbanBoardComponent &iKanbanBoard;

	TableListBox iTable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanBoardArchive)
};
