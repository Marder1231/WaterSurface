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
	if (tw->worldCam->value())
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

		//if (boxShader.shader == nullptr)
		//{
		//	boxShader.SetShader("../src/shaders/colors.vert"
		//		, nullptr, nullptr, nullptr
		//		, "../src/shaders/color.frag");
		//	boxShader.SetVAO();
		//	boxShader.SetTexture("../Images/container2.png", "../Images/specular.png");
		//	boxShader.Init();
		//}

		//if (waveShader.shader == nullptr)
		//{
		//	waveShader.SetShader("../src/shaders/wave.vert"
		//		, nullptr, nullptr, nullptr
		//		, "../src/shaders/wave.frag");
		//	waveShader.SetVAO();
		//	waveShader.SetTexture("../Images/Wave.png");
		//	waveShader.Init();
		//}

		if (lightCubeShader.shader == nullptr)
		{
			lightCubeShader.SetShader("../src/shaders/lightCbue.vert"
				, nullptr, nullptr, nullptr
				, "../src/shaders/lightCube.frag");
			lightCubeShader.SetVAO();
			lightCubeShader.Init();
		}

		if (heightMapShader.shader == nullptr)
		{
			heightMapShader.SetShader("../src/shaders/HeightWave.vert",
				nullptr, nullptr, nullptr,
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
				heightMapShader.HeightMaps[i] = new Texture2D((path + pickPicture + png).c_str());
			}
		}

		//if (modelShader.shader == nullptr)
		//{
		//	modelShader.SetShader("../src/shaders/model.vert", nullptr, nullptr, nullptr, "../src/shaders/model.frag");
		//	modelShader.SetModel("../Model/nanosuit/nanosuit.obj");
		//}
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

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
	
	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]			= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	//glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	//glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	// set linstener position 
	if(selectedCube >= 0)
		alListener3f(AL_POSITION, 
			m_pTrack->points[selectedCube].pos.x,
			m_pTrack->points[selectedCube].pos.y,
			m_pTrack->points[selectedCube].pos.z);
	else
		alListener3f(AL_POSITION, 
			this->source_pos.x, 
			this->source_pos.y,
			this->source_pos.z);


	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	//drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  1, 0)
	};

	LightingShader::DirLight dirLight;
	dirLight.direction = glm::vec3(-.2f, -1.0f, -0.3f);
	dirLight.ambient = glm::vec3(.05f, .05f, .05f);
	dirLight.diffuse = glm::vec3(.4f, .4f, .4f);
	dirLight.specular = glm::vec3(.5f, .5f, .5f);

	LightingShader::PointLight pointLights[4];
	pointLights[0].position = pointLightPositions[0];
	pointLights[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	pointLights[0].diffuse = glm::vec3(.8f, .8f, .8f);
	pointLights[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLights[0].constant = 1.0f;
	pointLights[0].linear = 0.09;
	pointLights[0].quadratic = 0.032;

	pointLights[1].position = pointLightPositions[1];
	pointLights[1].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	pointLights[1].diffuse = glm::vec3(.8f, .8f, .8f);
	pointLights[1].specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLights[1].constant = 1.0f;
	pointLights[1].linear = 0.09;
	pointLights[1].quadratic = 0.032;

	pointLights[2].position = pointLightPositions[2];
	pointLights[2].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	pointLights[2].diffuse = glm::vec3(.3f, .3f, .3f);
	pointLights[2].specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLights[2].constant = 1.0f;
	pointLights[2].linear = 0.09;
	pointLights[2].quadratic = 0.032;

	glm::vec3 spotLightPos(1.2f, 1.0f, 2.0f);
	LightingShader::SpotLight spotLight;
	spotLight.position = spotLightPos;
	spotLight.direction = glm::vec3(glm::normalize(pointLightPositions[0] - spotLightPos));
	spotLight.ambient = glm::vec3(.0f, .0f, .0f);
	spotLight.diffuse = glm::vec3(.0f, .8f, .0f);
	spotLight.specular = glm::vec3(.9f, .0f, .0f);
	spotLight.constant = 1.0f;
	spotLight.linear = 0.09;
	spotLight.quadratic = 0.032;
	spotLight.cutOff = 12.5f;
	spotLight.outerCutOff = 15.0f;

	glm::vec3 viewPos = glm::vec3(arcball.eyeX, arcball.eyeY, arcball.eyeZ);
	
	
	//testShader.Draw(timer);

	heightMapShader.Use(viewPos, dirLight, pointLights[2]);
	heightMapShader.Draw(glm::vec3(0, 0, 0));
	//waveShader.Use(viewPos, dirLight, pointLights[2], tw->speed->value());

	//for (int x = -20; x <= 20; x++)
	//{
	//	for (int z = -20; z <= 20; z++)
	//	{
	//		waveShader.Draw(glm::vec3(x, 1, z), this->timer);
	//	}
	//}

	//boxShader.Use(viewPos, dirLight, pointLights, spotLight);

	//for (size_t i = 0; i < 10; i++)
	//{
	//	boxShader.Draw(cubePositions[i], 20.0f * i, glm::vec3(1.0f, 0.3f, 0.5f));
	//}

	lightCubeShader.Use(viewPos);
	lightCubeShader.Draw(pointLightPositions[2]);
	//for (int i = 0; i < 3; i++)
	//{
	//	lightCubeShader.Draw(pointLightPositions[i]);
	//}
	//lightCubeShader.Draw(spotLightPos);
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
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
	}
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
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	//if (!tw->trainCam->value()) {
	//	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
	//		if (!doingShadows) {
	//			if ( ((int) i) != selectedCube)
	//				glColor3ub(240, 60, 60);
	//			else
	//				glColor3ub(240, 240, 30);
	//		}
	//		m_pTrack->points[i].draw();
	//	}
	//}

	

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
