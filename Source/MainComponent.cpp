#include "MainComponent.h"
#include "CConfiguration.h"
#include "CKanbanBoardArchive.h"

const String AppVersion("v0.60 BETA");



//==============================================================================
MainComponent::MainComponent() : iMdiPanel(*this), iTimer24h(*this)
{
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
		if (iTextSearch.getText().isEmpty())
		{
			this->unfocusAllComponents();
		}
		else
		{
			iTextSearch.setText("");
			auto mdi = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
			if (mdi)
			{
				mdi->setSearchText("");
				auto kb = mdi->getKanbanBoard();
				if (kb) kb->searchClear();
			}
		}
	};
	updateFindCallbacks();

	addAndMakeVisible(iMdiPanel);
	iMdiPanel.setLayoutMode(MultiDocumentPanel::MaximisedWindowsWithTabs);
	//iMdiPanel.setBackgroundColour(Colours::transparentBlack);
	iMdiPanel.setBackgroundColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	/*auto kb = new CKanbanBoardComponent();
	kb->createDefaultBoard();
	kb->setName("board 1");
	//addAndMakeVisible(iKanbanBoard);
	iKanbanBoards.add(kb);*/
	

	//juce::Time t = juce::Time::getCurrentTime();
	//int woy = CConfiguration::WeekOfYear();
	//iStatuBarR.setText( String(t.getYear()) + " "+ String::formatted("wk%02d",woy), NotificationType::dontSendNotification );
	updateTimer24h(); // set status bar text
	iStatuBarR.setJustificationType(Justification::centredRight);
	addAndMakeVisible(iStatuBarR);

	iStatuBarL.setJustificationType(Justification::centredLeft);
	//iStatuBarL.setText("started", NotificationType::dontSendNotification);
//	iStatuBarL.setco
	addAndMakeVisible(iStatuBarL);

	setOpaque(true);

	setSize(1100, 600);

	//iMdiPanel.addDocument(iKanbanBoards[0]);
	//iMdiPanel.addDocument(iKanbanBoards[0], getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), false);
	//	Colours::lightblue.withAlpha(0.6f), false);

	//Timer::callAfterDelay(300, [this] { grabKeyboardFocus(); }); // ensure that key presses are sent to the KeyPressTarget object

	iTimer24h.Start();
}

MainComponent::~MainComponent()
{
	CKanbanCardComponent::cleanupClipboardCard();

	iMdiPanel.closeAllDocuments(false);
	CConfiguration::getInstance().Destroy();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
	int h = LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight();
	
/*	g.setGradientFill(ColourGradient(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), Point<float>(0, h),
		getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.9), Point<float>(getWidth(), getHeight()), true));
	g.fillAll();
	*/
	// fill menubar
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 0, getWidth(), h);


	// fill status bar background
	auto r = getBounds();
	auto r2 = r.removeFromBottom(h);
	auto colour = findColour(TextEditor::backgroundColourId);
	g.setColour(colour);
	g.fillRect(r2);

	// draw status bar top & bottom lines
	g.setColour(Colours::darkgrey);
	g.fillRect(r2.removeFromBottom(1));
	g.fillRect(r2.removeFromTop(1)); // .translated(0, -1));
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
	auto b3 = b2.removeFromLeft(b.getWidth() / 2);
	iStatuBarR.setBounds(b2);
	iStatuBarL.setBounds(b3);

	iMdiPanel.setBounds(b);
	//iKanbanBoard->setBounds(b);


/*	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	if (iKanbanCards.size() > 0) iKanbanCards[0]->setBounds(20, 20, 100, 60);
	iKanbanColumns[0]->setBounds(200, 40, w + m, 400);
	iKanbanColumns[1]->setBounds(500, 40, w + m, 400);
	iA->setBounds(30, 200, 110, 70);*/
}

