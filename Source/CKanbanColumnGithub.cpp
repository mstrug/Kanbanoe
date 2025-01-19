/*
  ==============================================================================

    CKanbanColumnGithub.cpp
    Created: 30 Dec 2024 8:12:58am
    Author:  mstrug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanColumnGithub.h"
#include "CConfiguration.h"
#include "CKanbanBoard.h"


//==============================================================================
CKanbanColumnGithub::CKanbanColumnGithub(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, StringRef aUrl, 
	StringRef aToken, StringRef aRepoOwner, StringRef aRepo, StringRef aUser, StringRef aQuery, StringRef aQueryReviews) : CKanbanColumnComponent(aColumnId, aTitle, aOwner, true),
		iGithubUrl(aUrl), iGithubToken(aToken), iGithubRepoOwner(aRepoOwner), iGithubRepo(aRepo), iGithubUser(aUser),
		iGithubQuery(aQuery), iGithubReviewsQuery(aQueryReviews)
{
}

CKanbanColumnGithub::~CKanbanColumnGithub()
{
}

bool CKanbanColumnGithub::showRefreshMenuEntry()
{
	return true;
}

void CKanbanColumnGithub::refreshThreadWorkerFunction()
{
	requestGithubStart();
}


int CKanbanColumnGithub::getColumnTypeId()
{
	return 2;
}

void CKanbanColumnGithub::outputAdditionalDataToJson(String & aJson)
{
	aJson << ", \"githubUrl\":\"" + URL::addEscapeChars(iGithubUrl, false) + "\"";
	aJson << ", \"githubToken\":\"" + URL::addEscapeChars(iGithubToken, false) + "\"";
	aJson << ", \"githubRepoOwner\":\"" + URL::addEscapeChars(iGithubRepoOwner, false) + "\"";
	aJson << ", \"githubRepo\":\"" + URL::addEscapeChars(iGithubRepo, false) + "\"";
	aJson << ", \"githubUser\":\"" + URL::addEscapeChars(iGithubUser, false) + "\"";
	aJson << ", \"githubQuery\":\"" + URL::addEscapeChars(iGithubQuery, false) + "\"";
	aJson << ", \"githubReviewsQuery\":\"" + URL::addEscapeChars(iGithubReviewsQuery, false) + "\"";
}


// return: 
//		-1 : canceled
//		0 : ok
//		1 : wrong data entered, retry by showing the window
static int createAndShowWizardWindow(String& aUrl, String& aToken, String& aOwner, String& aRepo, String& aUser, String& aQuery, String& aReviewsQuery, bool& aTestConn, bool edit = false)
{
	AlertWindow aw( edit ? "Edit github integration column" : "Create github integration column", 
					edit ? "Edit column information" : "Provide new column information", AlertWindow::QuestionIcon);
	aw.addTextEditor("text_url", aUrl, "Github URL:");
	aw.addTextEditor("text_token", aToken, "Github API token:");
	aw.addTextEditor("text_owner", aOwner, "Repo owner:");
	aw.addTextEditor("text_repo", aRepo, "Repo:");
	aw.addTextEditor("text_user", aUser, "User:");
	aw.addTextEditor("text_query", aQuery, "Github API lust PRs query:");
	aw.addTextEditor("text_query2", aReviewsQuery, "Github API PR reviews query:");

	ToggleButton tb("Test connection");
	tb.setComponentID("checkbox");
	tb.setSize(250, 36);
	tb.setName("");
	tb.setToggleState(aTestConn, NotificationType::dontSendNotification);
	aw.addCustomComponent(&tb);

	aw.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
	aw.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

	if (aw.runModalLoop() != 0) // is they picked 'ok'
	{
		auto text_url = aw.getTextEditorContents("text_url");
		auto text_token = aw.getTextEditorContents("text_token");
		auto text_owner = aw.getTextEditorContents("text_owner");
		auto text_repo = aw.getTextEditorContents("text_repo");
		auto text_user = aw.getTextEditorContents("text_user");
		auto text_query = aw.getTextEditorContents("text_query");
		auto text_query2 = aw.getTextEditorContents("text_query2");

		String errorStr;  
		if (text_url.isEmpty()) errorStr = "Github URL cannot be empty.";
		else if (text_token.isEmpty()) errorStr = "Github API token cannot be empty.";
		else if (text_owner.isEmpty()) errorStr = "Repo owner cannot be empty.";
		else if (text_repo.isEmpty()) errorStr = "Repo cannot be empty.";
		else if (text_user.isEmpty()) errorStr = "Github user cannot be empty.";
		else if (text_query.isEmpty()) errorStr = "Github API query cannot be empty.";
		else if (text_query2.isEmpty()) errorStr = "Github API query cannot be empty.";
		else if (!text_query.contains("{URL}") && !text_query2.contains("{URL}")) errorStr = "Github API query must contain {URL} placeholder.";
		else if (!text_query.contains("{OWNER}") && !text_query2.contains("{OWNER}")) errorStr = "Github API query must contain {OWNER} placeholder.";
		else if (!text_query.contains("{REPO}") && !text_query2.contains("{REPO}")) errorStr = "Github API query must contain {REPO} placeholder.";
		else if (!text_query2.contains("{PULLNUMBER}")) errorStr = "Github API PR reviews query must contain {PULLNUMBER} placeholder.";

		aUrl = text_url;
		aToken = text_token;
		aOwner = text_owner;
		aRepo = text_repo;
		aUser = text_user;
		aQuery = text_query;
		aReviewsQuery = text_query2;
		aTestConn = tb.getToggleState();

		if (!errorStr.isEmpty())
		{
			AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Information", errorStr, "Ok");
			return 1;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

static int decodeHttpStatusCode(StringRef msg, String& text_code) {
	StringArray lines = StringArray::fromLines(msg);
	if (lines.size() > 0) {
		StringArray data = StringArray::fromTokens(lines[0], " ");
		if (data.size() >= 3) {
			int code = data[1].getIntValue();
			text_code = data.joinIntoString(" ", 2);
			return code;
		}
	}
	return -1;
}

static bool dropHttpHeaders(String& msg, String& body_start) {
	int idx = msg.indexOf("\r\n\r\n");
	if (idx >= 0)
	{
		body_start = msg.substring(idx);
		return true;
	}
	return false;
}

static bool invokeConnection(StringRef aQuery, StringRef aToken, String& aOutput, int& aCurlErrorCode)
{
	ChildProcess cp;
	String curls = CConfiguration::getValue("curl");
	if (curls.isEmpty())
	{
		aOutput = "curl bad path";
		aCurlErrorCode = -2;
		return false;
	}
	String cmd = curls + " -i -L --header \"Accept: application/vnd.github+json\" --header \"X-GitHub-Api-Version: 2022-11-28\" --header \"Authorization: Bearer " + aToken + "\" \"" + aQuery + "\"";
	//Logger::outputDebugString("curl cmd: " + cmd);
	if (cp.start(cmd, ChildProcess::wantStdOut))
	{
		String out = cp.readAllProcessOutput();
		//Logger::outputDebugString("out: " + out);

		String http_code_str;
		int http_code = decodeHttpStatusCode(out, http_code_str);
		uint32 ec = cp.getExitCode();
		aCurlErrorCode = ec;

		if (http_code == 200 && ec == 0) 
		{
			if (dropHttpHeaders(out, aOutput)) 
			{
				return true;
			}
			else 
			{
				aOutput = "Malformed HTTP message body";
				aCurlErrorCode = -4;
				return false;
			}
		}
		else if (ec == 0)
		{
			aOutput = "  HTTP error code: " + String(http_code) + " " + http_code_str;
			aCurlErrorCode = -4;
			return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		aCurlErrorCode = -3;
		aOutput = "Process start failed";
		return false;
	}
}

static bool invokeTestConnection(StringRef aUrl, StringRef aToken, StringRef aOwner, StringRef aRepo, StringRef aQuery, String& aOutput, int& aCurlErrorCode)
{
	String q(aQuery);
	q = q.replace("{URL}", aUrl);
	q = q.replace("{OWNER}", aOwner);
	q = q.replace("{REPO}", aRepo);

	int ec = 0;
	String out;
	bool ret = invokeConnection(q, aToken, out, ec);
	aCurlErrorCode = ec;
	if (ec != 0)
	{
		aOutput = out;
	}
	return ret;
}

static int createAndShowWizardWindowTestConnection(StringRef aUrl, StringRef aToken, StringRef aOwner, StringRef aRepo, StringRef aQuery, String& aOutput)
{
	class DemoBackgroundThread : public ThreadWithProgressWindow
	{
	public:
		StringRef url;
		StringRef token;
		StringRef owner;
		StringRef repo;
		StringRef query;
		bool invokeResult;
		String invokeOutput;
		int curlErrorCode;

		DemoBackgroundThread() : ThreadWithProgressWindow("Verification", true, true)
		{
			invokeResult = false;
			curlErrorCode = 0;
			setProgress(-1.0);
			setStatusMessage("Establishing connection...");
		}

		void run() override
		{
			invokeResult = invokeTestConnection(url, token, owner, repo, query, invokeOutput, curlErrorCode);
		}
	} *dw = new DemoBackgroundThread();
	dw->url = aUrl;
	dw->token = aToken;
	dw->owner = aOwner;
	dw->repo = aRepo;
	dw->query = aQuery;

	if (dw->runThread())
	{
		int ret;
		if (dw->invokeResult)
		{ // ok
			ret = 0;
		}
		else
		{ // not connected
			aOutput = dw->invokeOutput.trimStart();
			ret = dw->curlErrorCode;
		}
		delete dw;
		return ret;
	}
	else
	{ // user pressed cancel
		delete dw;
		return -1;
	}
}

CKanbanColumnGithub * CKanbanColumnGithub::createWithWizard(int aColumnId, const String & aTitle, CKanbanBoardComponent & aOwner)
{
	String url = "https://api.github.com";
	String token = "";
	String owner = "repo-owner";
	String repo = "repo-name";
	String user = "github-login";
	String query = "{URL}/repos/{OWNER}/{REPO}/pulls?sort=updated&direction=desc&state=all&per_page=15";
	String query2 = "{URL}/repos/{OWNER}/{REPO}/pulls/{PULLNUMBER}/reviews";
	bool testConn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, owner, repo, user, query, query2, testConn)) >= 0)
	{
		if (res == -1) return nullptr; // user clicked cancel
		if (res == 0)
		{
			if (testConn)
			{
				String out_msg;
				int res2 = createAndShowWizardWindowTestConnection(url, token, owner, repo, query, out_msg);
				if (res2 == 0)
				{ // all ok -> create window
				}
				else if (res2 == -1)
				{ // user pressed cancel -> show wizard again
					continue;
				}
				else
				{ // else res2 > 0 -> show error message and then wizard again
				  // res2 = -2 -> bad path
				  // res2 = -3 -> curl connection failed (sub process start failed)

					String msg = "Test connection failed. Please verify your Github settings.\n";
					if (res2 > 0)
					{
						msg += "Curl error code: " + String(res2);
					}
					else 
					{
						msg += "Invoking curl error code: " + String(res2) + "\n" + out_msg;
					}

					AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Information", msg, "Ok");
					continue;
				}
			}
			
			return new CKanbanColumnGithub(aColumnId, aTitle, aOwner, url, token, owner, repo, user, query, query2);
		}
	}
	
	return nullptr;
}

CKanbanColumnGithub * CKanbanColumnGithub::createFromJson(int aColumnId, const String & aTitle, CKanbanBoardComponent & aOwner, DynamicObject * aJsonItem)
{
	var url = aJsonItem->getProperty("githubUrl");
	var token = aJsonItem->getProperty("githubToken");
	var owner = aJsonItem->getProperty("githubRepoOwner");
	var repo = aJsonItem->getProperty("githubRepo");
	var user = aJsonItem->getProperty("githubUser");
	var query = aJsonItem->getProperty("githubQuery");
	var query2 = aJsonItem->getProperty("githubReviewsQuery");

	if (url.isString() && token.isString() && owner.isString() && repo.isString() && query.isString() && query2.isString())
	{
		CKanbanColumnGithub *col = new CKanbanColumnGithub(aColumnId, aTitle, aOwner, URL::removeEscapeChars(url), 
			URL::removeEscapeChars(token), URL::removeEscapeChars(owner), URL::removeEscapeChars(repo), URL::removeEscapeChars(user),
			URL::removeEscapeChars(query), URL::removeEscapeChars(query2));
		return col;
	}

	return nullptr;
}

bool CKanbanColumnGithub::decodeGithubResponse(const String& aData)
{
	/* {
		File myTargetFile("c:\\tmp\\aa.json");
		myTargetFile.appendText(aData);
	}*/

	var d = JSON::parse(aData);
	if (d != var())
	{
		String errorMessage;
		auto ar = d.getArray();
		if (!ar->isEmpty())
		{
			for (auto& i : *ar)
			{
				auto dd = i.getDynamicObject();
				auto& draft = dd->getProperty("draft");

				bool merged = false;
				bool merged_prop_exists = dd->hasProperty("merged");
				auto& merged_prop = dd->getProperty("draft");
				if (!merged_prop_exists || (merged_prop_exists && !merged_prop.isBool()))
				{
					auto& state = dd->getProperty("state");
					if (state == "closed") {
						merged = true;
					}
				}
				else 
				{
					merged = (bool)merged_prop;
				}

				if (draft.isBool() && !draft) {
					auto& id = dd->getProperty("number");
					//Logger::outputDebugString("PR number: " + id.toString() + " Merged: " + (merged ? "true" : "false"));

					auto& url = dd->getProperty("html_url");
					auto& title = dd->getProperty("title");
					auto& cr = dd->getProperty("created_at");
					auto& up = dd->getProperty("updated_at");
					auto& notes = dd->getProperty("body");

					auto& user = dd->getProperty("user");
					auto du = user.getDynamicObject();
					auto& user_login = du->getProperty("login");

					auto& labels = dd->getProperty("labels");
					auto dl = labels.getArray();
					String tags;
					//for (auto& j : *dl) tags += j.toString() + " ";


					juce::int64 last_pr_update_time = Time::fromISO8601(up.toString()).toMilliseconds();
					juce::int64 user_added_review_time = 0;
					if (!merged) {
						// get user review for this PR
						user_added_review_time = requestGithubPullReviews(id.toString()); // todo: optimize these requests
					}

					if (user_added_review_time == 0 || user_added_review_time < last_pr_update_time)
					{
						// if user_added_review_time == last_pr_update_time that means user review was PR last update

						// add card only if there is no review added from github user yet
						// or review was added before PR last update
						// or pr is merged (updating only colour)

						CKanbanCardComponent::CKanbanCardData c;

						c.values.set("text", "#" + id.toString() + " " + title);
						c.values.set("notes", notes);
						c.values.set("colour", -1);
						c.values.set("url", url);
						c.values.set("tags", tags);
						c.values.set("assignee", user_login);

						c.values.set("dueDateSet", false);

						c.values.set("creationDate", Time::fromISO8601(cr.toString()).toMilliseconds());
						c.values.set("lastUpdateDate", last_pr_update_time);

						c.customProps.set("id", id.toString());
						c.customProps.set("remoteLastUpdateDate", String(last_pr_update_time));
						c.customProps.set("userReviewAddedDate", String(user_added_review_time));
						c.customProps.set("merged", (merged ? "true" : "false"));

						iTempCardList.add(c);
					}
					
				}
			}
		}

		return true;
	}
	return false;
}

