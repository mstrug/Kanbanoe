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
CKanbanColumnGithub::CKanbanColumnGithub(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, StringRef aUrl, StringRef aToken, StringRef aRepoOwner, StringRef aRepo, StringRef aQuery) : CKanbanColumnComponent(aColumnId, aTitle, aOwner, true), iGithubUrl(aUrl), iGithubToken(aToken), iGithubRepoOwner(aRepoOwner), iGithubRepo(aRepo), iGithubQuery(aQuery)
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
	decodeGitlabStarting_v2();
}


int CKanbanColumnGithub::getColumnTypeId()
{
	return 1;
}

void CKanbanColumnGithub::outputAdditionalDataToJson(String & aJson)
{
	aJson << ", \"githubUrl\":\"" + URL::addEscapeChars(iGithubUrl, false) + "\"";
	aJson << ", \"githubToken\":\"" + URL::addEscapeChars(iGithubToken, false) + "\"";
	aJson << ", \"githubRepoOwner\":\"" + URL::addEscapeChars(iGithubRepoOwner, false) + "\"";
	aJson << ", \"githubRepo\":\"" + URL::addEscapeChars(iGithubRepo, false) + "\"";
	aJson << ", \"githubQuery\":\"" + URL::addEscapeChars(iGithubQuery, false) + "\"";
}


