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

#define CHANGEATTRIBUTE_MAXSIZE 3

class WaveWidget : public Fl_Group
{
	const int X = 0;
	const int Y = 1;
	const int Z = 2;
public:

	Fl_Browser* WaveType = nullptr;

	Fl_Value_Slider* amplitude = nullptr;
	Fl_Value_Slider* speed = nullptr;
	Fl_Value_Slider* rainSize = nullptr;
	Fl_Value_Slider* reflectAttenuate = nullptr;
	Fl_Value_Slider* refractAttenuate = nullptr;

	enum class EmWaveType : int
	{
		Sin = 0,
		Height = 1,
	};
	EmWaveType NowChoiceWaveType = EmWaveType::Height;
	void ChooseWaveType(int value)
	{
		NowChoiceWaveType = EmWaveType(value);
	}

	WaveWidget(int _x, int _y, int _z, int _w) : Fl_Group(_x, _y, _z, _w)
	{
		this->begin();

		int pty = y();

		pty += 76;
		WaveType = new Fl_Browser(605, pty, 80, 124, "Wave Type");
		WaveType->add("Sin");
		WaveType->add("Height");
		WaveType->type(1);
		WaveType->callback((Fl_Callback*)ChooseWaveTypeCB, this);

		pty += 124 + 10;
		amplitude = new Fl_Value_Slider(700, pty, 100, 20, "Amplitude");
		amplitude->range(0, 10);
		amplitude->value(2.46f);
		amplitude->align(FL_ALIGN_LEFT);
		amplitude->type(FL_HORIZONTAL);

		pty += 20 + 10;
		speed = new Fl_Value_Slider(700, pty, 100, 20, "Speed");
		speed->range(0, 10);
		speed->value(0.246f);
		speed->align(FL_ALIGN_LEFT);
		speed->type(FL_HORIZONTAL);

		pty += 20 + 10;
		rainSize = new Fl_Value_Slider(700, pty, 100, 20, "Rain Size");
		rainSize->range(0, 10);
		rainSize->value(0);
		rainSize->align(FL_ALIGN_LEFT);
		rainSize->type(FL_HORIZONTAL);

		pty += 20 + 10;
		reflectAttenuate = new Fl_Value_Slider(700, pty, 100, 20, "reflect");
		reflectAttenuate->range(0, 10);
		reflectAttenuate->value(6.24f);
		reflectAttenuate->align(FL_ALIGN_LEFT);
		reflectAttenuate->type(FL_HORIZONTAL);

		pty += 20 + 10;
		refractAttenuate = new Fl_Value_Slider(700, pty, 100, 20, "refract");
		refractAttenuate->range(0, 10);
		refractAttenuate->value(6.24f);
		refractAttenuate->align(FL_ALIGN_LEFT);
		refractAttenuate->type(FL_HORIZONTAL);

		this->end();
	}
};


class ObjectWidget : public Fl_Group
{
	const int ChangeSize = 1;
public:
	const int X = 0;
	const int Y = 1;
	const int Z = 2;

	Fl_Input* fl_Input[CHANGEATTRIBUTE_MAXSIZE] = { nullptr };
	Fl_Output* fl_Output[CHANGEATTRIBUTE_MAXSIZE] = { nullptr };
	
	Fl_Browser* ObjectList;

	Fl_Button* attributeChoiceButton;
	Fl_Choice* AttributeMenu;

	enum class EmAttributeIndex : int
	{
		position = 0,
		diffuse = 1,
		specular = 2,
	};
	ObjectWidget(int _x, int _y, int _z, int _w) : Fl_Group(_x, _y, _z, _w)
	{
		this->begin();

		int pty = y();

		pty += 76;
		ObjectList = new Fl_Browser(605, pty, 80, 124, "Object List");
		ObjectList->type(2);
		ObjectList->callback((Fl_Callback*)ChooseWhichLightsCB, this);

		AttributeMenu = new Fl_Choice(690, pty, 100, 24);
		AttributeMenu->add("position");
		AttributeMenu->add("diffuse");
		AttributeMenu->add("specular");
		AttributeMenu->value(1);

		attributeChoiceButton = new Fl_Button(690, pty + 30, 24, 26, "Select");
		attributeChoiceButton->callback((Fl_Callback*)ChangeLightAttributeCB, this);

		pty += 99;

		this->end();
	}
};