bool MainComponent::keyPressed(const KeyPress & key)
{
	if (key.getKeyCode() == key.pageDownKey || key.getKeyCode() == key.pageUpKey)
	{ // handle pgup/pgdown from mouse on any column
		//Logger::outputDebugString("key: " + String(key.getKeyCode()));
		auto cmp = getComponentAt(getMouseXYRelative());
		while (cmp)
		{
			auto cmp1 = dynamic_cast<CKanbanColumnComponent*>(cmp);
			if (cmp1)
			{
				return cmp1->keyPressed(key);
			}
			else
			{
				cmp = cmp->getParentComponent();
			}
		}
	}
	return false;
}

void MainComponent::updateTimer24h()
{
	iStatuBarR.setText(CConfiguration::YearAndWeekOfYear(), NotificationType::dontSendNotification);
	repaint(); // force redraw all due date kards
}

ApplicationCommandManager & MainComponent::getApplicationCommandManager()
{
	return iCommandManager;
}

StringArray MainComponent::getMenuBarNames()
{
	return { "File", "View", "Config", "Help" };
}

PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String&)
{
	PopupMenu menu;

	if (topLevelMenuIndex == 0) // file
	{
		PopupMenu pm;
		CConfiguration::getInstance().updateRecentlyOpenedMenu(pm);

		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileNew);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileOpen);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileClose);
		menu.addSeparator();
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSave);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveAs);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveAll);
		menu.addSeparator();
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveGroup);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileSaveGroupAs);
		menu.addSeparator();
		menu.addSubMenu("Open recent", pm);
		menu.addSeparator();
		menu.addCommandItem(&iCommandManager, CommandIDs::menuFileExit);
	}
	else if (topLevelMenuIndex == 1) // view
	{
		menu.addCommandItem(&iCommandManager, CommandIDs::menuViewArchive);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuViewColumnsEdit);
	}
	else if (topLevelMenuIndex == 2) // config
	{
		menu.addCommandItem(&iCommandManager, CommandIDs::menuConfigSearchCaseInsensitive);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuConfigSearchDynamic);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuConfigAutosave);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuConfigCardViewComplex);
	}
	else if (topLevelMenuIndex == 3) // help
	{
		menu.addCommandItem(&iCommandManager, CommandIDs::menuHelpAbout);
		menu.addCommandItem(&iCommandManager, CommandIDs::menuHelpCheckUpdate);
	}

	return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	if (menuItemID >= KRecentlyOpenedMenuItemIdBase && menuItemID < KRecentlyOpenedMenuItemIdBase + KRecentlyOpenedMenuItemIdCount )
	{
		File f(CConfiguration::getInstance().getRecentlyOpened(menuItemID - KRecentlyOpenedMenuItemIdBase));
		if (f.exists())
		{
			if (iMdiPanel.isAlreadyOpened(f))
			{
				iMdiPanel.activateDocumentByFileName(f);
			}
			else if (openFile(f))
			{
				CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName());
			}
		}
	}
	else if (menuItemID >= KRecentlyOpenedGroupMenuItemIdBase && menuItemID < KRecentlyOpenedGroupMenuItemIdBase + KRecentlyOpenedGroupMenuItemIdCount)
	{
		File f(CConfiguration::getInstance().getRecentlyOpened(menuItemID - KRecentlyOpenedGroupMenuItemIdBase, true));
		if (f.exists())
		{
			if (openGroupFile(f))
			{
				CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName(), true);
			}
		}
	}
}

ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return nullptr;
}

