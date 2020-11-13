/*
  ==============================================================================

    CKanbanBoard.cpp
    Created: 12 Nov 2020 4:50:24pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanBoard.h"
#include "CConfiguration.h"

//==============================================================================
CKanbanBoard::CKanbanBoard()
{
	iKanbanColumns.add(new CKanbanColumnComponent());
	addAndMakeVisible(iKanbanColumns[0]);
	iKanbanColumns.add(new CKanbanColumnComponent());
	addAndMakeVisible(iKanbanColumns[1]);
	iKanbanColumns.add(new CKanbanColumnComponent());
	addAndMakeVisible(iKanbanColumns[2]);
	iKanbanColumns.add(new CKanbanColumnComponent());
	addAndMakeVisible(iKanbanColumns[3]);
	iKanbanColumns.add(new CKanbanColumnComponent());
	addAndMakeVisible(iKanbanColumns[4]);


}

CKanbanBoard::~CKanbanBoard()
{
}

void CKanbanBoard::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

}

void CKanbanBoard::resized()
{
	Grid grid;

	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	Grid::Px wpx(w + 2 * m);

	grid.rowGap = Grid::Px( m );
	grid.columnGap = Grid::Px(m);

	grid.templateColumns.add(Grid::TrackInfo(wpx));
	grid.templateColumns.add(Grid::TrackInfo(wpx));
	grid.templateColumns.add(Grid::TrackInfo(wpx));
	grid.templateColumns.add(Grid::TrackInfo(wpx));

	grid.templateRows.add(Grid::TrackInfo(50_fr));
	grid.templateRows.add(Grid::TrackInfo(50_fr));

	grid.autoFlow = Grid::AutoFlow::column;

	GridItem gi1(iKanbanColumns[0]);
	gi1.setArea(1, 1, 3, 1);
	grid.items.add(gi1);
	GridItem gi2(iKanbanColumns[1]);
	gi2.setArea(1, 2, 3, 2);
	grid.items.add(gi2);
	GridItem gi3(iKanbanColumns[2]);
	gi3.setArea(1, 3, 2, 3);
	grid.items.add(gi3);
	GridItem gi4(iKanbanColumns[3]);
	gi4.setArea(2, 3, 3, 3);
	grid.items.add(gi4);
	GridItem gi5(iKanbanColumns[4]);
	gi5.setArea(1, 4, 3, 4);
	grid.items.add(gi5);

	Rectangle<int> r(getLocalBounds());
	r.reduce(m, m);
	grid.performLayout(r);
	
}
