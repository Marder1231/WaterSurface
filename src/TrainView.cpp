/************************************************************************
     File:        TrainView.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/


#include "Utilities/3DUtils.H"
#include <iostream>
#include <Fl/fl.h>

//#include "GL/gl.h"

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>

#include "TrainView.H"
#include "TrainWindow.H"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<HeightMapWave::RainRipple> HeightMapWave::RainRipples;
void HeightMapWave::GenerateRipple(glm::vec3 _where, float _time)
{
	RainRipple rainRipple(_where, _time);
	RainRipples.push_back(rainRipple);
}

//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view

	if (arcball.handle(event)) 
		return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{
	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...

#pragma region ShaderInit
		if (skyBoxShader.shader == nullptr)
		{
			skyBoxShader.SetShader("../src/shaders/SkyBox.vert"
				, nullptr, nullptr, nullptr
				, "../src/shaders/SkyBox.frag");
			skyBoxShader.SetVAO();
			skyBoxShader.LoadCubemap();
		}

		if (heightMapShader.shader == nullptr)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			// positions of the point lights
			glm::vec3 pointLightPositions[] = {
				glm::vec3(2.3f, -3.3f, -4.0f),
				glm::vec3(-4.0f,  2.0f, -12.0f),
				glm::vec3(0.0f,  1, 0)
			};

			Environment* instance = Environment::GetInstance();

			Lighting::DirLight* dirLight = new Lighting::DirLight;
			dirLight->SetDirection(glm::vec3(-.2f, -1.0f, -0.3f));
			dirLight->SetAmbient(glm::vec3(.1f, .1f, .1f));
			dirLight->SetDiffuse(glm::vec3(.4f, .4f, .4f));
			dirLight->SetSpecular(glm::vec3(.5f, .5f, .5f));
			tw->lightingWidget->AddLightBrowser(dirLight);

			Lighting::PointLight* pointLights[3];
			pointLights[0] = new Lighting::PointLight();
			pointLights[0]->SetPosition(pointLightPositions[0]);
			pointLights[0]->SetAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
			pointLights[0]->SetDiffuse(glm::vec3(.8f, .8f, .8f));
			pointLights[0]->SetSpecular(glm::vec3(1.0f, 1.0f, 1.0f));
			pointLights[0]->SetConstant(1.0f);
			pointLights[0]->SetLinear(0.09);
			pointLights[0]->SetQuadratic(0.032);

			pointLights[1] = new Lighting::PointLight();
			pointLights[1]->SetPosition(pointLightPositions[1]);
			pointLights[1]->SetAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
			pointLights[1]->SetDiffuse(glm::vec3(.8f, .8f, .8f));
			pointLights[1]->SetSpecular(glm::vec3(1.0f, 1.0f, 1.0f));
			pointLights[1]->SetConstant(1.0f);
			pointLights[1]->SetLinear(0.09);
			pointLights[1]->SetQuadratic(0.032);

			pointLights[2] = new Lighting::PointLight();
			pointLights[2]->SetPosition(pointLightPositions[2]);
			pointLights[2]->SetAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
			pointLights[2]->SetDiffuse(glm::vec3(.4f, .4f, .4f));
			pointLights[2]->SetSpecular(glm::vec3(1.0f, 1.0f, 1.0f));
			pointLights[2]->SetConstant(1.0f);
			pointLights[2]->SetLinear(0.09);
			pointLights[2]->SetQuadratic(0.032);

			tw->lightingWidget->AddLightBrowser(pointLights[0]);
			tw->lightingWidget->AddLightBrowser(pointLights[1]);
			tw->lightingWidget->AddLightBrowser(pointLights[2]);

			glm::vec3 spotLightPos(1.2f, 1.0f, 2.0f);
			Lighting::SpotLight* spotLight = new Lighting::SpotLight();
			spotLight->SetPosition(spotLightPos);
			spotLight->SetDirection(glm::vec3(glm::normalize(pointLightPositions[0] - spotLightPos)));
			spotLight->SetAmbient(glm::vec3(.0f, .0f, .0f));
			spotLight->SetDiffuse(glm::vec3(.0f, .8f, .0f));
			spotLight->SetSpecular(glm::vec3(.9f, .0f, .0f));
			spotLight->SetConstant(1.0f);
			spotLight->SetLinear(0.09);
			spotLight->SetQuadratic(0.032);
			spotLight->SetCutOff(12.5f);
			spotLight->SetOuterCutOff(15.0f);
			tw->lightingWidget->AddLightBrowser(spotLight);

			heightMapShader.skyboxID = skyBoxShader.cubemapTexture;
			heightMapShader.SetShader("../src/shaders/HeightWave.vert",
				"../src/shaders/HeightWave.tesc", "../src/shaders/HeightWave.tese", nullptr,
				"../src/shaders/HeightWave.frag");
			heightMapShader.SetVAO();
			for (int i = 0; i < 200; i++)
			{
				std::string path = "../Images/waves5/";
				std::string pickPicture = "";
				if (i < 10)
					pickPicture = "00";
				else if (i < 100)
					pickPicture = "0";
				pickPicture += std::to_string(i);
				std::string png = ".png";
				heightMapShader.AddTexture((path + pickPicture + png).c_str());
			}

			heightMapShader.SetFBO();

			HeightWave.rainParticleShader.SetShader("../src/shaders/RainParticles.vert",
				nullptr, nullptr, "../src/shaders/RainParticles.geom",
				"../src/shaders/RainParticles.frag");
			HeightWave.RainParticles.GenerateRain(100, HeightWave.timer);
			HeightWave.RainParticles.Attribute2VAO(&HeightWave.rainParticleShader);
		}

		if (lightCubeShader.shader == nullptr)
		{
			lightCubeShader.SetShader("../src/shaders/lightCbue.vert"
				, nullptr, nullptr, nullptr
				, "../src/shaders/lightCube.frag");
			lightCubeShader.SetVAO();
		}

		if (modelShader.shader == nullptr)
		{
			modelShader.SetShader("../src/shaders/model.vert", nullptr, nullptr, nullptr, "../src/shaders/model.frag");
			modelShader.SetModel("../Model/nanosuit/nanosuit.obj");
		}

		if (cubeShader.shader == nullptr)
		{
			cubeShader.SetShader("../src/shaders/colors.vert", nullptr, nullptr, nullptr, "../src/shaders/color.frag");
			cubeShader.SetVAO();
			cubeShader.AddTexture("../Images/container2.png");
			cubeShader.AddTexture("../Images/specular.png");
		}

		if (mapRoadShader.shader == nullptr)
		{
			mapRoadShader.SetShader("../src/shaders/MapRoad.vert",
				nullptr, nullptr, nullptr,
				"../src/shaders/MapRoad.frag");
			ComputeDistance();
		}
#pragma endregion 

	}
	else
		throw std::runtime_error("Could not initialize GLAD!");


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();

	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	setupObjects();

	glClearColor(.1f, .1f, .1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	const glm::vec3 viewPos = this->arcball.GetEyePos();
	glm::mat4 modelView;
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelView[0][0]);

	skyBoxShader.Use(viewPos);
	glm::mat4 removeTranslateViewPos = glm::mat4(glm::mat3(modelView)); // remove translation from the view matrix
	skyBox.Draw(&skyBoxShader, removeTranslateViewPos);
	skyBoxShader.Unuse();

	//modelShader.Draw(viewPos);

	cubeShader.Use(viewPos);
	cube.Draw(&cubeShader);
	cubeShader.Unuse();

	std::vector<glm::mat4> waveViewMats
	{
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0),  glm::vec3(0, 1, 0)),
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),  glm::vec3(0, 0, 1)),
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1),  glm::vec3(0, 1, 0)),
		glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0))
	};

	float aspect = w() / h();
	glm::mat4 waveProjectionMat = glm::perspective(90.0f, aspect, .1f, 1000.0f);

	//heightMapShader.BindFBO();

	//for (int i = 0; i < 6; i++)
	//{
	//	/*draw skybox*/
	//	{
	//		skyBoxShader.shader->Use();

	//		skyBoxShader.shader->setMat4("u_projection", waveProjectionMat);
	//		glm::mat4 _translateView = glm::mat4(glm::mat3(waveViewMats[i]));
	//		skyBoxShader.shader->setMat4("u_view", _translateView);

	//		glBindVertexArray(skyBoxShader.Vaos->vao);
	//		glActiveTexture(GL_TEXTURE0 + 0);
	//		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxShader.cubemapTexture);
	//		skyBoxShader.shader->setInt("skybox", 0);

	//		skyBoxShader.GLDraw();
	//		skyBoxShader.Unuse();
	//	}

	//	cubeShader.shader->Use();
	//	cubeShader.shader->setVec3("viewPos", viewPos);
	//	cubeShader.shader->setFloat("material.shininess", 32.0f);

	//	Environment::GetInstance()->lights.SetShader(cubeShader.shader);

	//	cubeShader.shader->setMat4("projection", waveProjectionMat);
	//	cubeShader.shader->setMat4("view", waveViewMats[i]);
	//	cubeShader.BindTextures();
	//	glBindVertexArray(cubeShader.Vaos->vao);

	//	glm::mat4 model = glm::mat4(1.0f);
	//	model = glm::translate(model, cube.Position);
	//	cubeShader.shader->setMat4("model", model);
	//	cubeShader.GLDraw();
	//	cubeShader.Unuse();

	//	//modelShader.shader->Use();
	//	//modelShader.shader->setVec3("viewPos", viewPos);
	//	//modelShader.shader->setFloat("material.shininess", 32.0f);
	//	//modelShader.shader->setMat4("projection", waveProjectionMat);
	//	//modelShader.shader->setMat4("view", waveViewMats[i]);
	//	//glm::mat4 modelMat = glm::mat4(1.0f);
	//	//modelMat = glm::translate(modelMat, glm::vec3(0, 10, 0)); // translate it down so it's at the center of the scene
	//	//modelShader.shader->setMat4("model", modelMat);
	//	//modelShader.model->Draw(*modelShader.shader);

	//}
	//heightMapShader.UnBindFBO();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	heightMapShader.Use(viewPos);
	HeightWave.Draw(&heightMapShader, skyBoxShader.cubemapTexture, viewPos);
	heightMapShader.Unuse();

	glDisable(GL_CULL_FACE);

	glStencilMask(0x00);

	//std::cout << "view pos x : " << viewPos.x << ", y : " << viewPos.y << ", z : " << viewPos.z << std::endl;
	//boxShader.Use(viewPos);
	//testCube.Draw(&boxShader);
	
	//cubeShader.Use(viewPos);
	//cube.Draw(&cubeShader);

	/*draw light*/ 
	if(tw->ChoiceWhichGameObjectAttribute->value() == int(TrainWindow::EmObjectChoice::Lighting))
	{
		lightCubeShader.Use(viewPos);
		Environment* instance = Environment::GetInstance();
		Lighting::BaseLight* nowChoiceLight = nullptr;
		for (auto& light : instance->lights.Lightings)
		{
			//record now pick light
			if (tw->lightingWidget->nowChooseLightIndex != -1 && light.second->ID == tw->lightingWidget->LightListIDs[tw->lightingWidget->nowChooseLightIndex])
			{
				//¼g¤J¼ÒªO
				glStencilFunc(GL_ALWAYS, 1, 0xff);
				glStencilMask(0xff);
				nowChoiceLight = light.second;
			}
			else
			{
				glStencilMask(0x00);
			}

			if (light.second->Type == Lighting::EmLightType::Point)
			{
				Lighting::PointLight* pointLight = static_cast<Lighting::PointLight*>(light.second);
				lightCubeShader.SetColor(glm::vec3(1, 1, 1));
				lightCubeShader.Draw(pointLight->GetPosition());
			}
			else if (light.second->Type == Lighting::EmLightType::Spot)
			{
				Lighting::SpotLight* spotLight = static_cast<Lighting::SpotLight*>(light.second);
				lightCubeShader.SetColor(glm::vec3(1, 1, 1));
				lightCubeShader.Draw(spotLight->GetPosition());

				glm::vec3 rayDirection = spotLight->GetDirection();

				//draw raycast
				glPushMatrix();
				glBegin(GL_LINES);
				if (tw->lightingWidget->nowChooseLightIndex != -1 && spotLight->ID == tw->lightingWidget->LightListIDs[tw->lightingWidget->nowChooseLightIndex])
					glColor3f(1, 1, 0);
				else
					glColor3f(0, 1, 1);
				glVertex3f(0, 0, 0);
				glVertex3f(6.24 * rayDirection.x, 6.24 * rayDirection.y, 6.24 * rayDirection.z);
				glEnd();
				glPopMatrix();
			}
		}

		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		lightCubeShader.Use(viewPos);
		glm::vec3 scale = glm::vec3(1.1f, 1.1f, 1.1f);
		lightCubeShader.SetColor(glm::vec3(1, 0, 0));
		if (nowChoiceLight != nullptr)
		{
			if (nowChoiceLight->Type == Lighting::EmLightType::Point)
			{
				Lighting::PointLight* _light = static_cast<Lighting::PointLight*>(nowChoiceLight);
				lightCubeShader.Draw(_light->GetPosition(), scale);
			}
			else if (nowChoiceLight->Type == Lighting::EmLightType::Spot)
			{
				Lighting::SpotLight* _light = static_cast<Lighting::SpotLight*>(nowChoiceLight);
				lightCubeShader.Draw(_light->GetPosition(), scale);
			}
		}
		lightCubeShader.shader->Unuse();
		glBindVertexArray(0);
	};

	glDisable(GL_CULL_FACE);
	drawStuff(false);
}

