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
#pragma once

#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Double_Window.h>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <Fl/Fl_Choice.H>

#pragma warning(pop)

// we need to know what is in the world to show
#include <vector>
#include "Track.H"
#include "CallBacks.H"

#include "../src/GameObject/GameObject.h"

// other things we just deal with as pointers, to avoid circular references
class TrainView;

// if we're also making the sample solution, then we need to know 
// about the stuff we don't tell students
#ifdef EXAMPLE_SOLUTION
#include "TrainExample/TrainExample.H"
#endif

class LightingWidget : public Fl_Group
{
	void SetVec3(glm::vec3& vec, std::string _x, std::string _y, std::string _z)
	{
		glm::vec3 pos = glm::vec3(std::stof(_x), std::stof(_y), std::stof(_z));
		vec = pos;
	}

public:
	const int X = 0;
	const int Y = 1;
	const int Z = 2;

	//--------------------position------------
	Fl_Input* iPos[3] = { nullptr };
	Fl_Output* oPos[3] = { nullptr };

	//--------------------direction------------
	Fl_Input* iDir[3] = { nullptr };
	Fl_Output* oDir[3] = { nullptr };

	//--------------------material------------
	Fl_Input* iAmbient[3] = { nullptr };
	Fl_Output* oAmbient[3] = { nullptr };

	Fl_Input* iDiffuse[3] = { nullptr };
	Fl_Output* oDiffuse[3] = { nullptr };

	Fl_Input* iSpecular[3] = { nullptr };
	Fl_Output* oSpecular[3] = { nullptr };

	//-------------------attenuation-----------
	Fl_Input* iAttenuation[3] = { nullptr };
	Fl_Output* oAttenuation[3] = { nullptr };


	void CloseAllAttribute()
	{
		for (int i = 0; i < 3; i++)
		{
			iPos[i]->hide();
			oPos[i]->hide();

			iDir[i]->hide();
			oDir[i]->hide();

			iAmbient[i]->hide();
			oAmbient[i]->hide();

			iDiffuse[i]->hide();
			oDiffuse[i]->hide();

			iSpecular[i]->hide();
			oSpecular[i]->hide();

			iAttenuation[i]->hide();
			oAttenuation[i]->hide();
		}
	}
	void ShowPosition()
	{
		CloseAllAttribute();

		iPos[X]->show();
		oPos[X]->show();
		iPos[Y]->show();
		oPos[Y]->show();
		iPos[Z]->show();
		oPos[Z]->show();
	}
	void ShowDirection()
	{
		CloseAllAttribute();
		for (int i = 0; i < 3; i++)
		{
			iDir[i]->show();
			oDir[i]->show();
		}
	}
	void ShowAmbient()
	{
		CloseAllAttribute();
		for (int i = 0; i < 3; i++)
		{
			iAmbient[i]->show();
			oAmbient[i]->show();
		}
	}
	void ShowDiffuse()
	{
		CloseAllAttribute();
		for (int i = 0; i < 3; i++)
		{
			iDiffuse[i]->show();
			oDiffuse[i]->show();
		}
	}
	void ShowSpecular()
	{
		CloseAllAttribute();
		for (int i = 0; i < 3; i++)
		{
			iSpecular[i]->show();
			oSpecular[i]->show();
		}
	}
	void ShowAttenuation()
	{
		CloseAllAttribute();
		for (int i = 0; i < 3; i++)
		{
			iAttenuation[i]->show();
			oAttenuation[i]->show();
		}
	}

	Fl_Button* setupPositionCertainButtom;
	/// <summary>
	/// setup now choose lighting position from input
	/// </summary>
	void SetupLightingPosition();
	void SetupLightingDirection();
	void SetupLightingAmbient();
	void SetupLightingDiffuse();
	void SetupLightingSpecular();
	void SetupLightingAttenuation();

	/// <summary>
	/// restore lights of id which is from environment.light 
	/// </summary>
	std::vector<int> LightListIDs;
	int nowChooseLightIndex = -1;
	Fl_Browser* LightList;
	void AddLightBrowser(Lighting::BaseLight* _light);
	void ChooseWhichLight(int choose);
	void ShowLightAttribute(Lighting::DirLight* _light, int which);
	void ShowLightAttribute(Lighting::PointLight* _light, int which);
	void ShowLightAttribute(Lighting::SpotLight* _light, int which);
	
	Lighting::BaseLight* GetNowLight()
	{
		if (nowChooseLightIndex < LightList->size() && nowChooseLightIndex >= 0)
		{
			Environment* instance = Environment::GetInstance();
			if (instance->lights.Lightings.find(LightListIDs[nowChooseLightIndex]) != instance->lights.Lightings.end())
				return instance->lights.Lightings[LightListIDs[nowChooseLightIndex]];
			else
				return nullptr;
		}
		else
			return nullptr;
	}

	void PrintWhiceAttribute();
	enum class AttributeMenuIndex : int
	{
		position = 0,
		direction = 1,
		attenuation = 2,
		ambient = 3,
		diffuse = 4,
		specular = 5,
	};
	Fl_Choice* LightAttributeMenu;
	Fl_Button* attributeChoiceButton;

