/*
  ==============================================================================

    CKanbanCardComponent.h
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    properties: url, tags, assignee
*/
using namespace juce;

class CKanbanColumnContentComponent;

class CKanbanCardComponent  : public juce::Component
{
public:
    CKanbanCardComponent(CKanbanColumnContentComponent* aOwner);
    ~CKanbanCardComponent() override;

	void duplicateDataFrom(const CKanbanCardComponent& aCard, bool aDuplicateDates = false);

    void paint (juce::Graphics&) override;
    void resized() override;
	void mouseDown(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void mouseDoubleClick(const MouseEvent& event) override;
	void mouseEnter(const MouseEvent& event) override;
	void mouseMove(const MouseEvent& event) override;
	void mouseExit(const MouseEvent& event) override;

	CKanbanColumnContentComponent* getOwner();
	void setOwner(CKanbanColumnContentComponent* aOwner);

	void openPropertiesWindow();
	void setupFromArchive(const juce::var & aArchive);
	void setupFromJson(const NamedValueSet& aValues, const StringPairArray& aCustomProps );

	void setText(const String& aString);
	String getText();

	void setUrl(const String& aString);
	void setTags(const String& aString);
	
	bool isDueDateSet();
	void setDueDate(bool aIsSet, juce::Time& aDueDate);
	bool isDone();
	void setDone(bool aDone);
	String getDueDateAsString( juce::Colour* aColour = nullptr);
	void setDates(Time& aCreateionDate, Time& aLastUpdateDate);
	juce::Time getCreationDate();
	juce::Time getLastUpdateDate();
	juce::Time getDueDate();
	void updateLastUpdateDate();

	void setColour(Colour aColor);
	Colour getColour();

	void setNotes(const String& aString);
	String getNotes();

	void setAssigne(const String& aString);
	String getAssigne();

	void deselect();
	String toJson();

	int getOwnerColumnId() const;

	void setReadOnly(bool aReadOnly);

public: // clipboard

	static CKanbanCardComponent* getClipboardCard();
	static void setClipboardCard(CKanbanCardComponent* aCard);
	static void cleanupClipboardCard();

private:

	void showProperties();

	void setChildrenVisibility(bool aHidden);


private:

	ComponentDragger iDragger;

	bool iIsDragging;
	
	bool iMouseActive;

	Label iLabel;

	Colour iColorBar;

	String iNotes;

	HyperlinkButton iButtonUrl;

	Line<float> iLineUrl;
	Rectangle<int> iRectUrl;
	bool iIsUrlSet;
	bool iIsUrlMouseActive;

	bool iIsDueDateSet;
	bool iIsDone;
	juce::Time iDueDate;
	juce::Time iCreationDate;
	juce::Time iLastUpdateDate;

	Label iAssigne;
	Rectangle<int> iRectAssigne;

	bool iReadOnly;

	CKanbanColumnContentComponent* iOwner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanCardComponent)
};

const String KanbanCardComponentDragDescription = "KanbanCard";
const String KanbanCardComponentLoadFromFileDescription = "KanbanCard2";

