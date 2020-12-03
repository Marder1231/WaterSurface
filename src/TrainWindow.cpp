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
#include <string>

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
		widget = new Fl_Group(600,5,190,590);
		widget->begin();

		runButton = new Fl_Button(605,pty,60,20,"Run");
		togglify(runButton);
		
		ChoiceWhichGameObjectAttribute = new Fl_Choice(700, pty, 60, 24);
		ChoiceWhichGameObjectAttribute->add("lighting");
		ChoiceWhichGameObjectAttribute->add("Object");

		choiceWhichObjectAttributeButton = new Fl_Button(700, pty + 30, 24, 24, "Select");
		//choiceWhichObjectAttributeButton->callback((Fl_Callback*) /*call back*/, this);

		lightingWidget = new LightingWidget(605, pty, 190, 590);
		this->add(lightingWidget);
		lightingWidget->hide();

		objectWidget = new ObjectWidget(605, pty, 190, 590);
		this->add(objectWidget);
		//objectWidget->hide();

#ifdef EXAMPLE_SOLUTION
		makeExampleWidgets(this,pty);
#endif

		// we need to make a little phantom widget to have things resize correctly
		Fl_Box* resizebox = new Fl_Box(600,595,200,5);
		widget->resizable(resizebox);

		widget->end();
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

#pragma region LightingWidget
void LightingWidget::AddLightBrowser(Lighting::BaseLight* _light)
{
	Environment::GetInstance()->lights.AddLight(_light);

	std::string strLightType = "Dir : ";
	if (_light->Type == Lighting::EmLightType::Point)
		strLightType = "Point : ";
	else if (_light->Type == Lighting::EmLightType::Spot)
		strLightType = "Spot : ";

	std::cout << _light->ID <<std::endl;
	LightListIDs.push_back(_light->ID);
	LightList->add( (strLightType + std::to_string(_light->ID)).c_str() );
}

