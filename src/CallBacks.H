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

// Preclarify to prevent the compiler error
class LightingWidget;
class TrainWindow;
class Fl_Widget;

// Something change and thus we need to update the view
void damageCB(Fl_Widget*, TrainWindow* tw);

// Idle callback: for run the step of the window
void runButtonCB(TrainWindow* tw);


//------------------------------------------------------------------------
//                         LightingWidget call back
//lightlist browser choose which light
void ChooseWhichLightsCB(Fl_Browser* w, LightingWidget* lw);

//certain setup position
void LightingPositionCertainSetupCB(Fl_Button* w, LightingWidget* lw);

void ChangeLightAttributeCB(Fl_Button* w, LightingWidget* lw);
//------------------------------------------------------------------------