// return: 
//		-1 : canceled
//		0 : ok
//		1 : wrong data entered, retry by showing the window
static int createAndShowWizardWindow(String& aUrl, String& aToken, String& aOwner, String& aRepo, String& aQuery, bool& aTestConn, bool edit = false)
{
	AlertWindow aw( edit ? "Edit github integration column" : "Create github integration column", 
					edit ? "Edit column information" : "Provide new column information", AlertWindow::QuestionIcon);
	aw.addTextEditor("text_url", aUrl, "Github URL:");
	aw.addTextEditor("text_token", aToken, "Github API token:");
	aw.addTextEditor("text_owner", aOwner, "Repo owner:");
	aw.addTextEditor("text_repo", aRepo, "Repo:");
	//aw.addTextEditor("text_users", aUsers, "API query user name list:");
	//aw.addTextEditor("text_duedates", aDueDates, "API query due date list:");
	aw.addTextEditor("text_query", aQuery, "Github API query (evaluated for each user):");
	
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
		auto text_query = aw.getTextEditorContents("text_query");

		String errorStr;  
		if (text_url.isEmpty()) errorStr = "Github URL cannot be empty.";
		else if (text_token.isEmpty()) errorStr = "Github API token cannot be empty.";
		else if (text_owner.isEmpty()) errorStr = "Repo owner cannot be empty.";
		else if (text_repo.isEmpty()) errorStr = "Repo cannot be empty.";
		else if (text_query.isEmpty()) errorStr = "Github API query cannot be empty.";
		else if (!text_query.contains("{URL}")) errorStr = "Github API query must contain {URL} placeholder.";
		else if (!text_query.contains("{OWNER}")) errorStr = "Github API query must contain {OWNER} placeholder.";
		else if (!text_query.contains("{REPO}")) errorStr = "Github API query must contain {REPO} placeholder.";

		aUrl = text_url;
		aToken = text_token;
		aOwner = text_owner;
		aRepo = text_repo;
		aQuery = text_query;
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

static bool invokeConnection(StringRef aUrl, StringRef aToken, StringRef aOwner, StringRef aRepo, StringRef aQuery, String& aOutput, int& aCurlErrorCode)
{
	String q(aQuery);
	q = q.replace("{URL}", aUrl);
	q = q.replace("{OWNER}", aOwner);
	q = q.replace("{REPO}", aRepo);

	ChildProcess cp;
	String curls = CConfiguration::getValue("curl");
	if (curls.isEmpty())
	{
		aOutput = "curl bad path";
		aCurlErrorCode = -2;
		return false;
	}
	Logger::outputDebugString("query: " + q);
	String process = curls + " -L --header \"Accept: application/vnd.github+json\" --header \"X-GitHub-Api-Version: 2022-11-28\" --header \"Authorization: Bearer " + aToken + "\" \"" + q + "\"";
	Logger::outputDebugString("process: " + process);
	if (cp.start(process, ChildProcess::wantStdOut))
	{
		String out = cp.readAllProcessOutput();
		Logger::outputDebugString("curl output: " + out);
		uint32 ec = cp.getExitCode();
		aCurlErrorCode = ec;
		if (ec == 0)
		{
			aOutput = out + "HTTP/1.1 200 OK";
			return true;
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

static int createAndShowWizardWindowTestConnection(StringRef aUrl, StringRef aToken, StringRef aOwner, StringRef aRepo, StringRef aQuery)
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
			String out;
			invokeResult = invokeConnection(url, token, owner, repo, query, out, curlErrorCode);
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
			ret = dw->curlErrorCode; // > 0 ? dw->curlErrorCode : 9999;
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
	String query = "{URL}/repos/{OWNER}/{REPO}/pulls?sort=updated&direction=asc";
	bool testConn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, owner, repo, query, testConn)) >= 0)
	{
		if (res == -1) return nullptr; // user clicked cancel
		if (res == 0)
		{
			if (testConn)
			{
				int res2 = createAndShowWizardWindowTestConnection(url, token, owner, repo, query);
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

					String msg = "Test connection failed. You need to verify your Github settings. ";
					if (res2 > 0)
					{
						msg += "Curl error code: " + String(res2);
					}
					else 
					{
						msg += "Invoking curl error code: " + String(res2);
					}

					AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Information", msg, "Ok");
					continue;
				}
			}
			
			return new CKanbanColumnGithub(aColumnId, aTitle, aOwner, url, token, owner, repo, query);
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
	var query = aJsonItem->getProperty("githubQuery");

	if (url.isString() && token.isString() && owner.isString() && repo.isString() && query.isString())
	{
		CKanbanColumnGithub *col = new CKanbanColumnGithub(aColumnId, aTitle, aOwner, URL::removeEscapeChars(url), URL::removeEscapeChars(token), URL::removeEscapeChars(owner), URL::removeEscapeChars(repo), URL::removeEscapeChars(query));
		return col;
	}

	return nullptr;
}



bool CKanbanColumnGithub::decodeGitlabRsp(const String & aData)
{
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
				auto id = dd->getProperty("id");
				auto pid = dd->getProperty("project_id");
				auto title = dd->getProperty("title");
				auto duedate = dd->getProperty("due_date");
				auto state = dd->getProperty("state");
				auto cr = dd->getProperty("created_at");
				auto up = dd->getProperty("updated_at");
				auto labels = dd->getProperty("labels"); //tab
				auto assobj = dd->getProperty("assignee"); //obj
				auto url = dd->getProperty("web_url");

				auto da = assobj.getDynamicObject();
				auto assingee = da->getProperty("username"); // name?

				auto dl = labels.getArray();
				String tags;
				for (auto& j : *dl) tags += j.toString() + " ";


				CKanbanCardComponent::CKanbanCardData c;

				c.values.set("text", title);
				//c.values.set("notes", dd->getProperty("description"));
				c.values.set("colour", -1);
				//c.values.set("colour", 0);
				c.values.set("url", url);
				c.values.set("tags", tags);
				c.values.set("assignee", assingee);
				if (duedate.isString())
				{
					c.values.set("dueDateSet", true);
					c.values.set("dueDate", Time::fromISO8601(duedate.toString()).toMilliseconds());

					Time t0 = Time::getCurrentTime();
					Time t1 = Time::fromISO8601(duedate.toString());
					RelativeTime rt = RelativeTime::milliseconds(t1.toMilliseconds() - t0.toMilliseconds());
					Logger::outputDebugString("rt: " + String(rt.inDays()) + "   " + String(t1.toMilliseconds() - t0.toMilliseconds()));
					if ((t1.getYear() == t0.getYear() && t1.getMonth() == t0.getMonth() && t1.getDayOfMonth() == t0.getDayOfMonth()) || (t1 < t0))
					{ // tooday or previos days
						c.values.set("colour", CConfiguration::getColourPalette().getColor(0).toString());
					}
					else if (rt.inDays() > 0 && rt.inDays() < 5 )
					{
						c.values.set("colour", CConfiguration::getColourPalette().getColor(((int)rt.inDays())+1).toString());
					}
				}
				else
				{
					c.values.set("dueDateSet", false);
				}
				c.values.set("creationDate", Time::fromISO8601(cr.toString()).toMilliseconds());
				c.values.set("lastUpdateDate", Time::fromISO8601(up.toString()).toMilliseconds());

				c.customProps.set("pid", pid.toString());
				c.customProps.set("id", id.toString());

				iTempCardList.add(c);

				/*				CKanbanCardComponent* c = new CKanbanCardComponent(nullptr);
				iTempCardList.add(c);
				c->setText(title);
				c->getProperties().set("pid", pid.toString());
				c->getProperties().set("id", id.toString());
				c->setUrl(url);
				c->setTags(tags);
				c->setAssigne(assingee);
				// c.setNotes(dd->getProperty("description")); // too large?
				c->setDates(Time::fromISO8601(cr.toString()), Time::fromISO8601(up.toString()));
				*/
				//decodeGitlabNotifier(c);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void CKanbanColumnGithub::decodeGitlabStarting_v2()
{
	iRefreshOngoing = true;
	iTempCardList.clear();

	String err;

	String queries;

	/*for (auto& sd : iGitlabDuedates)
	{
		for (auto& s : iGitlabUsers)
		{
			String out;
			int ec = 0;

			String q(iGitlabQuery);
			q = q.replace("{URL}", iGithubUrl);
			q = q.replace("{PROJECT_ID}", iGithubRepoOwner);
			q = q.replace("{DUE_DATE}", sd);
			q = q.replace("{USER}", s);

			queries += "\"" + q + "\" ";
		}
	}

	Logger::outputDebugString("built query: " + queries);

	String out;
	int ec = 0;
	if (invokeConnection_v2(queries, iGithubToken, out, ec))
	{
		StringArray rsps = StringArray::fromLines(out);
		rsps.removeEmptyStrings();
		for (auto& s : rsps)
		{
			if (!decodeGitlabRsp(s))
			{
				ec = -3;
			}
		}
	}
	if ( ec != 0 )
	{
		if (err.isEmpty()) err = "Error occured during refresh action, error code:";
		err += " " + String(ec) + " ";
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
	decodeGitlabFinished();

	iProgressBar.setVisible(false);
	iRefreshOngoing = false;

	if (!err.isEmpty())
	{
		CConfiguration::showStatusbarMessage("Refresh error: " + err);
	}
	else
	{
		CConfiguration::showStatusbarMessage("Refresh success");
	}*/
}

void CKanbanColumnGithub::decodeGitlabStarting()
{
	iRefreshOngoing = true;
	iTempCardList.clear();

	String err;

	/*for (auto& sd : iGitlabDuedates)
	{
		for (auto& s : iGitlabUsers)
		{
			String out;
			int ec = 0;

			if (invokeConnection(iGithubUrl, iGithubToken, iGithubRepoOwner, s, sd, iGitlabQuery, out, ec))
			{
				decodeGitlabRsp(out);
			}
			else
			{
				if (err.isEmpty()) err = "Error occured during refresh action, error codes:";
				err += " " + String(ec) + " ";
			}
		}
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
	decodeGitlabFinished();

	iProgressBar.setVisible(false);
	iRefreshOngoing = false;

	if (!err.isEmpty())
	{
		CConfiguration::showStatusbarMessage("Refresh error: " + err);
	}
	else
	{
		CConfiguration::showStatusbarMessage("Refresh success");
	}*/
	//AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", err);
}

void CKanbanColumnGithub::decodeGitlabNotifier(CKanbanCardComponent* aCard)
{
}

void CKanbanColumnGithub::decodeGitlabFinished()
{
	auto ar = iOwner.getCardsForColumn(this);

	for (auto& c : iTempCardList)
	{
		bool foundCard = false;
		for (int i = ar.size() - 1; i >= 0; --i)
		{
			if (ar[i]->getProperties()["pid"] == c.customProps["pid"] &&
				ar[i]->getProperties()["id"] == c.customProps["id"])
			{
				foundCard = true;
				ar[i]->setDates(Time(c.values["creationDate"]), Time(c.values["lastUpdateDate"]));
				ar[i]->setDueDate(c.values["dueDateSet"], Time(c.values["dueDate"]));
				ar[i]->setAssigne(c.values["assignee"]);
				if (c.values.contains("colour")) ar[i]->setColour(Colour::fromString(c.values["colour"].toString()));
				//if (c.values.contains("colour")) ar[i]->setColour(CConfiguration::getColourPalette().getColor(c.values["colour"]));
				break;
			}
		}
		if (!foundCard)
		{
			CKanbanCardComponent* card = new CKanbanCardComponent(nullptr);
			card->setupFromJson(c.values, c.customProps);
			iViewportLayout.createNewCard(card, true, false);
			delete card;
			ar = iOwner.getCardsForColumn(this);
		}
	}

	for (int i = ar.size() - 1; i >= 0; --i)
	{
		bool foundCard = false;
		for (auto& c : iTempCardList)
		{
			if (ar[i]->getProperties()["pid"] == c.customProps["pid"] &&
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
	String query = iGithubQuery;
	bool conn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, owner, repo, query, conn, true)) >= 0)
	{
		if (res == -1) return; // user clicked cancel
		if (res == 0)
		{
			if (conn)
			{
				int res2 = createAndShowWizardWindowTestConnection(url, token, owner, repo, query);
				if (res2 == 0)
				{ // all ok -> create window
				}
				else if (res2 == -1)
				{ // user pressed cancel -> show wizard again
					continue;
				}
				else
				{ // else res2 > 0 -> show error message and then wizard again
					AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Information", "Test connection failed. You need to verify your Gitlab settings. Curl error code: " + String(res2), "Ok");
					continue;
				}
			}

			iGithubUrl = url;
			iGithubToken = token;
			iGithubRepoOwner = owner;
			iGithubRepo = repo;
			iGithubQuery = query;
			return;
		}
	}
}

