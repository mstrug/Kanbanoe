#include "MainComponent.h"
#include "CConfiguration.h"


//==============================================================================
MainComponent::MainComponent()
{
	auto& c = CConfiguration::getInstance(); // create object
	c.getPropertiesFile()->setValue("Test", 1);

	iMenuBar.reset(new MenuBarComponent(this));
	addAndMakeVisible(iMenuBar.get());

	iKanbanBoard = new CKanbanBoard();
	addAndMakeVisible(iKanbanBoard);

	setSize(600, 400);
}

MainComponent::~MainComponent()
{
	delete iKanbanBoard;

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

	iKanbanBoard->setBounds(b);


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
		menu.addItem(0x0001, "Exit", true );
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
		{
			JUCEApplicationBase::quit();
		}
	}
	else if (topLevelMenuIndex == 1)
	{
		if (menuItemID == 0x0101)
		{
			static int t = 0;
			auto c = new CKanbanCardComponent();
			c->setTopLeftPosition(10, 20);
			c->name = "Card " + String(t++);
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