void MainComponent::getAllCommands(Array<CommandID>& aCommands)
{
	Array<CommandID> commands{ CommandIDs::menuFile, CommandIDs::menuFileNew, CommandIDs::menuFileOpen, CommandIDs::menuFileClose,
		CommandIDs::menuFileSave, CommandIDs::menuFileSaveAs, CommandIDs::menuFileSaveAll, CommandIDs::menuFileSaveGroup, CommandIDs::menuFileSaveGroupAs, CommandIDs::menuFileOpenRecent, CommandIDs::menuFileExit,
		CommandIDs::menuViewArchive, CommandIDs::menuViewColumnsEdit, CommandIDs::menuConfigSearchDynamic, CommandIDs::menuConfigSearchCaseInsensitive, CommandIDs::menuConfigAutosave, CommandIDs::menuConfigCardViewComplex, CommandIDs::menuHelpAbout, CommandIDs::menuHelpCheckUpdate, CommandIDs::menubarSearch,
		CommandIDs::menubarSearchClear, CommandIDs::mdiNextDoc, CommandIDs::mdiPrevDoc, CommandIDs::statusbarMessage };
	aCommands.addArray(commands);
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	bool isDocOpened = iMdiPanel.getActiveDocument() != nullptr;

	switch (commandID)
	{
	case mdiNextDoc:
			result.setInfo("Next Tab", "", "Mdi", 0);
			result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::ctrlModifier);
		break;
	case mdiPrevDoc:
			result.setInfo("Previous Tab", "", "Mdi", 0);
			result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::ctrlModifier | ModifierKeys::shiftModifier);
		break;
	case menubarSearch:
			result.setInfo("Search", "", "Menubar", 0);
			result.addDefaultKeypress('F', ModifierKeys::ctrlModifier);
		break;
	case menubarSearchClear:
			result.setInfo("Search Clear", "", "Menubar", 0);
			result.addDefaultKeypress(KeyPress::escapeKey, ModifierKeys::noModifiers);
		break;
	case menuFile:
			result.setInfo("File", "", "Menu Bar", 0);
			result.addDefaultKeypress('F', ModifierKeys::altModifier);
		break;
	case menuFileNew:
			result.setInfo("New", "", "Menu", 0);
			result.addDefaultKeypress('N', ModifierKeys::ctrlModifier);
		break;
	case menuFileOpen:
			result.setInfo("Open", "", "Menu", 0);
			result.addDefaultKeypress('O', ModifierKeys::ctrlModifier);
		break;
	case menuFileClose:
			result.setInfo("Close", "", "Menu", (!isDocOpened ? ApplicationCommandInfo::isDisabled : 0));
			result.addDefaultKeypress('W', ModifierKeys::ctrlModifier);
		break;
	case menuFileSave:
		{
			bool enabled = false;
			if (isDocOpened)
			{
				CMyMdiDoc* d = dynamic_cast<CMyMdiDoc*>( iMdiPanel.getActiveDocument() );
				enabled = (d ? d->getKanbanBoard()->isFileSet() : false);
			}
			result.setInfo("Save", "", "Menu", (!enabled ? ApplicationCommandInfo::isDisabled : 0));
			result.addDefaultKeypress('S', ModifierKeys::ctrlModifier);
		}
		break;
	case menuFileSaveAs:
		{
			bool enabled = false;
			if (isDocOpened)
			{
				CMyMdiDoc* d = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
				enabled = (d != nullptr);
			}
			result.setInfo("Save As", "", "Menu", (!enabled ? ApplicationCommandInfo::isDisabled : 0));
		}
		break;
	case menuFileSaveAll:
			result.setInfo("Save All", "", "Menu", (!isDocOpened ? ApplicationCommandInfo::isDisabled : 0));
			result.addDefaultKeypress('S', ModifierKeys::ctrlModifier | ModifierKeys::shiftModifier);
		break;
	case menuFileSaveGroup:
			result.setInfo("Save Group", "", "Menu", (!isDocOpened || !iMdiPanel.isFileSet() ? ApplicationCommandInfo::isDisabled : 0 ) );
		break;
	case menuFileSaveGroupAs:
			result.setInfo("Save Group As", "", "Menu", (!isDocOpened ? ApplicationCommandInfo::isDisabled : 0));
		break;
	case menuFileOpenRecent:
			result.setInfo("Open recent", "", "Menu", 0);
		break;
	case menuFileExit:
			result.setInfo("Exit", "Exit from application", "Menu", 0);
		break;
	case menuViewArchive:
			result.setInfo("View archives", "", "Menu", (!isDocOpened ? ApplicationCommandInfo::isDisabled : 0));
			result.addDefaultKeypress('A', ModifierKeys::altModifier);
		break;
	case menuViewColumnsEdit:
		{
			if (isDocOpened)
			{
				CMyMdiDoc* d = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
				if (d && d->getKanbanBoard()->isColumnsEditorEnabled())
				{
					result.setInfo("Add column mode", "", "Menu", ApplicationCommandInfo::isTicked);
				}
				else
				{
					result.setInfo("Add column mode", "", "Menu", 0);
				}
			}
			else
			{
				result.setInfo("Add column mode", "", "Menu", ApplicationCommandInfo::isDisabled );
			}
			result.addDefaultKeypress('C', ModifierKeys::altModifier);
		}
		break;
	case menuConfigSearchCaseInsensitive:
			result.setInfo("Search tags/date/assigne case insensitive", "Configure search case insensitive", "Menu", (CConfiguration::getBoolValue(KConfigSearchCase) ? ApplicationCommandInfo::isTicked : 0) );
		break;
	case menuConfigSearchDynamic:
			result.setInfo("Search doesn't require return key", "Configure dynmic search", "Menu", (CConfiguration::getBoolValue(KConfigSearchDynamic) ? ApplicationCommandInfo::isTicked : 0));
		break;
	case menuConfigAutosave:
		result.setInfo("Automatically save modified files", "Autosave", "Menu", (CConfiguration::getBoolValue(KConfigAutosave) ? ApplicationCommandInfo::isTicked : 0));
		break;
	case menuConfigCardViewComplex:
		result.setInfo("Card view with due date, assigne, url", "Complex card view", "Menu", (CConfiguration::getBoolValue(KConfigCardViewComplex) ? ApplicationCommandInfo::isTicked : 0));
		break;
	case menuHelpAbout:
			result.setInfo("About", "", "Menu", 0);
		break;
	case menuHelpCheckUpdate:
		result.setInfo("Check for updates...", "", "Menu", 0);
		break;
	case statusbarMessage:
			result.setInfo("Show message", "", "Statusbar", 0);
		break;
	default:
		break;
	}
}