class LightingWidget : public Fl_Group
{
	void SetVec3(glm::vec3& vec, std::string _x, std::string _y, std::string _z)
	{
		if (_x.size() == 0 || _y.size() == 0 || _z.size() < 0)
			return;
		glm::vec3 pos = glm::vec3(std::stof(_x), std::stof(_y), std::stof(_z));
		vec = pos;
	}

public:
	const int X = 0;
	const int Y = 1;
	const int Z = 2;

	Fl_Input* inputAttribute[3] = { nullptr };
	Fl_Output* outputAttribute[3] = { nullptr };

	void CloseAllAttribute()
	{
		for (int i = 0; i < 3; i++)
		{
			inputAttribute[i]->hide();
			outputAttribute[i]->hide();
		}
	}
	void ShowAttribute(int _which)
	{
		CloseAllAttribute();

		if (_which > 3)
			throw new std::exception("didn't allow show much attribute");

		for (int i = 0; i < _which; i++)
		{
			inputAttribute[i]->show();
			outputAttribute[i]->show();
		}
	}
	
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
	
	Fl_Button* deleteLightButton;
	void DeleteNowChooseLight()
	{
		if (nowChooseLightIndex == -1)
			return;

		Lighting::BaseLight* _light = GetNowLight();

		Environment* instance = Environment::GetInstance();


		//$$$ bug LightList Browers already delete, but it also display
		//remove this light at LightList
		LightList->remove(nowChooseLightIndex+1);
		LightListIDs.erase(LightListIDs.begin() + nowChooseLightIndex);

		nowChooseLightIndex = -1;

		//remove this light in environment
		instance->lights.RemoveLight(_light);

		LightList->damage();
		this->DamageMe();
	}

	Lighting::BaseLight* GetNowLight()
	{
		if (nowChooseLightIndex < LightList->size() && nowChooseLightIndex >= 0)
		{
			Environment* instance = Environment::GetInstance();
			if (nowChooseLightIndex >= instance->lights.Lightings.size())
				return nullptr;

			if (instance->lights.Lightings.find(LightListIDs[nowChooseLightIndex]) != instance->lights.Lightings.end())
				return instance->lights.Lightings[LightListIDs[nowChooseLightIndex]];
			else
				return nullptr;
		}
		else
			return nullptr;
	}

	/// <summary>
	/// show now choice element of attribute 
	/// </summary>
	void PrintWhiceAttribute();
	enum class EmAttributeMenuIndex : int
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

