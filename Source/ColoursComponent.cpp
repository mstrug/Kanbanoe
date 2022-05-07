/*
  ==============================================================================

    ColoursComponent.cpp
    Created: 13 Dec 2018 10:55:21pm
    Author:  Michał

  ==============================================================================
*/

#include "ColoursComponent.h"

const int SquareWidth = 30;

//==============================================================================
ColoursComponent::ColoursComponent(int xColourCount, int yColourCount, ColourPalette& aPalette, int aIdxSelected) : iListener( nullptr )
{
	Random r;
	int group = r.nextInt();
	int w = SquareWidth;
	int h = w;

	for (int i = 0, k = 0; i < yColourCount; i++)
	{
		if (aPalette.getColorCount() <= k) break;

		OwnedArray< TextButton >* ar = new OwnedArray< TextButton >();
		iButtons.add(ar);
		for (int j = 0; j < xColourCount; j++)
		{
			auto b = new TextButton();

			if (aIdxSelected == k)
			{
				b->setToggleState(true, dontSendNotification );
			}
			b->setRadioGroupId(group);
			b->setClickingTogglesState(true);
			b->setBounds(j * w, i * h, w, h);
			if (aPalette.getColorCount() <= k)
			{
				if (i == 0)
				{ // for first line return
					delete b;
					break;
				}
				b->setEnabled(false);
			}
			else
			{
				b->setColour(TextButton::buttonColourId, aPalette.getColor(k));
				b->setColour(TextButton::buttonOnColourId, aPalette.getColor(k));
				b->setColour(TextButton::textColourOnId, aPalette.getColorActive(k++));
			}

			int f = 0;
			if (j == 0 && xColourCount > 1) f |= Button::ConnectedOnRight;
			if (j == xColourCount - 1 && xColourCount > 1) f |= Button::ConnectedOnLeft;
			if (j > 0 && j < xColourCount - 1 && xColourCount > 1) f |= Button::ConnectedOnLeft | Button::ConnectedOnRight;
			if (i == 0 && yColourCount > 1) f |= Button::ConnectedOnBottom;
			if (i == xColourCount - 1 && yColourCount > 1) f |= Button::ConnectedOnTop;
			if (i > 0 && i < yColourCount - 1 && yColourCount > 1) f |= Button::ConnectedOnTop | Button::ConnectedOnBottom;

			f |= Button::ConnectedOnLeft | Button::ConnectedOnRight | Button::ConnectedOnTop | Button::ConnectedOnBottom;
			b->setConnectedEdges(f);
			b->getProperties().set("idx", j);

			b->onStateChange = [this, b]
			{
				if (b->getToggleState())
				{
					if (this->iListener)
					{
						int idx = b->getProperties()["idx"];
						this->iListener->ColorChanged( idx );
					}
				}
			};

			addAndMakeVisible(b);
			iButtons[i]->add(b);
		}
	}	

	setSize(iButtons[0]->size() * SquareWidth, iButtons.size() * SquareWidth);
}

ColoursComponent::~ColoursComponent()
{
}

int ColoursComponent::getSelectedColourIdx()
{
	for (int i = 0, k = 0; i < iButtons.size(); i++)
	{
		for (int j = 0; j < iButtons[i]->size(); j++)
		{
			if ( (*iButtons[i])[j]->getToggleState() )
			{
				return k;
			}
			k++;
		}
	}
	return 0;
}

void ColoursComponent::setListener(ColoursComponentListener* aListener)
{
	iListener = aListener;
}

void ColoursComponent::paintOverChildren(Graphics& g)
{
	g.setColour(Colours::lightgrey);
	for (int i = 0; i < iButtons.size(); i++)
	{
		for (int j = 0; j < iButtons[i]->size(); j++)
		{
			if ((*iButtons[i])[j]->getToggleState())
			{
				auto r = (*iButtons[i])[j]->getBounds();// .reduced(1);
				g.drawRect(r, 2);
				/*r.reduce(6, 6);
				r.translate(-1, 0);
				g.drawLine(r.getPosition().x + 2, r.getCentreY(), r.getCentreX(), r.getBottom(), 3);
				g.drawLine(r.getRight(), r.getTopLeft().y, r.getCentreX(), r.getBottom(), 3);*/
				return;
			}
		}
	}
}

void ColoursComponent::resized()
{
}

int ColoursComponent::getColoursWidth()
{
	return SquareWidth * iButtons[0]->size();
}
