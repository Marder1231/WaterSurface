/************************************************************************
     File:        TrainWindow.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						this class defines the window in which the project 
						runs - its the outer windows that contain all of 
						the widgets, including the "TrainView" which has the 
						actual OpenGL window in which the train is drawn

						You might want to modify this class to add new widgets
						for controlling	your train

						This takes care of lots of things - including installing 
						itself into the FlTk "idle" loop so that we get periodic 
						updates (if we're running the train).


     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <FL/fl.h>
#include <FL/Fl_Box.h>

// for using the real time clock
#include <time.h>

#include "TrainWindow.H"
#include "TrainView.H"
#include "CallBacks.H"

//************************************************************************
//
// * Constructor
//========================================================================
TrainWindow::
TrainWindow(const int x, const int y) 
	: Fl_Double_Window(x,y,800,600,"Train and Roller Coaster")
//========================================================================
{
	srand(time(NULL));
	// make all of the widgets
	begin();	// add to this widget
	{
		int pty=5;			// where the last widgets were drawn

		trainView = new TrainView(5,5,590,590);
		trainView->tw = this;
		trainView->m_pTrack = &m_Track;
		this->resizable(trainView);

		// to make resizing work better, put all the widgets in a group
		widgets = new Fl_Group(600,5,190,590);
		widgets->begin();

		runButton = new Fl_Button(605,pty,60,20,"Run");
		togglify(runButton);

		pty += 24;
		LightList = new Fl_Browser(605, pty, 60, 24, "Light List");
		LightList->type(2);
		LightList->callback((Fl_Callback*)damageCB, this);

		

#ifdef EXAMPLE_SOLUTION
		makeExampleWidgets(this,pty);
#endif

		// we need to make a little phantom widget to have things resize correctly
		Fl_Box* resizebox = new Fl_Box(600,595,200,5);
		widgets->resizable(resizebox);

		widgets->end();
	}
	end();	// done adding to this widget

	// set up callback on idle
	Fl::add_idle((void (*)(void*))runButtonCB,this);
}

//************************************************************************
//
// * handy utility to make a button into a toggle
//========================================================================
void TrainWindow::
togglify(Fl_Button* b, int val)
//========================================================================
{
	b->type(FL_TOGGLE_BUTTON);		// toggle
	b->value(val);		// turned off
	b->selection_color((Fl_Color)3); // yellow when pressed	
	b->callback((Fl_Callback*)damageCB,this);
}

void TrainWindow::AddLightBrowser(Lighting::BaseLight& _light)
{
	std::string strLightType = "Dir : ";
	if (_light.Type == Lighting::EmLightType::Point)
		strLightType = "Point : ";
	else if (_light.Type == Lighting::EmLightType::Spot)
		strLightType = "Spot : ";

	LightList->add( (strLightType + std::to_string(_light.ID)).c_str() );
}

//************************************************************************
//
// *
//========================================================================
void TrainWindow::
damageMe()
//========================================================================
{
	if (trainView->selectedCube >= ((int)m_Track.points.size()))
		trainView->selectedCube = 0;
	trainView->damage(1);
}

//************************************************************************
//
// * This will get called (approximately) 30 times per second
//   if the run button is pressed
//========================================================================
void TrainWindow::
advanceTrain(float dir)
//========================================================================
{
	//#####################################################################
	// TODO: make this work for your train
	trainView->HeightWave.timer += .1;

	//#####################################################################
#ifdef EXAMPLE_SOLUTION
	// note - we give a little bit more example code here than normal,
	// so you can see how this works

	if (arcLength->value()) {
		float vel = ew.physics->value() ? physicsSpeed(this) : dir * (float)speed->value();
		world.trainU += arclenVtoV(world.trainU, vel, this);
	} else {
		world.trainU +=  dir * ((float)speed->value() * .1f);
	}

	float nct = static_cast<float>(world.points.size());
	if (world.trainU > nct) world.trainU -= nct;
	if (world.trainU < 0) world.trainU += nct;
#endif
}