void TrainView::GetPos(float const t, Pnt3f& pos, Pnt3f& orient)
{
	// pos
	ControlPoint cp1 = m_pTrack->points[int(t) % m_pTrack->points.size()];
	ControlPoint cp2 = m_pTrack->points[(int(t) + 1) % m_pTrack->points.size()];
	ControlPoint cp3 = m_pTrack->points[(int(t) + 2) % m_pTrack->points.size()];
	ControlPoint cp4 = m_pTrack->points[(int(t) + 3) % m_pTrack->points.size()];

	float interval = t - int(t);

	glm::mat4x3 matG =
	{
		cp1.pos.x, cp1.pos.y, cp1.pos.z,
		cp2.pos.x, cp2.pos.y, cp2.pos.z,
		cp3.pos.x, cp3.pos.y, cp3.pos.z,
		cp4.pos.x, cp4.pos.y, cp4.pos.z,
	};
	glm::mat4x3 matO =
	{
		cp1.orient.x, cp1.orient.y, cp1.orient.z,
		cp2.orient.x, cp2.orient.y, cp2.orient.z,
		cp3.orient.x, cp3.orient.y, cp3.orient.z,
		cp4.orient.x, cp4.orient.y, cp4.orient.z,
	};
	glm::mat4x4 matM =
	{
		-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
		3 / 6.0f, -6 / 6.0f, 3 / 6.0f, 0,
		-3 / 6.0f, 0, 3 / 6.0f, 0,
		1 / 6.0f, 4 / 6.0f, 1 / 6.0f, 0
	};
	glm::fvec4 vecT = { interval * interval * interval, interval * interval, interval, 1 };

	glm::fvec3 qt = matG * matM * vecT;
	glm::fvec3 ot = matO * matM * vecT;

	pos.x = qt.x;
	pos.y = qt.y;
	pos.z = qt.z;
	orient.x = ot.x;
	orient.y = ot.y;
	orient.z = ot.z;
}
//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	ComputeDistance();

	// Draw the control points
	// don't draw the control points if you're driving 
	//// (otherwise you get sea-sick as you drive through them)
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) 
	{
		if (!doingShadows) {
			if (((int)i) != selectedCube)
				glColor3ub(240, 60, 60);
			else
				glColor3ub(240, 240, 30);
		}
		m_pTrack->points[i].draw();
	}

	mapRoadShader.Use(glm::vec3());
	mapRoad.Draw(&mapRoadShader);
	mapRoadShader.Unuse();

	HeightWave.RainParticles.Attribute2VAO(&HeightWave.rainParticleShader);
	HeightWave.rainParticleShader.Use(glm::vec3());
	HeightWave.RainParticles.Draw(&HeightWave.rainParticleShader);
	HeightWave.rainParticleShader.Unuse();

	//lineShader.Use(glm::vec3());
	//lineShader.shader->setInt("u_ControlPointAmount", m_pTrack->points.size());
	//for (int i = 0; i < m_pTrack->points.size(); i++)
	//{
	//	lineShader.shader->setVec3("cp[" + std::to_string(i) + "].position" , glm::vec3(m_pTrack->points[i].pos.x, m_pTrack->points[i].pos.y, m_pTrack->points[i].pos.z));
	//	lineShader.shader->setVec3("cp[" + std::to_string(i) + "].orient", glm::vec3(m_pTrack->points[i].orient.x, m_pTrack->points[i].orient.y, m_pTrack->points[i].orient.z));
	//}

	//for (int i = 0; i < m_pTrack->points.size(); i++)
	//{
	//	lineShader.shader->setInt("u_nowControlPointIndex", i);
	//	line.Draw(&lineShader);
	//}
	//lineShader.Unuse();

	//Pnt3f cpS;
	//Pnt3f cpSo;

	//GetPos(0, cpS, cpSo);

	//for (int i = 0; i < m_pTrack->points.size(); i++)
	//{
	//	float dividLine = m_pTrack->PointDistances[i] / 2;
	//	float t = i;
	//	float percent = 1.0f / dividLine;

	//	for (int j = 0; j < dividLine; j++)
	//	{
	//		t += percent;
	//		float interval = t - int(t);

	//		Pnt3f cpN;
	//		Pnt3f cpNo;
	//		GetPos(t, cpN, cpNo);

	//		Pnt3f cross = Pnt3f(cpN.x - cpS.x, cpN.y - cpS.y, cpN.z - cpS.z) * cpNo;
	//		cross.normalize();
	//		cross = cross * 2.5f;

	//		Pnt3f u = Pnt3f(cpN.x - cpS.x, cpN.y - cpS.y, cpN.z - cpS.z); u.normalize();
	//		Pnt3f w = u * cpNo; w.normalize();
	//		Pnt3f v = w * u; v.normalize();

	//		float newVertices[] = {
	//			cpS.x + cross.x, cpS.y + cross.y, cpS.z + cross.z,
	//			cpN.x + cross.x, cpN.y + cross.y, cpN.z + cross.z,
	//			cpS.x - cross.x, cpS.y - cross.y, cpS.z - cross.z,
	//			cpN.x - cross.x, cpN.y - cross.y, cpN.z - cross.z
	//		};
	//		float sourceElement[] = {
	//			0, 1, 2,
	//			1, 2, 3
	//		};



	//		//float barWidth = 2;
	//		//float barHeight = 0.5f;
	//		//float barLength = 6;

	//		//float rotation[16] =
	//		//{
	//		//	u.x, u.y, u.z, 0.0f,
	//		//	v.x, v.y, v.z, 0.0f,
	//		//	w.x, w.y, w.z, 0.0f,
	//		//	0.0f, 0.0f, 0.0f, 1.0f,
	//		//};

	//		//glm::mat4 model = glm::mat4(1.0f);
	//		//model = glm::translate(model, glm::vec3(cpN.x, cpN.y, cpN.z));
	//		//glm::mat4 rotate = glm::make_mat4(rotation);
	//		//model = rotate * model;

	//		//glPushMatrix();
	//		//glTranslatef(cpN.x, cpN.y, cpN.z);
	//		//glMultMatrixf(rotation);
	//		//glRotated(90, 0, 1, 0);
	//		//glScaled(2.4f, 1, 2.4f);

	//		//glBegin(GL_QUADS);

	//		//glColor3ub(0, 0, 0);
	//		//glNormal3f(0, 1, 0);
	//		//glVertex3f(-barLength / 2, 0, -barWidth / 2);
	//		//glVertex3f(-barLength / 2, 0, barWidth / 2);
	//		//glVertex3f(barLength / 2, 0, barWidth / 2);
	//		//glVertex3f(barLength / 2, 0, -barWidth / 2);

	//		//glEnd();
	//		//glPopMatrix();

	//		cpS = cpN;
	//	}
	//}

	////$$$¹ê¨Ò¤Æ 
	//roadShader.Use(glm::vec3());
	//for (int i = 0; i < m_pTrack->points.size(); i++)
	//{
	//	for (int bFour = 0; bFour < 4; bFour++)
	//	{
	//		lineShader.shader->setVec3("cp[" + std::to_string(bFour) + "].position",
	//			glm::vec3(
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].pos.x,
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].pos.y,
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].pos.z)
	//		);

	//		lineShader.shader->setVec3("cp[" + std::to_string(bFour) + "].orient",
	//			glm::vec3(
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].orient.x,
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].orient.y,
	//				m_pTrack->points[(i + bFour) % m_pTrack->points.size()].orient.z)
	//		);
	//	}

	//	float t = i;
	//	float percent = 1.0f / roadShader.clipSize;
	//	for (int clip = 0; clip < roadShader.clipSize; clip++)
	//	{
	//		roadShader.shader->setFloat("u_time", t);
	//		road.Draw(&roadShader);
	//		t += percent;
	//	}
	//}
	//roadShader.Unuse();

	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}