// returns true when user review has been found
juce::int64 CKanbanColumnGithub::decodeGithubReviewsResponse(const String& aData)
{
	int64 max_time = 0;
	var d = JSON::parse(aData);
	if (d != var())
	{
		String errorMessage;
		auto ar = d.getArray();
		if (!ar->isEmpty())
		{
			for (auto& i : *ar)
			{
				auto dd = i.getDynamicObject();
				auto& user = dd->getProperty("user");

				if (user.isObject()) {
					auto du = user.getDynamicObject();

					auto& login = du->getProperty("login");
					if (login.toString() != iGithubUser) {
						continue;
					}
					auto& sub = dd->getProperty("submitted_at");

					int64 current_time = Time::fromISO8601(sub.toString()).toMilliseconds();
					if (current_time > max_time) {
						max_time = current_time;
					}
				}
			}
		}
	}
	return max_time;
}

void CKanbanColumnGithub::requestGithubStart()
{
	iRefreshOngoing = true;
	iTempCardList.clear();

	String q(iGithubQuery);
	q = q.replace("{URL}", iGithubUrl);
	q = q.replace("{OWNER}", iGithubRepoOwner);
	q = q.replace("{REPO}", iGithubRepo);
	//Logger::outputDebugString("built query: " + q);

	String out, err;
	int ec = 0;

	if (invokeConnection(q, iGithubToken, out, ec))
	{
		if (!decodeGithubResponse(out))
		{
			out = "Decoding json failed.";
			ec = -4;
		}
	}
	if (ec != 0)
	{
		err = String(ec) + " " + out;
	}

	const MessageManagerLock mml(Thread::getCurrentThread());
	if (!mml.lockWasGained())
	{
		Thread::getCurrentThread()->wait(1100); // retry
		if (!mml.lockWasGained())
		{
			iRefreshOngoing = false;
			return;
		}
	}

	// ui update
	decodeGithubFinished();

	iProgressBar.setVisible(false);
	iRefreshOngoing = false;

	if (!err.isEmpty())
	{
		CConfiguration::showStatusbarMessage("Refresh error: " + err);
	}
	else
	{
		CConfiguration::showStatusbarMessage("Refresh success");
	}
}