bool MainComponent::perform(const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case menuFile:
		{
			iMenuBar->showMenu(0);
		}
		break;
	case menuFileNew:
		{
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
			iFileDialog.reset(new FileChooser("Choose a file to open...", File::getCurrentWorkingDirectory(), "*.pkb;*.kbf;*.kbg", false));
			if (iFileDialog->showDialog(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				if (f.getFileExtension().compareIgnoreCase(".kbg") == 0)
				{ // open group
					if (openGroupFile(f))
					{
						CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName(), true);
					}
				}
				else 
				{
					if (iMdiPanel.isAlreadyOpened(f))
					{
						iMdiPanel.activateDocumentByFileName(f);
					}
					else if (openFile(f))
					{
						CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName());
					}
				}
			}
		}
		break;
	case menuFileClose:
		{
			if (!iMdiPanel.getActiveDocument()) break;

			auto mdi = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
			if (mdi)
			{
				auto kb = mdi->getKanbanBoard();
				if (kb)
				{
					iMdiPanel.closeDocument(iMdiPanel.getActiveDocument(), true);
					iKanbanBoards.removeObject(kb, true);
				}
			}
			else
			{
				auto mdib = dynamic_cast<CMyMdiDocBase*>(iMdiPanel.getActiveDocument());
				if (mdib)
				{
					iMdiPanel.closeDocument(iMdiPanel.getActiveDocument(), true);
				}
			}
		}
		break;
	case menuFileSave:
		{
			if (!iMdiPanel.getActiveDocument()) break;
			auto mdi = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
			if (mdi)
			{
				if (mdi->save())
				{
					CConfiguration::getInstance().addRecentlyOpened(mdi->getFilePath()); // todo

					showStatusBarMessage("Saved");
				}
			}
		}
		break;
	case menuFileSaveAs:
		{
			if (!iMdiPanel.getActiveDocument()) break;
			iFileDialog.reset(new FileChooser("Choose a file to save...", File::getCurrentWorkingDirectory(), "*.kbf", false));
			if (iFileDialog->showDialog(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				if (f.getFileExtension().isEmpty())
				{
					f = f.withFileExtension("kbf");
				}

				auto md = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
				if (md)
				{
					auto kb = md->getKanbanBoard();
					if (kb)
					{
						kb->setFile(f);
						md->setName(f.getFileName());
						if (md->save())
						{
							CConfiguration::getInstance().addRecentlyOpened(md->getFilePath()); // todo
							
							showStatusBarMessage("Saved");
						}
					}
				}
			}
		}
		break;
	case menuFileSaveAll:
		{	
			for (int i = 0; i < iMdiPanel.getNumDocuments(); i++ )
			{
				auto* doc = iMdiPanel.getDocument(i);

				auto mdi = dynamic_cast<CMyMdiDoc*>(doc);
				if ( mdi ) mdi->save();
			}

			showStatusBarMessage("All files saved");
		}
		break;
	case menuFileSaveGroup:
		{
			if (!iMdiPanel.getActiveDocument()) break;
			auto f = iMdiPanel.getFile();
			if (f.exists())
			{
				if (saveGroupFile(f))
				{
					CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName(), true);

					showStatusBarMessage("Group saved");
				}
			}
		}
		break;
	case menuFileSaveGroupAs:
		{
			if (!iMdiPanel.getActiveDocument()) break;
			iFileDialog.reset(new FileChooser("Choose a file to save group...", File(), "*.kbg", false));
			if (iFileDialog->showDialog(FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting | FileBrowserComponent::canSelectFiles, nullptr))
			{
				auto f = iFileDialog->getResult();
				if (f.getFileExtension().isEmpty())
				{
					f = f.withFileExtension("kbg");
				}
				if (saveGroupFile(f))
				{
					CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName(), true);

					showStatusBarMessage("Group saved");
				}
			}
		}	
		break;
	case menuFileExit:
			requestedApplicationExit();
			JUCEApplicationBase::quit();
		break;
	case menuViewArchive:
		{
			auto doc = iMdiPanel.getActiveDocument();
			if (!doc) break;
			CMyMdiDoc* mdoc = dynamic_cast<CMyMdiDoc*>(doc);
			if (!mdoc) break;
			auto kb = mdoc->getKanbanBoard();
			String tabName = mdoc->getName() + "\\archives";
			if (iMdiPanel.isAlreadyOpened(tabName))
			{
				iMdiPanel.activateDocumentByTabName(tabName);
				break;
			}

			CKanbanBoardArchive * arch = new CKanbanBoardArchive(*kb);
			CMdiDocArchives *cmp = new CMdiDocArchives(arch);
			cmp->setName(tabName);
			iMdiPanel.addDocument(cmp, mdoc);


			//auto kb = static_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument())->getKanbanBoard();
			//kb->logArchives();
		}
		break;
	case menuViewColumnsEdit:
		{
			auto doc = iMdiPanel.getActiveDocument();
			if (!doc) break;
			CMyMdiDoc* mdoc = dynamic_cast<CMyMdiDoc*>(doc);
			if (!mdoc) break;
			auto kb = mdoc->getKanbanBoard();
			kb->setColumnsEditor(!kb->isColumnsEditorEnabled());
		}
		break;
	case menuConfigSearchCaseInsensitive:
		{
			bool b = CConfiguration::getBoolValue(KConfigSearchCase);
			auto pf = CConfiguration::getInstance().getPropertiesFile();
			pf->setValue(KConfigSearchCase, !b);
			textFindCallbackFcn();
		}
		break;
	case menuConfigSearchDynamic:
		{
			bool b = CConfiguration::getBoolValue(KConfigSearchDynamic);
			auto pf = CConfiguration::getInstance().getPropertiesFile();
			pf->setValue(KConfigSearchDynamic, !b);
			updateFindCallbacks();
		}
		break;
	case menuConfigAutosave:
		{
			bool b = CConfiguration::getBoolValue(KConfigAutosave);
			auto pf = CConfiguration::getInstance().getPropertiesFile();
			pf->setValue(KConfigAutosave, !b);
		}
		break;
	case menuConfigCardViewComplex:
		{
			bool b = CConfiguration::getBoolValue(KConfigCardViewComplex);
			auto pf = CConfiguration::getInstance().getPropertiesFile();
			pf->setValue(KConfigCardViewComplex, !b);
			iMdiPanel.updateDocuments();
		}
		break;
	case menuHelpAbout:
			AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "About", "Kanbanoe " + AppVersion + "\nMichal Strug\n\nhttp://kanbanoe.app/", "OK");
		break;
	case menuHelpCheckUpdate:
			checkForUpdates();
		break;
	case menubarSearch:
			iTextSearch.grabKeyboardFocus();
		break;
	case menubarSearchClear:
			iTextSearch.onEscapeKey();
		break;
	case mdiNextDoc:
			iMdiPanel.activateNextPrevDocument(true);
		break;
	case mdiPrevDoc:
			iMdiPanel.activateNextPrevDocument(false);
		break;
	case statusbarMessage:
		{
			auto s = CConfiguration::getStatusbarMessage();
			showStatusBarMessage(s);
		}
		break;
	default:
		return false;
	}

	return true;
}


