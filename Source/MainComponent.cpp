#include "MainComponent.h"
#include "CConfiguration.h"


//==============================================================================
MainComponent::MainComponent()
{
	auto& c = CConfiguration::getInstance(); // create object
	c.getPropertiesFile()->setValue("Test", 1);

	iMenuBar.reset(new MenuBarComponent(this));
	addAndMakeVisible(iMenuBar.get());

	addAndMakeVisible(iMdiPanel);
	iMdiPanel.setLayoutMode(MultiDocumentPanel::MaximisedWindowsWithTabs);
	//iMdiPanel.setBackgroundColour(Colours::transparentBlack);
	iMdiPanel.setBackgroundColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	auto kb = new CKanbanBoardComponent();
	kb->createDefaultBoard();
	kb->setName("board 1");
	//addAndMakeVisible(iKanbanBoard);
	iKanbanBoards.add(kb);

	juce::Time t = juce::Time::getCurrentTime();
	int d = t.getDayOfYear();
	int e = t.getDayOfWeek();
	juce::Time t2 = t.fromISO8601(String(t.getYear()) + "0101");
	int f = t2.getDayOfWeek();
	int wk = ((d + 6) / 7);
	if (e < f) wk++;

	iStatuBar.setJustificationType(Justification::centredRight);
	iStatuBar.setText( String(t.getYear()) + " wk" + String(wk), NotificationType::dontSendNotification );
	addAndMakeVisible(iStatuBar);

	setSize(600, 400);

	iMdiPanel.addDocument(iKanbanBoards[0]);
	//iMdiPanel.addDocument(iKanbanBoards[0], getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
	//	Colours::lightblue.withAlpha(0.6f), false);
}

MainComponent::~MainComponent()
{
	iMdiPanel.closeAllDocuments(false);
	CConfiguration::getInstance().Destroy();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setFont (juce::Font (16.0f));
    //g.setColour (juce::Colours::white);
    //g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	auto b = getLocalBounds();
	auto b1 = b.removeFromTop(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());
	iMenuBar->setBounds( b1 );

	auto b2 = b.removeFromBottom(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());
	iStatuBar.setBounds(b2);

	iMdiPanel.setBounds(b);
	//iKanbanBoard->setBounds(b);


/*	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	if (iKanbanCards.size() > 0) iKanbanCards[0]->setBounds(20, 20, 100, 60);
	iKanbanColumns[0]->setBounds(200, 40, w + m, 400);
	iKanbanColumns[1]->setBounds(500, 40, w + m, 400);
	iA->setBounds(30, 200, 110, 70);*/
}

StringArray MainComponent::getMenuBarNames()
{
	return { "File", "Edit", "Help" };
}


PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String&)
{
	PopupMenu menu;

	if (topLevelMenuIndex == 0) // file
	{
		menu.addItem(0x0001, "New", true);
		menu.addItem(0x0010, "Open", true);
		menu.addItem(0x0020, "Close", true);
		menu.addItem(0x0030, "Save", true);
		menu.addItem(0x0040, "Save as", true);
		menu.addItem(0x0050, "Save all", true);
		menu.addItem(0x00f0, "Exit", true);
	}
	else if (topLevelMenuIndex == 1) // help
	{
		menu.addItem(0x0101, "Add Card", true);
	}
	else if (topLevelMenuIndex == 2) // help
	{
		menu.addItem(0x0201, "About", true);
	}

	return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	if (topLevelMenuIndex == 0)
	{
		if (menuItemID == 0x0001)
		{ // new
			//removeChildComponent(iKanbanBoard);
			//delete iKanbanBoard;
			auto kb = new CKanbanBoardComponent();
			kb->setName("board " + String(iKanbanBoards.size()));
			kb->createDefaultBoard();
			//addAndMakeVisible(iKanbanBoard);			
			iKanbanBoards.add(kb);
			//iMdiPanel.addDocument(iKanbanBoards.getLast(), getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
			iMdiPanel.addDocument(iKanbanBoards.getLast());
		}
		else if (menuItemID == 0x0010)
		{ // open
			iFileDialog.reset(new FileChooser("Choose a file to open...", File::getCurrentWorkingDirectory(), "*.pkb", false));
			if (iFileDialog->showDialog(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				openFile(f);
			}
		}
		else if (menuItemID == 0x0020)
		{ // close
			auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
			iMdiPanel.closeDocument(iMdiPanel.getActiveDocument(), false);
			iKanbanBoards.removeObject(kb, true);
		}
		else if (menuItemID == 0x0030)
		{  // save
			auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
			saveFile(kb);
		}
		else if (menuItemID == 0x0040)
		{
			// save as
			iFileDialog.reset(new FileChooser("Choose a file to save...", File::getCurrentWorkingDirectory(), "*.pkb", false));
			if (iFileDialog->showDialog(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				if (f.getFileExtension().isEmpty())
				{
					f = f.withFileExtension("pkb");
				}

				auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
				if (kb)
				{
					kb->setFile(f);
					saveFile(kb);
				}
			}
		}
		else if (menuItemID == 0x0050)
		{ // save all
			for (auto& i : iKanbanBoards)
			{
				saveFile(i);
			}
		}
		else if (menuItemID == 0x00f0)
		{
			JUCEApplicationBase::quit();
		}
	}
	else if (topLevelMenuIndex == 1)
	{
		if (menuItemID == 0x0101)
		{
			static int t = 0;
			auto c = new CKanbanCardComponent(nullptr);
			c->setTopLeftPosition(10, 20);
			c->setText("Card " + String(t++));
			iKanbanCards.add(c);
			addAndMakeVisible(c);
		}
	}
	else if (topLevelMenuIndex == 2)
	{
		if (menuItemID == 0x0201)
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "About","Information about application","OK");
		}
	}
}

void MainComponent::openFile(File& aFn)
{
	var d = JSON::parse(aFn);
	if (d == var())
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Error parsing file", "Close");
	}
	else
	{
		String errorMessage;
		
		CKanbanBoardComponent* newboard = CKanbanBoardComponent::fromJson(d, errorMessage);
		if (!newboard)
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", errorMessage, "Close");
		}
		else
		{
			//removeChildComponent(iKanbanBoard);
			//delete iKanbanBoard;
			//iKanbanBoard = newboard;
			//addAndMakeVisible(newboard);
			//resized();

			iKanbanBoards.add(newboard);
			newboard->setName(aFn.getFileName());
			newboard->setFile(aFn);
			//iMdiPanel.addDocument(newboard, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
			iMdiPanel.addDocument(newboard);

			//Config::getInstance()->setOpenRecent(aFn.getFullPathName());
		}
	}
}

bool MainComponent::saveFile(CKanbanBoardComponent* aBoard)
{
	String errorMessage;
	if (aBoard)
	{
		if (aBoard->getFile().getFullPathName().isEmpty())
		{
			Logger::outputDebugString("File not opened!");
			return false;
		}
		if (!aBoard->saveFile(errorMessage))
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", errorMessage, "Close");
		}
		return true;
	}
	return false;
}