juce::int64 CKanbanColumnGithub::requestGithubPullReviews(StringRef aPullNumber)
{
	jassert(iRefreshOngoing);

	String q(iGithubReviewsQuery);
	q = q.replace("{URL}", iGithubUrl);
	q = q.replace("{OWNER}", iGithubRepoOwner);
	q = q.replace("{REPO}", iGithubRepo);
	q = q.replace("{PULLNUMBER}", aPullNumber);
	//Logger::outputDebugString("built query reviews: " + q);

	String out, err;
	int ec = 0;

	if (invokeConnection(q, iGithubToken, out, ec))
	{
		return decodeGithubReviewsResponse(out);
	}
	if (ec != 0)
	{
		err = String(ec) + " " + out;
	}
	return 0;
}

void CKanbanColumnGithub::decodeGithubFinished()
{
	for (auto& c : iTempCardList)
	{
		auto cards = iOwner.getCardsByNameAndUrl( c.values["text"].toString(), c.values["url"].toString());
		bool foundCard = false;

		for (int i = 0; i < cards.size(); i++ )
		{
			if (cards[i]->getProperties()["id"] == c.customProps["id"]) {
				foundCard = true;
				if (!cards[i]->getOwner()->getOwner().isColumnDueDateDone()) {

					bool changed = false;

					String date = c.customProps["remoteLastUpdateDate"];
					if (cards[i]->getCreationDate() != Time(c.values["creationDate"]) || cards[i]->getLastUpdateDate() < Time(date.getLargeIntValue()))
					{
						changed = true;
						cards[i]->setDates(Time(c.values["creationDate"]), Time(c.values["lastUpdateDate"]));
					}
					if (cards[i]->isDueDateSet() != (bool)c.values["dueDateSet"] || cards[i]->getDueDate() != Time(c.values["dueDate"]))
					{
						changed = true;
						cards[i]->setDueDate(c.values["dueDateSet"], Time(c.values["dueDate"]));
					}
					if (cards[i]->getAssigne() != c.values["assignee"].toString())
					{
						changed = true;
						cards[i]->setAssigne(c.values["assignee"]);
					}

					if (c.customProps["merged"] == "true") 
					{
						cards[i]->setColour(CConfiguration::getColourPalette().getColor(2)); // green
					}
					else if (changed)
					{
						cards[i]->setColour(CConfiguration::getColourPalette().getColor(1)); // yellow
					}

					//if (c.values.contains("colour")) cards[i]->setColour(Colour::fromString(c.values["colour"].toString()));
					//if (c.values.contains("colour")) cards[i]->setColour(CConfiguration::getColourPalette().getColor(c.values["colour"]));
					break;
				}
				else if (c.customProps["merged"] == "true") {
					cards[i]->setColour(CConfiguration::getColourPalette().getColor(2)); // green
					break;
				}
			}
			
		}

		if (!foundCard && c.customProps["merged"] == "true") 
		{
			// mark that the card was found, so it will not be created
			foundCard = true;
		}

		if (!foundCard)
		{
			CKanbanCardComponent* card = new CKanbanCardComponent(nullptr);
			card->setupFromJson(c.values, c.customProps);
			iViewportLayout.createNewCard(card, true, false);
			delete card;
		}
	}

	auto ar = iOwner.getCardsForColumn(this);
	for (int i = ar.size() - 1; i >= 0; --i)
	{
		bool foundCard = false;
		for (auto& c : iTempCardList)
		{
			if (ar[i]->getText() == c.values["text"].toString() &&
				ar[i]->getUrl() == c.values["url"].toString() &&				
				ar[i]->getProperties()["id"] == c.customProps["id"])
			{
				foundCard = true;
				break;
			}
		}
		if (!foundCard)
		{
			removeCard(ar[i]);
		}
	}

	iTempCardList.clear();

	iSortedAsc = false;
	iViewportLayout.sortCardsByDueDate(iSortedAsc);
}