void MainComponent::setSearchText(const String & aString, bool aUpdateSearchField)
{
	auto mdi = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
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

void MainComponent::showStatusBarMessage(StringRef aString)
{
	iStatuBarL.setText(aString, NotificationType::dontSendNotification);
	Timer::callAfterDelay(aString.length() > 15 ? 10000 : 2000, [this] { iStatuBarL.setText("", NotificationType::dontSendNotification); });
}

void MainComponent::requestedApplicationExit()
{
	iMdiPanel.closeAllDocumentsAndVerifyStore();
}

bool MainComponent::openFile(File& aFn)
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
			return true;
		}
	}
	return false;
}

bool MainComponent::openGroupFile(File & aFn)
{
	Array<String> files;
	if (iMdiPanel.openFile(aFn, files))
	{
		for (auto fstr : files)
		{
			File f(fstr);
			if (f.exists() )
			{
				if (!iMdiPanel.isAlreadyOpened(f))
				{
					if (openFile(f))
					{
						CConfiguration::getInstance().addRecentlyOpened(f.getFullPathName());
					}
				}
			}
		}
	}
	return true;
}

bool MainComponent::saveGroupFile(File & aFn)
{
	return iMdiPanel.saveFile(aFn);
}

void MainComponent::textFindCallbackFcn()
{
	auto mdi = dynamic_cast<CMyMdiDoc*>(iMdiPanel.getActiveDocument());
	if (mdi)
	{
		mdi->setSearchText(iTextSearch.getText());
		setSearchText(iTextSearch.getText(), false);
	}
}

