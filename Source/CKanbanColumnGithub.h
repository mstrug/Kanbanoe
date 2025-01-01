/*
  ==============================================================================

    CKanbanColumnGithub.h
    Created: 30 Dec 2024 8:12:58am
    Author:  mstrug

  ==============================================================================
*/

#pragma once

#include "CKanbanColumnComponent.h"

//==============================================================================
/* https://docs.gitlab.com/ee/api/issues.html
*/
class CKanbanColumnGithub  : public CKanbanColumnComponent
{
	CKanbanColumnGithub(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, StringRef aUrl, 
		StringRef aToken, StringRef aRepoOwner, StringRef aRepo, StringRef aUser, StringRef aQuery, StringRef aQueryReviews);

public:
	static CKanbanColumnGithub* createWithWizard(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner);

	static CKanbanColumnGithub* createFromJson(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, DynamicObject* aJsonItem);

	~CKanbanColumnGithub() override;

public:

	bool showRefreshMenuEntry() override;

	void refreshThreadWorkerFunction() override;

	void refreshSetupFunction() override;

	int getColumnTypeId() override;

	void outputAdditionalDataToJson(String& aJson) override;

private:

	void requestGithubStart();
	juce::int64 requestGithubPullReviews(StringRef aPullNumber);
	bool decodeGithubResponse(const String& aData);
	juce::int64 decodeGithubReviewsResponse(const String& aData);
	void decodeGithubFinished();

private:

	String iGithubUrl;
	String iGithubToken;
	String iGithubRepoOwner;
	String iGithubRepo;
	String iGithubUser;
	String iGithubQuery;
	String iGithubReviewsQuery;

	Array<CKanbanCardComponent::CKanbanCardData> iTempCardList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnGithub)
};