	Fl_Button* setupPositionCertainButtom;
	/// <summary>
	/// setup now choose lighting position from input
	/// </summary>
	void SetUpAttribute(EmAttributeMenuIndex _menu)
	{
		//強制設定成目前所選取的屬性
		int nowAttributeIndex = LightAttributeMenu->value();
		Lighting::BaseLight* nowLight = GetNowLight();
		if (nowLight == nullptr)
			return;

		switch (_menu)
		{
			case EmAttributeMenuIndex::position:
			{
				if (nowLight->Type == Lighting::EmLightType::Point)
				{
					Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

					SetVec3(light->GetPosition(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					SetVec3(light->GetPosition(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			case EmAttributeMenuIndex::direction:
			{
				if (nowLight->Type == Lighting::EmLightType::Dir)
				{
					Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

					SetVec3(light->GetDirection(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					SetVec3(light->GetDirection(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			case EmAttributeMenuIndex::ambient:
			{
				if (nowLight->Type == Lighting::EmLightType::Dir)
				{
					Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

					SetVec3(light->GetAmbient(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Point)
				{
					Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

					SetVec3(light->GetAmbient(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					SetVec3(light->GetAmbient(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			case EmAttributeMenuIndex::diffuse:
			{
				if (nowLight->Type == Lighting::EmLightType::Dir)
				{
					Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

					SetVec3(light->GetDiffuse(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Point)
				{
					Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

					SetVec3(light->GetDiffuse(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					SetVec3(light->GetDiffuse(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			case EmAttributeMenuIndex::specular:
			{
				if (nowLight->Type == Lighting::EmLightType::Dir)
				{
					Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

					SetVec3(light->GetSpecular(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Point)
				{
					Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

					SetVec3(light->GetSpecular(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					SetVec3(light->GetSpecular(), inputAttribute[X]->value(), inputAttribute[Y]->value(), inputAttribute[Z]->value());
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			case EmAttributeMenuIndex::attenuation:
			{
				if (nowLight->Type == Lighting::EmLightType::Point)
				{
					Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

					light->SetConstant(std::stof(inputAttribute[0]->value()));
					light->SetLinear(std::stof(inputAttribute[1]->value()));
					light->SetQuadratic(std::stof(inputAttribute[2]->value()));
					ShowLightAttribute(light, nowAttributeIndex);
				}
				else if (nowLight->Type == Lighting::EmLightType::Spot)
				{
					Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

					light->SetConstant(std::stof(inputAttribute[0]->value()));
					light->SetLinear(std::stof(inputAttribute[1]->value()));
					light->SetQuadratic(std::stof(inputAttribute[2]->value()));
					ShowLightAttribute(light, nowAttributeIndex);
				}
			}break;
			default:
				throw new std::exception("this attribute is unkown");
		}

		this->DamageMe();
	}

	LightingWidget(int _x, int _y, int _z, int _w) : Fl_Group(_x, _y, _z, _w)
	{
		this->begin();

		int pty = y();
		
		pty += 76;
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

		pty += 146;

		int lightAttributePty = pty;
		inputAttribute[X] = new Fl_Input(624, lightAttributePty, 50, 24, "P iX"); inputAttribute[X]->hide();
		outputAttribute[X] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");  outputAttribute[X]->hide();
		lightAttributePty += 26;
		inputAttribute[Y] = new Fl_Input(624, lightAttributePty, 50, 24, "P iX"); inputAttribute[Y]->hide();
		outputAttribute[Y] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");  outputAttribute[Y]->hide();
		lightAttributePty += 26;
		inputAttribute[Z] = new Fl_Input(624, lightAttributePty, 50, 24, "P iX"); inputAttribute[Z]->hide();
		outputAttribute[Z] = new Fl_Output(710, lightAttributePty, 50, 24, "oX");  outputAttribute[Z]->hide();

		lightAttributePty += 45;
		setupPositionCertainButtom = new Fl_Button(710, lightAttributePty, 50, 24, "setup");
		setupPositionCertainButtom->callback((Fl_Callback*)LightingPositionCertainSetupCB, this);

		//$$$ delete and add light
		deleteLightButton = new Fl_Button(605, lightAttributePty, 50, 24, "delete");
		deleteLightButton->callback((Fl_Callback*)DeleteLightCB, this);

		this->end();
	}

	void DamageMe();
};

class TrainWindow : public Fl_Double_Window 
{

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
	
	void UpdataShader();
	Fl_Button* btn_updataShader;
	
	Fl_Group* widget;	// all widgets, grouped for resizing ease
	
	
	//------------------GameObejct attribute widget-----------
#pragma region GameObjectAttributeWidget

public:
	int nowChoiceAttribute = -1;

	enum class EmObjectChoice : int
	{
		Lighting = 0,
		Object = 1,
		Wave = 2,
	};
	Fl_Choice* ChoiceWhichGameObjectAttribute;
	Fl_Button* choiceWhichObjectAttributeButton;

	LightingWidget* lightingWidget;
	ObjectWidget* objectWidget;
	WaveWidget* waveWidget;

public:
	void ShowWhichObjectAttribute()
	{
		nowChoiceAttribute = ChoiceWhichGameObjectAttribute->value();
		
		if (nowChoiceAttribute == -1)
			return;

		//$$$ hide now choice
		lightingWidget->hide();
		objectWidget->hide();
		waveWidget->hide();

		switch (nowChoiceAttribute)
		{
		case int(EmObjectChoice::Lighting):
			{
				lightingWidget->show();
			}break;
		case int(EmObjectChoice::Wave):
			{
				waveWidget->show();
			}break;
		default:
			break;
		}
		//show object
	}
#pragma endregion GameObjectAttributeWidget

		// we have other widgets as part of the sample solution
		// this is not for 559 students to know about
#ifdef EXAMPLE_SOLUTION
	ExampleWidgets ew;
#endif
};