	LightingWidget(int _x, int _y, int _z, int _w) : Fl_Group(_x, _y, _z, _w)
	{
		this->begin();

		int pty = y();
		
		pty += 24;
		LightList = new Fl_Browser(605, pty, 80, 124, "Light List");
		LightList->type(2);
		LightList->callback((Fl_Callback*)ChooseWhichLightsCB,this);

		LightAttributeMenu = new Fl_Choice(690, pty, 100, 24);
		LightAttributeMenu->add("position");
		LightAttributeMenu->add("direction");
		LightAttributeMenu->add("attenuation");
		LightAttributeMenu->add("ambient");
		LightAttributeMenu->add("diffuse");
		LightAttributeMenu->add("specular");
		LightAttributeMenu->value(3);

		attributeChoiceButton = new Fl_Button(690, pty + 30, 24, 26, "Select");
		attributeChoiceButton->callback((Fl_Callback*)ChangeLightAttributeCB, this);

		pty += 145;

		int lightAttributePty = pty;
		iPos[X] = new Fl_Input(624, lightAttributePty, 50, 24, "P iX"); iPos[X]->hide();
		oPos[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");  oPos[X]->hide();
		lightAttributePty += 26;
		iPos[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "P iY");   iPos[Y]->hide();
		oPos[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");    oPos[Y]->hide();
		lightAttributePty += 26;
		iPos[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "P iZ");   iPos[Z]->hide();
		oPos[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");    oPos[Z]->hide();

		lightAttributePty = pty;
		iDir[X] = new Fl_Input(624, lightAttributePty, 50, 24, "D iX");   iDir[X]->hide();
		oDir[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");    oDir[X]->hide();
		lightAttributePty += 24;
		iDir[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "D iY");	iDir[Y]->hide();
		oDir[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");	oDir[Y]->hide();
		lightAttributePty += 24;
		iDir[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "D iZ");	iDir[Z]->hide();
		oDir[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");	oDir[Z]->hide();

		lightAttributePty = pty;
		iAmbient[X] = new Fl_Input(624, lightAttributePty, 50, 24, "A iX");   iAmbient[X]->hide();
		oAmbient[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");    oAmbient[X]->hide();
		lightAttributePty += 24;
		iAmbient[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "A iY");	iAmbient[Y]->hide();
		oAmbient[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");	oAmbient[Y]->hide();
		lightAttributePty += 24;
		iAmbient[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "A iZ");	iAmbient[Z]->hide();
		oAmbient[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");	oAmbient[Z]->hide();

		lightAttributePty = pty;
		iDiffuse[X] = new Fl_Input(624, lightAttributePty, 50, 24, "F iX");   iDiffuse[X]->hide();
		oDiffuse[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");    oDiffuse[X]->hide();
		lightAttributePty += 24;
		iDiffuse[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "F iY");	iDiffuse[Y]->hide();
		oDiffuse[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");	oDiffuse[Y]->hide();
		lightAttributePty += 24;
		iDiffuse[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "F iZ");	iDiffuse[Z]->hide();
		oDiffuse[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");	oDiffuse[Z]->hide();

		lightAttributePty = pty;
		iSpecular[X] = new Fl_Input(624, lightAttributePty, 50, 24, "F iX");   iSpecular[X]->hide();
		oSpecular[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");    oSpecular[X]->hide();
		lightAttributePty += 24;
		iSpecular[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "F iY");	iSpecular[Y]->hide();
		oSpecular[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");	oSpecular[Y]->hide();
		lightAttributePty += 24;
		iSpecular[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "F iZ");	iSpecular[Z]->hide();
		oSpecular[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");	oSpecular[Z]->hide();

		lightAttributePty = pty;
		iAttenuation[X] = new Fl_Input(624, lightAttributePty, 50, 24, "C iX");   iAttenuation[X]->hide();
		oAttenuation[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");    oAttenuation[X]->hide();
		lightAttributePty += 24;
		iAttenuation[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "L iY");	iAttenuation[Y]->hide();
		oAttenuation[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oY");	oAttenuation[Y]->hide();
		lightAttributePty += 24;
		iAttenuation[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "Q iZ");	iAttenuation[Z]->hide();
		oAttenuation[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oZ");	oAttenuation[Z]->hide();

		lightAttributePty += 45;
		setupPositionCertainButtom = new Fl_Button(710, lightAttributePty, 50, 24, "setup");
		setupPositionCertainButtom->callback((Fl_Callback*)LightingPositionCertainSetupCB, this);

		this->end();
	}

	void DamageMe();
};

class TrainWindow : public Fl_Double_Window {
	public:
		TrainWindow(const int x=50, const int y=50);

	public:
		// call this method when things change
		void damageMe();

		// this moves the train forward on the track - its up to you to do this
		// correctly. it gets called from the idle callback loop
		// it should handle forward and backwards
		void advanceTrain(float dir = 1);

		// simple helper function to set up a button
		void togglify(Fl_Button*, int state=0);

	public:
		// keep track of the stuff in the world
		CTrack				m_Track;

		// the widgets that make up the Window
		TrainView*			trainView;


		// are we animating the train?
		Fl_Button*			runButton;

		//---------------light attribute --------------------
		Fl_Group* widget;	// all widgets, grouped for resizing ease


		LightingWidget* lightingWidget;

		// we have other widgets as part of the sample solution
		// this is not for 559 students to know about
#ifdef EXAMPLE_SOLUTION
	ExampleWidgets ew;
#endif
};
