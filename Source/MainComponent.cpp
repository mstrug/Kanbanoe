#include "MainComponent.h"
#include "CConfiguration.h"


//==============================================================================
MainComponent::MainComponent() : iMdiPanel(*this)
{
	auto& c = CConfiguration::getInstance(); // create object
	//c.getPropertiesFile()->setValue("Test", 1);

	setApplicationCommandManagerToWatch(&iCommandManager);
	iCommandManager.registerAllCommandsForTarget(this);

	iMenuBar.reset(new MenuBarComponent(this));
	addAndMakeVisible(iMenuBar.get());

	// search
	iLabelSearch.setText("Search:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelSearch);

	addAndMakeVisible(iTextSearch);
	iTextSearch.setSelectAllWhenFocused(true);
	iTextSearch.onEscapeKey = [this]
	{
		iTextSearch.setText("");
		auto mdi = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
		if (mdi)
		{
			mdi->setSearchText("");
			auto kb = mdi->getKanbanBoard();
			if (kb) kb->searchClear();
		}
	};
	iTextSearch.onReturnKey = [this]
	{
		auto mdi = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
		if (mdi)
		{
			mdi->setSearchText(iTextSearch.getText());
			setSearchText(iTextSearch.getText(), false);
		}
	};

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
	//if (e < f) wk++;

	iStatuBar.setJustificationType(Justification::centredRight);
	iStatuBar.setText( String(t.getYear()) + " wk" + String(wk), NotificationType::dontSendNotification );
	addAndMakeVisible(iStatuBar);

	setSize(600, 400);

	iMdiPanel.addDocument(iKanbanBoards[0]);
	//iMdiPanel.addDocument(iKanbanBoards[0], getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
	//	Colours::lightblue.withAlpha(0.6f), false);

	//Timer::callAfterDelay(300, [this] { grabKeyboardFocus(); }); // ensure that key presses are sent to the KeyPressTarget object
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

	int txtsw = 250;
	int txtmar = 20;
	iLabelSearch.setBounds(b.getWidth() - 50 - 10 - txtsw - txtmar, 0, 50, 24);
	iTextSearch.setBounds(b.getWidth() - txtsw - txtmar, 2, txtsw, 20);

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

ApplicationCommandManager & MainComponent::getApplicationCommandManager()
{
	return iCommandManager;
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
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileNew);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileOpen);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileClose);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSave);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveAs);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveAll);
		menu.addSeparator();
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileExit);
	}
	else if (topLevelMenuIndex == 1) // edit
	{
		menu.addCommandItem(&iCommandManager, CommandIDs::menuEditAddCard);
	}
	else if (topLevelMenuIndex == 2) // help
	{
		menu.addCommandItem(&iCommandManager, CommandIDs::menuHelpAbout);
	}

	return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
}

ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return nullptr;
}

void MainComponent::getAllCommands(Array<CommandID>& aCommands)
{
	Array<CommandID> commands{ CommandIDs::menuFileNew, CommandIDs::menuFileOpen, CommandIDs::menuFileClose,
		CommandIDs::menuFileSave, CommandIDs::menuFileSaveAs, CommandIDs::menuFileSaveAll, CommandIDs::menuFileExit,
		CommandIDs::menuEditAddCard, CommandIDs::menuHelpAbout, CommandIDs::menubarSearch };
	aCommands.addArray(commands);
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case menuFileNew:
			result.setInfo("New", "", "Menu", 0);
			result.addDefaultKeypress('N', ModifierKeys::ctrlModifier);
		break;
	case menuFileOpen:
			result.setInfo("Open", "", "Menu", 0);
			result.addDefaultKeypress('O', ModifierKeys::ctrlModifier);
		break;
	case menuFileClose:
			result.setInfo("Close", "", "Menu", 0);
		break;
	case menuFileSave:
			result.setInfo("Save", "", "Menu", 0);
			result.addDefaultKeypress('S', ModifierKeys::ctrlModifier);
		break;
	case menuFileSaveAs:
			result.setInfo("Save As", "", "Menu", 0);
		break;
	case menuFileSaveAll:
			result.setInfo("Save All", "", "Menu", 0);
			result.addDefaultKeypress('S', ModifierKeys::ctrlModifier | ModifierKeys::shiftModifier);
		break;
	case menuFileExit:
			result.setInfo("Exit", "Exit from application", "Menu", 0);
		break;
	case menuEditAddCard:
			result.setInfo("Add card", "", "Menu", 0);
			//result.addDefaultKeypress('A', ModifierKeys::ctrlModifier);
		break;
	case menuHelpAbout:
			result.setInfo("About", "", "Menu", 0);
		break;
	case menubarSearch:
			result.setInfo("Search", "", "Menubar", 0);
			result.addDefaultKeypress('F', ModifierKeys::ctrlModifier );
		break;
	default:
		break;
	}
}

bool MainComponent::perform(const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case menuFileNew:
		{
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
		break;
	case menuFileOpen:
		{
			iFileDialog.reset(new FileChooser("Choose a file to open...", File::getCurrentWorkingDirectory(), "*.pkb", false));
			if (iFileDialog->showDialog(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				openFile(f);
			}
		}
		break;
	case menuFileClose:
		{
			auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
			iMdiPanel.closeDocument(iMdiPanel.getActiveDocument(), false);
			iKanbanBoards.removeObject(kb, true);
		}
		break;
	case menuFileSave:
		{
			auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
			saveFile(kb);
		}
		break;
	case menuFileSaveAs:
		{
			iFileDialog.reset(new FileChooser("Choose a file to save...", File::getCurrentWorkingDirectory(), "*.pkb", false));
			if (iFileDialog->showDialog(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				if (f.getFileExtension().isEmpty())
				{
					f = f.withFileExtension("pkb");
				}

				auto md = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
				auto kb = md->getKanbanBoard();
				if (kb)
				{
					kb->setFile(f);
					md->setName(f.getFileName());
					saveFile(kb);
				}
			}
		}
		break;
	case menuFileSaveAll:
		{
			for (auto& i : iKanbanBoards)
			{
				saveFile(i);
			}
		}
		break;
	case menuFileExit:
			JUCEApplicationBase::quit();
		break;
	case menuEditAddCard:
		{
			static int t = 0;
			auto c = new CKanbanCardComponent(nullptr);
			c->setTopLeftPosition(10, 20);
			c->setText("Card " + String(t++));
			iKanbanCards.add(c);
			addAndMakeVisible(c);
		}
		break;
	case menuHelpAbout:
			AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "About", "v0.20\nM.Strug", "OK");
		break;
	case menubarSearch:
			iTextSearch.grabKeyboardFocus();
		break;
	default:
		return false;
	}

	return true;
}


void MainComponent::setSearchText(const String & aString, bool aUpdateSearchField)
{
	auto mdi = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
	if (mdi)
	{
		auto kb = mdi->getKanbanBoard();
		if (kb)
		{
			kb->searchClear();
			kb->search(aString);
		}
	}
	if (aUpdateSearchField)
	{
		iTextSearch.setText(aString);
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

