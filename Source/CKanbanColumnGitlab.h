/*
  ==============================================================================

    CKanbanColumnGitlab.h
    Created: 6 Oct 2021 9:44:02pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include "CKanbanColumnComponent.h"

//==============================================================================
/*
*/
class CKanbanColumnGitlab  : public CKanbanColumnComponent
{
	CKanbanColumnGitlab(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, StringRef aUrl, StringRef aToken, StringRef aProjectId, StringRef aUsers, StringRef aQuery);

public:
	static CKanbanColumnGitlab* createWithWizard(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner);

	static CKanbanColumnGitlab* createFromJson(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, DynamicObject* aJsonItem);

	~CKanbanColumnGitlab() override;

private:

	bool showRefreshMenuEntry() override;

	void refreshThreadWorkerFunction() override;

	void refreshSetupFunction() override;

	int getColumnTypeId() override;

	void outputAdditionalDataToJson(String& aJson) override;

private:

	void decodeGitlabRsp(const String & aData);
	void decodeGitlabStarting();
	void decodeGitlabNotifier(CKanbanCardComponent* aCard);
	void decodeGitlabFinished();

private:

	String iGitlabUrl;
	String iGitlabToken;
	String iGitlabProjectId;
	StringArray iGitlabUsers;
	String iGitlabQuery;

	Array<CKanbanCardComponent::CKanbanCardData> iTempCardList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnGitlab)
};