void MainComponent::updateFindCallbacks()
{
	if (CConfiguration::getBoolValue(KConfigSearchDynamic))
	{
		iTextSearch.onTextChange = [this] { textFindCallbackFcn(); };
		iTextSearch.onReturnKey = nullptr;
	}
	else
	{
		iTextSearch.onReturnKey = [this] { textFindCallbackFcn(); };
		iTextSearch.onTextChange = nullptr;
	}
}

void MainComponent::checkForUpdates()
{
	ChildProcess cp;
	String curls = CConfiguration::getValue("curl");
	if (curls.isEmpty())
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Check for update", "Cannot find curl.exe application.", "OK");
		return;
	}
	String cmd = curls + " http://kanbanoe.app/data/v.txt";
	Logger::outputDebugString("curl cmd: " + cmd);
	if (cp.start(cmd, ChildProcess::wantStdOut))
	{
		String out = cp.readAllProcessOutput();
		uint32 ec = cp.getExitCode();
		if (ec == 0)
		{
			int idx = out.indexOf("\r");
			if (idx >= 0)
			{
				auto version = out.substring(0, idx);
				auto url = out.substring(idx + 2); // \r an \n
				int idx2 = url.indexOf("\r");
				if (idx2 >= 0)
				{
					url = url.substring(0, idx2);
					url = url.removeCharacters("\r");
					url = url.removeCharacters("\n");
				}

				if (AppVersion.contains(version))
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Check for update", "You have the latest version.", "OK");
				}
				else
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Check for update", "There is a newer verson: " + version +".\r\nIt can be download from:\r\n" + url, "OK");
				}
			}
		}
		else
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Check for update", "Error during downloading of data.", "OK");
			return;
		}
	}
	else
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Check for update", "Process start failed.", "OK");
		return;
	}
}

