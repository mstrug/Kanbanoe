/*
  ==============================================================================

    CKanbanColumnGitlab.cpp
    Created: 6 Oct 2021 9:44:02pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanColumnGitlab.h"
#include "CConfiguration.h"
#include "CKanbanBoard.h"


//==============================================================================
CKanbanColumnGitlab::CKanbanColumnGitlab(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner, StringRef aUrl, StringRef aToken, StringRef aProjectId, StringRef aUsers, StringRef aDueDates, StringRef aQuery) : CKanbanColumnComponent(aColumnId, aTitle, aOwner), iGitlabUrl(aUrl), iGitlabToken(aToken), iGitlabProjectId(aProjectId), iGitlabQuery(aQuery)
{
	iGitlabUsers.addTokens(aUsers, ", ", "");
	iGitlabUsers.removeEmptyStrings();

	iGitlabDuedates.addTokens(aDueDates, ", ", "");
	iGitlabDuedates.removeEmptyStrings();
}

CKanbanColumnGitlab::~CKanbanColumnGitlab()
{
}

bool CKanbanColumnGitlab::showRefreshMenuEntry()
{
	return true;
}

void CKanbanColumnGitlab::refreshThreadWorkerFunction()
{
	decodeGitlabStarting();
}


int CKanbanColumnGitlab::getColumnTypeId()
{
	return 1;
}

void CKanbanColumnGitlab::outputAdditionalDataToJson(String & aJson)
{
	aJson << ", \"gitlabUrl\":\"" + URL::addEscapeChars(iGitlabUrl, false) + "\"";
	aJson << ", \"gitlabToken\":\"" + URL::addEscapeChars(iGitlabToken, false) + "\"";
	aJson << ", \"gitlabProjectId\":\"" + URL::addEscapeChars(iGitlabProjectId, false) + "\"";
	aJson << ", \"gitlabDueDates\":\"" + URL::addEscapeChars(iGitlabDuedates.joinIntoString(","), false) + "\"";
	aJson << ", \"gitlabUsers\":\"" + URL::addEscapeChars(iGitlabUsers.joinIntoString(","), false) + "\"";
	aJson << ", \"gitlabQuery\":\"" + URL::addEscapeChars(iGitlabQuery, false) + "\"";
}


// return: 
//		-1 : canceled
//		0 : ok
//		1 : wrong data entered, retry by showing the window
static int createAndShowWizardWindow(String& aUrl, String& aToken, String& aProject, String& aUsers, String& aDueDates, String& aQuery, bool& aTestConn)
{
	AlertWindow aw("Create gitlab integration column", "Provide new column information", AlertWindow::QuestionIcon);
	aw.addTextEditor("text_url", aUrl, "Gitlab URL:");
	aw.addTextEditor("text_token", aToken, "Gitlab API token:");
	aw.addTextEditor("text_project", aProject, "Project ID:");
	aw.addTextEditor("text_users", aUsers, "API query user name list:");
	aw.addTextEditor("text_duedates", aDueDates, "API query due date list:");
	aw.addTextEditor("text_query", aQuery, "Gitlab API query (evaluated for each user):");
	
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
		auto text_project = aw.getTextEditorContents("text_project");
		auto text_users = aw.getTextEditorContents("text_users");
		auto text_duedates = aw.getTextEditorContents("text_duedates");
		auto text_query = aw.getTextEditorContents("text_query");

		String errorStr;  
		if (text_url.isEmpty()) errorStr = "Gitlab URL cannot be empty.";
		else if (text_token.isEmpty()) errorStr = "Gitlab API token cannot be empty.";
		else if (text_project.isEmpty()) errorStr = "Project ID cannot be empty.";
		else if (text_duedates.isEmpty()) errorStr = "Due date list cannot be empty.";
		else if (text_users.isEmpty()) errorStr = "Users list cannot be empty.";
		else if (text_query.isEmpty()) errorStr = "Gitlab API query cannot be empty.";
		else if (!text_query.contains("{URL}")) errorStr = "Gitlab API query must contain {URL} placeholder.";
		else if (!text_query.contains("{PROJECT_ID}")) errorStr = "Gitlab API query must contain {PROJECT_ID} placeholder.";
		else if (!text_query.contains("{DUE_DATE}")) errorStr = "Gitlab API query must contain {DUE_DATE} placeholder.";
		else if (!text_query.contains("{USER}")) errorStr = "Gitlab API query must contain {USER} placeholder.";

		aUrl = text_url;
		aToken = text_token;
		aProject = text_project;
		aUsers = text_users;
		aDueDates = text_duedates;
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

static bool invokeConnection(StringRef aUrl, StringRef aToken, StringRef aProject, StringRef aUser, StringRef aDueDate, StringRef aQuery, String& aOutput, int& aCurlErrorCode)
{
	String q(aQuery);
	q = q.replace("{URL}", aUrl);
	q = q.replace("{PROJECT_ID}", aProject);
	q = q.replace("{DUE_DATE}", aDueDate);
	q = q.replace("{USER}", aUser);

	ChildProcess cp;
	String curls = CConfiguration::getValue("curl");
	if (curls.isEmpty())
	{
		aOutput = "curl bad path";
		aCurlErrorCode = -1;
		return false;
	}
	Logger::outputDebugString("query: " + q);
	if (cp.start(curls + " -k -v --header \"PRIVATE-TOKEN: " + aToken + "\" \"" + q + "\"", ChildProcess::wantStdOut))
	{
		String out = cp.readAllProcessOutput();
		uint32 ec = cp.getExitCode();
		aCurlErrorCode = ec;
		if (ec == 0)
		{
			aOutput = out;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		aCurlErrorCode = -2;
		aOutput = "Process start failed";
		return false;
	}
}

static int createAndShowWizardWindowTestConnection(StringRef aUrl, StringRef aToken, StringRef aProject, StringRef aUser, StringRef aDueDate, StringRef aQuery)
{
	class DemoBackgroundThread : public ThreadWithProgressWindow
	{
	public:
		StringRef url;
		StringRef token;
		StringRef project;
		StringRef duedate;
		StringRef user;
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
			invokeResult = invokeConnection(url, token, project, user, duedate, query, out, curlErrorCode);
		}
	} *dw = new DemoBackgroundThread();
	dw->url = aUrl;
	dw->token = aToken;
	dw->project = aProject;
	dw->duedate = aDueDate;
	dw->user = aUser;
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
			ret = dw->curlErrorCode > 0 ? dw->curlErrorCode : 1;
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

CKanbanColumnGitlab * CKanbanColumnGitlab::createWithWizard(int aColumnId, const String & aTitle, CKanbanBoardComponent & aOwner)
{
	String url = "https://yourdomain.com";
	String token = "";
	String projId = "1234";
	String users = "user1, user2, user3";
	String duedates = "overdue, week";
	String query = "{URL}/api/v4/projects/{PROJECT_ID}/issues?due_date={DUE_DATE}&state=opened&order_by=due_date&sort=asc&scope=all&assignee_username={USER}";
	bool testConn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, projId, users, duedates, query, testConn)) >= 0)
	{
		if (res == -1) return nullptr; // user clicked cancel
		if (res == 0)
		{
			if (testConn)
			{
				String duedate1 = duedates.upToFirstOccurrenceOf(",", false, true);
				String user1 = users.upToFirstOccurrenceOf(",", false, true);
				int res2 = createAndShowWizardWindowTestConnection(url, token, projId, user1, duedate1, query);
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
			
			return new CKanbanColumnGitlab(aColumnId, aTitle, aOwner, url, token, projId, users, duedates, query);
		}
	}
	
	return nullptr;
}

CKanbanColumnGitlab * CKanbanColumnGitlab::createFromJson(int aColumnId, const String & aTitle, CKanbanBoardComponent & aOwner, DynamicObject * aJsonItem)
{
	var url = aJsonItem->getProperty("gitlabUrl");
	var token = aJsonItem->getProperty("gitlabToken");
	var projId = aJsonItem->getProperty("gitlabProjectId");
	var duedates = aJsonItem->getProperty("gitlabDueDates");
	var users = aJsonItem->getProperty("gitlabUsers");
	var query = aJsonItem->getProperty("gitlabQuery");

	if (url.isString() && token.isString() && projId.isString() && users.isString() && duedates.isString() && query.isString())
	{
		CKanbanColumnGitlab *col = new CKanbanColumnGitlab(aColumnId, aTitle, aOwner, URL::removeEscapeChars(url), URL::removeEscapeChars(token), URL::removeEscapeChars(projId), URL::removeEscapeChars(users), URL::removeEscapeChars(duedates), URL::removeEscapeChars(query));
		return col;
	}

	return nullptr;
}



void CKanbanColumnGitlab::decodeGitlabRsp(const String & aData)
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
					if ((t1.getYear() == t0.getYear() && t1.getMonth() == t0.getMonth() && t1.getDayOfMonth() == t0.getDayOfMonth()) || (t1 < t0))
					{ // tooday or previos days
						c.values.set("colour", CConfiguration::getColourPalette().getColor(0).toString());
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
	}
}

void CKanbanColumnGitlab::decodeGitlabStarting()
{
	iRefreshOngoing = true;
	iTempCardList.clear();

	String err;

	for (auto& sd : iGitlabDuedates)
	{
		for (auto& s : iGitlabUsers)
		{
			String out;
			int ec = 0;

			if (invokeConnection(iGitlabUrl, iGitlabToken, iGitlabProjectId, s, sd, iGitlabQuery, out, ec))
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
	}
	//AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", err);
}

void CKanbanColumnGitlab::decodeGitlabNotifier(CKanbanCardComponent* aCard)
{
}

void CKanbanColumnGitlab::decodeGitlabFinished()
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
				//				if (c.values.contains("colour")) ar[i]->setColour(CConfiguration::getColourPalette().getColor(c.values["colour"]));
				break;
			}
		}
		if (!foundCard)
		{
			CKanbanCardComponent* card = new CKanbanCardComponent(nullptr);
			card->setupFromJson(c.values, c.customProps);
			iViewportLayout.createNewCard(card, true, false);
			delete card;
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


void CKanbanColumnGitlab::refreshSetupFunction()
{
	String url = iGitlabUrl;
	String token = iGitlabToken;
	String projId = iGitlabProjectId;
	String users = iGitlabUsers.joinIntoString(",");
	String duedates = iGitlabDuedates.joinIntoString(",");
	String query = iGitlabQuery;
	bool conn = true;

	int res;
	while ((res = createAndShowWizardWindow(url, token, projId, users, duedates, query, conn)) >= 0)
	{
		if (res == -1) return; // user clicked cancel
		if (res == 0)
		{
			if (conn)
			{
				String duedate1 = duedates.upToFirstOccurrenceOf(",", false, true);
				String user1 = users.upToFirstOccurrenceOf(",", false, true);
				int res2 = createAndShowWizardWindowTestConnection(iGitlabUrl, iGitlabToken, iGitlabProjectId, user1, duedate1, iGitlabQuery);
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

			iGitlabUrl = url;
			iGitlabToken = token;
			iGitlabProjectId = projId;
			iGitlabQuery = query;
			iGitlabDuedates.clearQuick();
			iGitlabDuedates.addTokens(duedates, ", ", "");
			iGitlabDuedates.removeEmptyStrings();
			iGitlabUsers.clearQuick();
			iGitlabUsers.addTokens(users, ", ", "");
			iGitlabUsers.removeEmptyStrings();
			return;
		}
	}
}

