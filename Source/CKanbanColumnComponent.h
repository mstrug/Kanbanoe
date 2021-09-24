/*
  ==============================================================================

    CKanbanColumnComponent.h
    Created: 9 Nov 2020 5:11:55pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanColumnContentComponent.h"
#include "CKanbanCardComponent.h"

using namespace juce;

class CKanbanBoardComponent;

//==============================================================================
/*
*/
class CKanbanColumnComponent : public juce::Component, public ScrollBar::Listener
{
public:
	CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner);
	~CKanbanColumnComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;
	void mouseUp(const MouseEvent& event) override;
	void mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & details) override;
	void mouseMove(const MouseEvent& event)  override;
	void mouseExit(const MouseEvent& event) override;
	bool keyPressed(const KeyPress& key) override;

	void setActiveFrame(bool aActive);
	void contentUpdated();
	void cardsCoutUpdated();
	CKanbanBoardComponent& kanbanBoard();
	CKanbanColumnContentComponent& cardsLayout();
	void addCard(CKanbanCardComponent* aCard, bool aLoadFromFile = false); // takes ownership of the pointer
	void duplicateCard(const CKanbanCardComponent* aCard);
	void removeCard(CKanbanCardComponent* aCard);
	void removeAllCards();
	void scrollToBottom();
	void scrollToTop();
	void scrollEnsureVisible(CKanbanCardComponent* aCard);

	String getTitle();
	int getColumnId() const;
	bool isColumnDueDateDone() const;
	void setColumnDueDateDone(bool aDueDateDone);
	
	bool isMinimized() const;
	void setMinimized(bool aMinimized, bool aUpdateOwner);

	int getMaxWip() const;
	void setMaxWip(int aWip);

	void setGridItem(const GridItem& aGridItem);
	const GridItem& getGridItem() const;
	bool isGridColumn(int aStartCol, int aEndCol);
	bool isColumnSameSize(const CKanbanColumnComponent& aColumn);
	void moveGridItemRight();

	void updateColumnTitle();

	void setEditMode(bool aLeftEnabled, bool aRightEnabled);
	bool isEditModeRightVisible();
	void setEditModeRightVisible(bool aVisible);
	static int getEditModeMargin();

private:

	Rectangle<int> getLocalBoundsForCardsSection();

	Rectangle<int> getLocalBoundsForEditLeft();
	
	Rectangle<int> getLocalBoundsForEditRight();

	void showSetupMenu();

	void archive();

	String getMinimalDueDate( juce::Colour* aColour = nullptr);

public:
	void decodeGitlabRsp(const String& aData);
	void decodeGitlabStarting();
	void decodeGitlabNotifier(CKanbanCardComponent* aCard);
	void decodeGitlabFinished();

public: // from ScrollBar::Listener

	void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;
	
private: // from ModalComponentManager::Callback

	class BtnMenuHandler : public ModalComponentManager::Callback
	{
		CKanbanColumnComponent& iOwner;
	public:
		BtnMenuHandler(CKanbanColumnComponent& aOwner) : iOwner(aOwner) {}
		void modalStateFinished(int returnValue) override { iOwner.iSetupButton.setState(Button::buttonNormal); }
	};

private:

	class ComponentListenerEditButton : public ComponentListener
	{
		void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized) override;
	} iEditModeButtonListener;

private:

	CKanbanBoardComponent& iOwner;

	int iColumnId;

	bool iMinimizedState;

	bool iIsFrameActive;

	bool iDueDateDone;

	bool iSortedAsc;

	GridItem iGridItem;

	String iColumnTitle;
	Label iTitle;
	Label iTitleCardsCount;
	Label iTitleMinimalDueDate;
	
	ScrollBar iScrollBar;

	DrawableButton iAddCardButton;

	DrawableButton iSetupButton;

	double iProgressBarValue;
	ProgressBar iProgressBar;

	CKanbanColumnContentComponent iViewportLayout;

	bool iMouseTitleIsActive;

	bool iEditMode;

	bool iEditButtonRightVisible;

	DrawableButton iEditModeLeft;
	DrawableButton iEditModeRight;

	bool iGitlabDecodingOngoing;
	Array<CKanbanCardComponent::CKanbanCardData> iTempCardList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnComponent)
};


