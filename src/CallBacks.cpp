/************************************************************************
     File:        CallBacks.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Header file to define callback functions.
						define the callbacks for the TrainWindow

						these are little functions that get called when the 
						various widgets
						get accessed (or the fltk timer ticks). these 
						functions are used 
						when TrainWindow sets itself up.

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

#include <time.h>
#include <math.h>

#include "TrainWindow.H"
#include "TrainView.H"
#include "CallBacks.H"

#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_File_Chooser.H>
#include <Fl/math.h>
#pragma warning(pop)



//***************************************************************************
//
// * any time something changes, you need to force a redraw
//===========================================================================
void damageCB(Fl_Widget*, TrainWindow* tw)
{
	tw->damageMe();
}

static unsigned long lastRedraw = 0;
//***************************************************************************
//
// * Callback for idling - if things are sitting, this gets called
// if the run button is pushed, then we need to make the train go.
// This is taken from the old "RunButton" demo.
// another nice problem to have - most likely, we'll be too fast
// don't draw more than 30 times per second
//===========================================================================
void runButtonCB(TrainWindow* tw)
//===========================================================================
{
	if (tw->runButton->value()) {	// only advance time if appropriate
		if (clock() - lastRedraw > CLOCKS_PER_SEC/30) {
			lastRedraw = clock();
			tw->advanceTrain();
			tw->damageMe();
		}
	}
}

void ChooseWhichLightsCB(Fl_Browser* w, LightingWidget* lw)
{
	lw->ChooseWhichLight(w->value() - 1);
}

void LightingPositionCertainSetupCB(Fl_Button* w, LightingWidget* lw)
{
	if (lw->nowChooseLightIndex == -1)
		return;

	if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::position)
		lw->SetupLightingPosition();
	else if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::direction)
		lw->SetupLightingDirection();
	else if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::ambient)
		lw->SetupLightingAmbient();
	else if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::diffuse)
		lw->SetupLightingDiffuse();
	else if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::specular)
		lw->SetupLightingSpecular();
	else if (lw->LightAttributeMenu->value() == (int)LightingWidget::AttributeMenuIndex::attenuation)
		lw->SetupLightingAttenuation();
	else throw new std::exception("this attribute didn't define");
}

void ChangeLightAttributeCB(Fl_Button* w, LightingWidget* lw)
{
	lw->PrintWhiceAttribute();
}