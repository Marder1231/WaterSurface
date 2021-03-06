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

void ChoiceWhichOAttributeCB(Fl_Button* w, TrainWindow* tw)
{
	tw->ShowWhichObjectAttribute();
}

void UpdataShaderCB(Fl_Button* w, TrainWindow* tw)
{
	tw->UpdataShader();
}

void ChooseWaveTypeCB(Fl_Browser* w, WaveWidget* ww)
{
	ww->ChooseWaveType(w->value() - 1);
}

#pragma region LightWidget

void ChooseWhichLightsCB(Fl_Browser* w, LightingWidget* lw)
{
	lw->ChooseWhichLight(w->value() - 1);
}

void LightingPositionCertainSetupCB(Fl_Button* w, LightingWidget* lw)
{
	if (lw->nowChooseLightIndex == -1)
		return;

	lw->PrintWhiceAttribute();
	lw->SetUpAttribute( (LightingWidget::EmAttributeMenuIndex)lw->LightAttributeMenu->value() );
}

void ChangeLightAttributeCB(Fl_Button* w, LightingWidget* lw)
{
	lw->PrintWhiceAttribute();
}

void DeleteLightCB(Fl_Button* w, LightingWidget* lw)
{
	lw->DeleteNowChooseLight();
}

#pragma endregion LightWidget