void TrainView::setUBO()
{
	//float wdt = this->pixel_w();
	//float hgt = this->pixel_h();

	//glm::mat4 view_matrix;
	//glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	////HMatrix view_matrix; 
	////this->arcball.getMatrix(view_matrix);

	//glm::mat4 projection_matrix;
	//glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	////projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	//glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TrainView::Distance2T(float& _distance, int& _turnCounter, float& _trainU)
{
	if (_distance < 0)
		_distance += TotalDistance;
	if (_distance > TotalDistance)
		_distance -= TotalDistance;

	//at where
	float computeDistance = _distance;
	int nowControlPointIndex = 0;
	for (int i = 0; i < m_pTrack->points.size(); i++)
	{
		computeDistance -= m_pTrack->PointDistances[i];
		nowControlPointIndex = i;
		if (computeDistance <= 0)
			break;
	}
	_turnCounter = nowControlPointIndex;
	_trainU = (computeDistance + m_pTrack->PointDistances[nowControlPointIndex])
		/ m_pTrack->PointDistances[nowControlPointIndex];
}

void TrainView::Distance2Pos(float _distance, Pnt3f& _pos, Pnt3f& _orient)
{
	int _turnCounter = 0;
	float _trainU = 0;
	Distance2T(_distance, _turnCounter, _trainU);

	GetPos(_turnCounter + _trainU, _pos, _orient);
}

float TrainView::T2Distance(int& _turnCounter, float& _trianU)
{
	float _distance = 0;
	for (int i = 0; i < _turnCounter; i++)
	{
		_distance += m_pTrack->PointDistances[i];
	}

	int nextIndex = _turnCounter;
	if (nextIndex < 0)
		nextIndex = 0;
	_distance += m_pTrack->PointDistances[nextIndex] * _trianU;
	return _distance;
}

void TrainView::ComputeDistance()
{
	const int clipcounter = 24;
	TotalDistance = 0;
	m_pTrack->PointDistances.clear();
	//compute distance
	Pnt3f cps;
	Pnt3f cpso;
	GetPos(0, cps, cpso);

	float sourceElement[] = {
	0, 1, 2,
	1, 2, 3
	};
	std::vector<float> vertices;
	std::vector<int> elements;

	for (int i = 0; i < m_pTrack->points.size(); i++)
	{
		float t = i;
		float percent = 1.0f / clipcounter;

		float distancetotal = 0;

		for (int j = 0; j < clipcounter; j++)
		{
			t += percent;
			float interval = t - int(t);

			Pnt3f cpn;
			Pnt3f cpno;
			GetPos(t, cpn, cpno);

			Pnt3f diff = Pnt3f(cpn.x - cps.x, cpn.y - cps.y, cpn.z - cps.z);
			float distance = abs(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			distance = sqrt(distance);
			distancetotal += distance;
			TotalDistance += distance;

			Pnt3f cross = diff * cpno;
			cross.normalize();
			cross = cross * 12.26f;

			Pnt3f u = Pnt3f(cpn.x - cps.x, cpn.y - cps.y, cpn.z - cps.z); u.normalize();
			Pnt3f w = u * cpno; w.normalize();
			Pnt3f v = w * u; v.normalize();

			float newVertices[] = {
				cps.x + cross.x, cps.y + cross.y, cps.z + cross.z,
				cps.x - cross.x, cps.y - cross.y, cps.z - cross.z,
				cpn.x + cross.x, cpn.y + cross.y, cpn.z + cross.z,
				cpn.x - cross.x, cpn.y - cross.y, cpn.z - cross.z
			};
			int mult = vertices.size() / 3;
			if (sourceElement[0] + mult - 2 > 0)
			{
				elements.push_back(sourceElement[0] + mult);	elements.push_back(sourceElement[1] + mult - 2);	elements.push_back(sourceElement[0] + mult - 2);
				elements.push_back(sourceElement[0] + mult);	elements.push_back(sourceElement[1] + mult);	elements.push_back(sourceElement[1] + mult - 2);
			}

			elements.push_back(sourceElement[0] + mult);	elements.push_back(sourceElement[1] + mult);	elements.push_back(sourceElement[2] + mult);
			elements.push_back(sourceElement[3] + mult);	elements.push_back(sourceElement[4] + mult);	elements.push_back(sourceElement[5] + mult);

			vertices.push_back(newVertices[0]);  vertices.push_back(newVertices[1]);  vertices.push_back(newVertices[2]);
			vertices.push_back(newVertices[3]);  vertices.push_back(newVertices[4]);  vertices.push_back(newVertices[5]);
			vertices.push_back(newVertices[6]);  vertices.push_back(newVertices[7]);  vertices.push_back(newVertices[8]);
			vertices.push_back(newVertices[9]);  vertices.push_back(newVertices[10]);  vertices.push_back(newVertices[11]);

			cps = cpn;
		}
		m_pTrack->PointDistances.push_back(distancetotal);
	}

	int mult = vertices.size() / 3 - 4;
	elements.push_back(sourceElement[5] + mult);	elements.push_back(sourceElement[0]);	elements.push_back(sourceElement[1]);
	elements.push_back(sourceElement[4] + mult);	elements.push_back(sourceElement[5] + mult);	elements.push_back(sourceElement[0]);

	if (mapRoadShader.shader != nullptr)
	{
		mapRoadShader.SetVAO(vertices, elements);
	}
}

void TrainView::ARCGoGo(float)
{
	float _speed = 1;
	//for (int i = 0; i < TrainAmount; i++)
	//{
	//	Trains[i]->NowDistance += _speed;
	//	Distance2T(Trains[i]->NowDistance, m_pTrack->TurnCounter, m_pTrack->trainU);
	//}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	arcball.setProjection(false);
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
}