void CKanbanColumnGithub::refreshSetupFunction()
{
	String url = iGithubUrl;
	String token = iGithubToken;
	String owner = iGithubRepoOwner;
	String repo = iGithubRepo;
	String user = iGithubUser;
	String query = iGithubQuery;
	String query2 = iGithubReviewsQuery;
	bool conn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, owner, repo, user, query, query2, conn, true)) >= 0)
	{
		if (res == -1) return; // user clicked cancel
		if (res == 0)
		{
			if (conn)
			{
				String out_msg;
				int res2 = createAndShowWizardWindowTestConnection(url, token, owner, repo, query, out_msg);
				if (res2 == 0)
				{ // all ok -> create window
				}
				else if (res2 == -1)
				{ // user pressed cancel -> show wizard again
					continue;
				}
				else
				{ // else res2 > 0 -> show error message and then wizard again
					String msg = "Test connection failed. Please verify your Github settings.\n";
					if (res2 > 0)
					{
						msg += "Curl error code: " + String(res2);
					}
					else
					{
						msg += "Invoking curl error code: " + String(res2) + "\n" + out_msg;
					}

					AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Information", msg, "Ok");
					continue;
				}
			}

			iGithubUrl = url;
			iGithubToken = token;
			iGithubRepoOwner = owner;
			iGithubRepo = repo;
			iGithubUser = user;
			iGithubQuery = query;
			iGithubReviewsQuery = query2;
			return;
		}
	}
}

