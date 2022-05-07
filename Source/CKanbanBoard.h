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
class CKanbanBoardComponent  : public juce::Component, public CKanbanCardComponent::Listener
{
public:

	struct SArchive
	{
		int iId;
		juce::Time iDate;
		String iName;
		StringArray iKanbanCards;
	};

public:
	CKanbanBoardComponent();
    ~CKanbanBoardComponent() override;

	static CKanbanBoardComponent* fromJson(var& aFile, String& aReturnErrorMessage);

	void createDefaultBoard();

    void paint (juce::Graphics&) override;
    void resized() override;

	void updateSize();
	void updateColumnSize(CKanbanColumnComponent* aColumn, bool aMinimized);
	void updateComunsTitles();

	void search(const String& aString);
	void searchClear();
	void updateSearch();

	CKanbanCardComponent* createCard();
	void removeCard(CKanbanCardComponent* aCard);
	void updateCardsView();

	bool archiveColumn(CKanbanColumnComponent* aColumn, const String& aArchiveName, bool aClearColumn);
	void logArchives();

	bool saveFile(String& aReturnErrorMessage);
	void setFile(File& aFile);
	File& getFile();
	bool isFileSet();

	void setColumnsEditor(bool aEnabled);
	bool isColumnsEditorEnabled();
	void addColumn(CKanbanColumnComponent* aColumn, bool aBefore);
	void removeColumn(CKanbanColumnComponent* aColumn);

	const Array< CKanbanCardComponent* > getCardsForColumn(CKanbanColumnComponent* aColumn);

	const OwnedArray< CKanbanColumnComponent >& getColumns();

	const OwnedArray< CKanbanBoardComponent::SArchive >& getArchives();

	static bool fromJsonCardList(juce::var& aObject, CKanbanBoardComponent* aKanbanBoard, String& aReturnErrorMessage, SArchive* aArchiveObject, CKanbanCardComponent* aCard = nullptr);

	static bool fromJsonCard(const juce::DynamicObject* aObject, CKanbanBoardComponent* aKanbanBoard, String& aReturnErrorMessage, SArchive* aArchiveObject, bool aLoadFromFile, CKanbanCardComponent* aCard = nullptr);

	struct Listener
	{
		virtual void KanbanBoardChanged() = 0;
		virtual void KanbanBoardStored() = 0;
	};
	void addListener(Listener* aListener);
	void removeListener(Listener* aListener);

	void setParentViewport(Viewport* aParentViewport);
	Viewport* getParentViewport();

public: // from CKanbanCardComponent::Listener

	void KanbanCardChanged();

private:

	int updateGridWidth();

	bool isColumnLastInGrid(CKanbanColumnComponent* aColumn);

	bool isColumnHalfBeforeFull(CKanbanColumnComponent * aColumn);

	bool isColumnNextInGridSameSize(CKanbanColumnComponent* aColumn);

private:

	Grid iGrid;

	Grid iEditGrid;

	File iFile;

	OwnedArray< CKanbanCardComponent > iKanbanCards;

	OwnedArray< CKanbanColumnComponent > iKanbanColumns;
	
	OwnedArray< SArchive > iArchive;

	int iGridWidth;

	String iSearchText;

	bool iColumnsEditorEnabled;

	Listener* iListener; // not owned

	Viewport* iParentViewport; // not owned

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanBoardComponent)
};