void LightingWidget::ShowLightAttribute(Lighting::DirLight* _light, int which)
{
	if (which == (int)AttributeMenuIndex::position)
	{

	}
	else if (which == (int)AttributeMenuIndex::direction)
	{
		ShowDirection();
		oDir[X]->value(std::to_string(_light->GetDirection().x).c_str());
		oDir[Y]->value(std::to_string(_light->GetDirection().y).c_str());
		oDir[Z]->value(std::to_string(_light->GetDirection().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::attenuation)	//attenuation
	{

	}
	else if (which == (int)AttributeMenuIndex::ambient)	//ambient
	{
		ShowAmbient();
		oAmbient[X]->value(std::to_string(_light->GetAmbient().x).c_str());
		oAmbient[Y]->value(std::to_string(_light->GetAmbient().y).c_str());
		oAmbient[Z]->value(std::to_string(_light->GetAmbient().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::diffuse)	//diffuse
	{
		ShowDiffuse();
		oDiffuse[X]->value(std::to_string(_light->GetDiffuse().x).c_str());
		oDiffuse[Y]->value(std::to_string(_light->GetDiffuse().y).c_str());
		oDiffuse[Z]->value(std::to_string(_light->GetDiffuse().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::specular)	//specular
	{
		ShowSpecular();
		oSpecular[X]->value(std::to_string(_light->GetSpecular().x).c_str());
		oSpecular[Y]->value(std::to_string(_light->GetSpecular().y).c_str());
		oSpecular[Z]->value(std::to_string(_light->GetSpecular().z).c_str());
	}
}

void LightingWidget::ShowLightAttribute(Lighting::PointLight* _light, int which)
{
	//position
	if (which == (int)AttributeMenuIndex::position)
	{
		ShowPosition();
		oPos[X]->value(std::to_string(_light->GetPosition().x).c_str());
		oPos[Y]->value(std::to_string(_light->GetPosition().y).c_str());
		oPos[Z]->value(std::to_string(_light->GetPosition().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::direction)	//direction
	{

	}
	else if (which == (int)AttributeMenuIndex::attenuation)	//attenuation
	{
		ShowAttenuation();
		oAttenuation[X]->value(std::to_string(_light->GetConstant()).c_str());
		oAttenuation[Y]->value(std::to_string(_light->GetLinear()).c_str());
		oAttenuation[Z]->value(std::to_string(_light->GetQuadratic()).c_str());
	}
	else if (which == (int)AttributeMenuIndex::ambient)	//ambient
	{
		ShowAmbient();
		oAmbient[X]->value(std::to_string(_light->GetAmbient().x).c_str());
		oAmbient[Y]->value(std::to_string(_light->GetAmbient().y).c_str());
		oAmbient[Z]->value(std::to_string(_light->GetAmbient().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::diffuse)	//diffuse
	{
		ShowDiffuse();
		oDiffuse[X]->value(std::to_string(_light->GetDiffuse().x).c_str());
		oDiffuse[Y]->value(std::to_string(_light->GetDiffuse().y).c_str());
		oDiffuse[Z]->value(std::to_string(_light->GetDiffuse().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::specular)	//specular
	{
		ShowSpecular();
		oSpecular[X]->value(std::to_string(_light->GetSpecular().x).c_str());
		oSpecular[Y]->value(std::to_string(_light->GetSpecular().y).c_str());
		oSpecular[Z]->value(std::to_string(_light->GetSpecular().z).c_str());
	}
	this->DamageMe();
}

void LightingWidget::ShowLightAttribute(Lighting::SpotLight* _light, int which)
{
	if (which == (int)AttributeMenuIndex::position)
	{
		ShowPosition();
		oPos[X]->value(std::to_string(_light->GetPosition().x).c_str());
		oPos[Y]->value(std::to_string(_light->GetPosition().y).c_str());
		oPos[Z]->value(std::to_string(_light->GetPosition().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::direction)
	{
		ShowDirection();
		oDir[X]->value(std::to_string(_light->GetDirection().x).c_str());
		oDir[Y]->value(std::to_string(_light->GetDirection().y).c_str());
		oDir[Z]->value(std::to_string(_light->GetDirection().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::attenuation)	//attenuation
	{
		ShowAttenuation();
		oAttenuation[X]->value(std::to_string(_light->GetConstant()).c_str());
		oAttenuation[Y]->value(std::to_string(_light->GetLinear()).c_str());
		oAttenuation[Z]->value(std::to_string(_light->GetQuadratic()).c_str());
	}
	else if (which == (int)AttributeMenuIndex::ambient)	//ambient
	{
		ShowAmbient();
		oAmbient[X]->value(std::to_string(_light->GetAmbient().x).c_str());
		oAmbient[Y]->value(std::to_string(_light->GetAmbient().y).c_str());
		oAmbient[Z]->value(std::to_string(_light->GetAmbient().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::diffuse)	//diffuse
	{
		ShowDiffuse();
		oDiffuse[X]->value(std::to_string(_light->GetDiffuse().x).c_str());
		oDiffuse[Y]->value(std::to_string(_light->GetDiffuse().y).c_str());
		oDiffuse[Z]->value(std::to_string(_light->GetDiffuse().z).c_str());
	}
	else if (which == (int)AttributeMenuIndex::specular)	//specular
	{
		ShowSpecular();
		oSpecular[X]->value(std::to_string(_light->GetSpecular().x).c_str());
		oSpecular[Y]->value(std::to_string(_light->GetSpecular().y).c_str());
		oSpecular[Z]->value(std::to_string(_light->GetSpecular().z).c_str());
	}
}

void LightingWidget::PrintWhiceAttribute()
{
	CloseAllAttribute();

	int nowChoiceValue = this->LightAttributeMenu->value();
	Lighting::BaseLight* light = GetNowLight();
	if (light == nullptr)
		return;

	//position
	if (nowChoiceValue == (int)AttributeMenuIndex::position)
	{
		if (light->Type == Lighting::EmLightType::Point)
		{
			Lighting::PointLight* pointLight = static_cast<Lighting::PointLight*>(light);
			ShowLightAttribute(pointLight, nowChoiceValue);
		}
		else if (light->Type == Lighting::EmLightType::Spot)
		{
			Lighting::SpotLight* spot = static_cast<Lighting::SpotLight*>(light);
			ShowLightAttribute(spot, nowChoiceValue);
		}
	}
	else if (nowChoiceValue == (int)AttributeMenuIndex::direction)	//direction
	{
		if (light->Type == Lighting::EmLightType::Dir)
		{
			Lighting::DirLight* dir = static_cast<Lighting::DirLight*>(light);
			ShowLightAttribute(dir, nowChoiceValue);
		}
		else if (light->Type == Lighting::EmLightType::Spot)
		{
			Lighting::SpotLight* spot = static_cast<Lighting::SpotLight*>(light);
			ShowLightAttribute(spot, nowChoiceValue);
		}
	}
	else if (nowChoiceValue == (int)AttributeMenuIndex::attenuation)	//attenuation
	{
		if (light->Type == Lighting::EmLightType::Point)
		{
			Lighting::PointLight* pointLight = static_cast<Lighting::PointLight*>(light);
			ShowLightAttribute(pointLight, nowChoiceValue);
		}
		else if (light->Type == Lighting::EmLightType::Spot)
		{
			Lighting::SpotLight* spot = static_cast<Lighting::SpotLight*>(light);
			ShowLightAttribute(spot, nowChoiceValue);
		}
	}
	else if (nowChoiceValue == (int)AttributeMenuIndex::ambient
		|| nowChoiceValue == (int)AttributeMenuIndex::diffuse
		|| nowChoiceValue == (int)AttributeMenuIndex::specular)	//material
	{
		if (light->Type == Lighting::EmLightType::Point)
		{
			Lighting::PointLight* pointLight = static_cast<Lighting::PointLight*>(light);
			ShowLightAttribute(pointLight, nowChoiceValue);
		}
		else if (light->Type == Lighting::EmLightType::Spot)
		{
			Lighting::SpotLight* spot = static_cast<Lighting::SpotLight*>(light);
			ShowLightAttribute(spot, nowChoiceValue);
		}
		else if (light->Type == Lighting::EmLightType::Dir)
		{
			Lighting::DirLight* dir = static_cast<Lighting::DirLight*>(light);
			ShowLightAttribute(dir, nowChoiceValue);
		}
	}
}

void LightingWidget::DamageMe()
{
	this->damage();

	//redraw main window
	TrainWindow* tw = static_cast<TrainWindow*>(this->parent());
	if (tw != nullptr)
		tw->damageMe();
}

void LightingWidget::ChooseWhichLight(int choose)
{
	nowChooseLightIndex = choose;
	Lighting::BaseLight* _light = GetNowLight();
	if (_light == nullptr)
		return;
	switch (_light->Type)
	{
		case Lighting::EmLightType::Dir:
		{
			Lighting::DirLight* dirLight = static_cast<Lighting::DirLight*>(_light);
		}break;
		case Lighting::EmLightType::Point:
		{
			Lighting::PointLight* positionLight = static_cast<Lighting::PointLight*>(_light);
			
		}break;
		case Lighting::EmLightType::Spot:
		{
			Lighting::SpotLight* spotLight = static_cast<Lighting::SpotLight*>(_light);
		}break;
		default:
			break;
	}

	PrintWhiceAttribute();
	this->DamageMe();
}

void LightingWidget::SetupLightingPosition()
{
	int nowAttributeIndex = LightAttributeMenu->value();
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;


	if (nowLight->Type == Lighting::EmLightType::Point)
	{
		Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

		SetVec3(light->GetPosition(), iPos[X]->value(), iPos[Y]->value(), iPos[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		SetVec3(light->GetPosition(), iPos[X]->value(), iPos[Y]->value(), iPos[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

void LightingWidget::SetupLightingDirection()
{
	int nowAttributeIndex = LightAttributeMenu->value();	
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;
	if (nowLight->Type == Lighting::EmLightType::Dir)
	{
		Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

		SetVec3(light->GetDirection(), iDir[X]->value(), iDir[Y]->value(), iDir[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		SetVec3(light->GetDirection(), iDir[X]->value(), iDir[Y]->value(), iDir[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

void LightingWidget::SetupLightingAmbient()
{
	int nowAttributeIndex = LightAttributeMenu->value();
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;
	if (nowLight->Type == Lighting::EmLightType::Dir)
	{
		Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

		SetVec3(light->GetAmbient(), iAmbient[X]->value(), iAmbient[Y]->value(), iAmbient[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Point)
	{
		Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

		SetVec3(light->GetAmbient(), iAmbient[X]->value(), iAmbient[Y]->value(), iAmbient[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		SetVec3(light->GetAmbient(), iAmbient[X]->value(), iAmbient[Y]->value(), iAmbient[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

void LightingWidget::SetupLightingDiffuse()
{
	int nowAttributeIndex = LightAttributeMenu->value();
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;
	if (nowLight->Type == Lighting::EmLightType::Dir)
	{
		Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

		SetVec3(light->GetDiffuse(), iDiffuse[X]->value(), iDiffuse[Y]->value(), iDiffuse[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Point)
	{
		Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

		SetVec3(light->GetDiffuse(), iDiffuse[X]->value(), iDiffuse[Y]->value(), iDiffuse[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		SetVec3(light->GetDiffuse(), iDiffuse[X]->value(), iDiffuse[Y]->value(), iDiffuse[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

void LightingWidget::SetupLightingSpecular()
{
	int nowAttributeIndex = LightAttributeMenu->value();
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;
	if (nowLight->Type == Lighting::EmLightType::Dir)
	{
		Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

		SetVec3(light->GetSpecular(), iSpecular[X]->value(), iSpecular[Y]->value(), iSpecular[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Point)
	{
		Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

		SetVec3(light->GetSpecular(), iSpecular[X]->value(), iSpecular[Y]->value(), iSpecular[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		SetVec3(light->GetSpecular(), iSpecular[X]->value(), iSpecular[Y]->value(), iSpecular[Z]->value());
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

void LightingWidget::SetupLightingAttenuation()
{
	int nowAttributeIndex = LightAttributeMenu->value();
	Lighting::BaseLight* nowLight = GetNowLight();
	if (nowLight == nullptr)
		return;
	if (nowLight->Type == Lighting::EmLightType::Dir)
	{
		Lighting::DirLight* light = static_cast<Lighting::DirLight*>(nowLight);

		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Point)
	{
		Lighting::PointLight* light = static_cast<Lighting::PointLight*>(nowLight);

		light->SetConstant(std::stof(iAttenuation[0]->value()));
		light->SetLinear(std::stof(iAttenuation[1]->value()));
		light->SetQuadratic(std::stof(iAttenuation[2]->value()));
		ShowLightAttribute(light, nowAttributeIndex);
	}
	else if (nowLight->Type == Lighting::EmLightType::Spot)
	{
		Lighting::SpotLight* light = static_cast<Lighting::SpotLight*>(nowLight);

		light->SetConstant(std::stof(iAttenuation[0]->value()));
		light->SetLinear(std::stof(iAttenuation[1]->value()));
		light->SetQuadratic(std::stof(iAttenuation[2]->value()));
		ShowLightAttribute(light, nowAttributeIndex);
	}
	this->DamageMe();
}

#pragma endregion LightingWidget

//************************************************************************
//
// *
//========================================================================
void TrainWindow::
damageMe()
//========================================================================
{
	if (trainView == nullptr)
		return;

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