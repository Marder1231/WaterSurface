/************************************************************************
     File:        TrainView.H

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

#pragma once

#include <glad/glad.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"


// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;


//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#pragma warning(pop)

#include <AL/alut.h>
//#include <AL/al.h>
//#include <AL/alc.h>

// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"

#include "../build/Model.h"

class BoxShader
{
public:
	Texture2D* DiffuseMap = nullptr;
	Texture2D* SpecularMap = nullptr;
	
	struct DirLight
	{
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct PointLight
	{
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct SpotLight 
	{
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	Shader* shader = nullptr;

	VAO Vao;

	BoxShader()
	{

	}

	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
	{
		shader = new Shader(vert, tesc, tese, geom, frag);
	}

	void SetVAO()
	{
		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		glGenVertexArrays(1, &this->Vao.vao);
		glGenBuffers(1, this->Vao.vbo);

		glBindVertexArray(this->Vao.vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vao.vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void SetTexture(const GLchar* diffuse, const GLchar* specular)
	{
		if (DiffuseMap != nullptr)
			delete DiffuseMap;
		if (SpecularMap != nullptr)
			delete SpecularMap;

		DiffuseMap = new Texture2D(diffuse);
		SpecularMap = new Texture2D(specular);
	}

	/// <summary>
	/// setting material which is never change
	/// </summary>
	void Init()
	{
		shader->setInt("material.diffuse", 0);
		shader->setInt("material.specular", 1);
	}

	void Use(glm::vec3 viewPos, 
	/*environment attribute*/ DirLight dirL, PointLight pointL[4], SpotLight spotL)
	{
		shader->Use();
		shader->setVec3("viewPos", viewPos);
		shader->setFloat("material.shininess", 32.0f);

		//directional light
		shader->setVec3("dirLight.direction", dirL.direction);
		shader->setVec3("dirLight.ambient", dirL.ambient);
		shader->setVec3("dirLight.diffuse", dirL.diffuse);
		shader->setVec3("dirLight.specular", dirL.specular);

		// point light 1
		shader->setVec3("pointLights[0].position",	 pointL[0].position);
		shader->setVec3("pointLights[0].ambient",	 pointL[0].ambient);
		shader->setVec3("pointLights[0].diffuse",	 pointL[0].diffuse);
		shader->setVec3("pointLights[0].specular",	 pointL[0].specular);
		shader->setFloat("pointLights[0].constant",  pointL[0].constant);
		shader->setFloat("pointLights[0].linear",	 pointL[0].linear);
		shader->setFloat("pointLights[0].quadratic", pointL[0].quadratic);

		// point light 2
		shader->setVec3 ("pointLights[1].position",  pointL[1].position);
		shader->setVec3 ("pointLights[1].ambient",   pointL[1].ambient);
		shader->setVec3 ("pointLights[1].diffuse",   pointL[1].diffuse);
		shader->setVec3 ("pointLights[1].specular",  pointL[1].specular);
		shader->setFloat("pointLights[1].constant",  pointL[1].constant);
		shader->setFloat("pointLights[1].linear",	 pointL[1].linear);
		shader->setFloat("pointLights[1].quadratic", pointL[1].quadratic);

		// point light 3
		shader->setVec3 ("pointLights[2].position",  pointL[2].position);
		shader->setVec3 ("pointLights[2].ambient",   pointL[2].ambient);
		shader->setVec3 ("pointLights[2].diffuse",   pointL[2].diffuse);
		shader->setVec3 ("pointLights[2].specular",  pointL[2].specular);
		shader->setFloat("pointLights[2].constant",  pointL[2].constant);
		shader->setFloat("pointLights[2].linear",	 pointL[2].linear);
		shader->setFloat("pointLights[2].quadratic", pointL[2].quadratic);

		// point light 3
		shader->setVec3 ("pointLights[3].position",  pointL[3].position);
		shader->setVec3 ("pointLights[3].ambient",	 pointL[3].ambient);
		shader->setVec3 ("pointLights[3].diffuse",	 pointL[3].diffuse);
		shader->setVec3 ("pointLights[3].specular",  pointL[3].specular);
		shader->setFloat("pointLights[3].constant",  pointL[3].constant);
		shader->setFloat("pointLights[3].linear",	 pointL[3].linear);
		shader->setFloat("pointLights[3].quadratic", pointL[3].quadratic);

		// spotLight
		shader->setVec3("spotLight.position", spotL.position);
		shader->setVec3("spotLight.direction", spotL.direction);
		shader->setVec3("spotLight.ambient", spotL.ambient);
		shader->setVec3("spotLight.diffuse", spotL.diffuse);
		shader->setVec3("spotLight.specular", spotL.specular);
		shader->setFloat("spotLight.constant", spotL.constant);
		shader->setFloat("spotLight.linear", spotL.linear);
		shader->setFloat("spotLight.quadratic", spotL.quadratic);
		shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(spotL.cutOff)));
		shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotL.outerCutOff)));

		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);



		DiffuseMap->bind(0);
		SpecularMap->bind(1);

		glBindVertexArray(this->Vao.vao);
	}

	void Draw(/*object attribute*/glm::vec3 objectPosition, GLfloat angle, glm::vec3 rotateAxis)
	{
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, objectPosition);
		model = glm::rotate(model, glm::radians(angle), rotateAxis);
		shader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};

class LightCubeShader
{
public:
	Shader* shader = nullptr;

	VAO Vao;

	LightCubeShader() { }

	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
	{
		shader = new Shader(vert, tesc, tese, geom, frag);
	}

	void SetVAO()
	{
		float vertices[] = {
			// positions          // normals          
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 
		};

		glGenVertexArrays(1, &this->Vao.vao);
		glGenBuffers(1, this->Vao.vbo);

		glBindVertexArray(this->Vao.vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vao.vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);					
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void Init()
	{
		shader->setInt("material.diffuse", 0);
		shader->setInt("material.specular", 1);
	}

	void Use(glm::vec3 viewPos)
	{
		shader->Use();
		shader->setVec3("viewPos", viewPos);
		shader->setFloat("material.shininess", 32.0f);

		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		glBindVertexArray(this->Vao.vao);
	}

	void Draw(/*object attribute*/glm::vec3 objectPosition)
	{
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, objectPosition);
		shader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};

class ModelShader
{
public:
	Shader* shader = nullptr;
	Model* model = nullptr;

	VAO Vao;

	ModelShader() { }

	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
	{
		shader = new Shader(vert, tesc, tese, geom, frag);
	}

	void SetModel(const GLchar* path)
	{
		model = new Model(path);
	}

	void Draw(glm::vec3 viewPos)
	{
		shader->Use();
		shader->setVec3("viewPos", viewPos);
		shader->setFloat("material.shininess", 32.0f);

		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		// render the loaded model
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader->setMat4("model", modelMat);
		model->Draw(*shader);
	}
};

class TrainView : public Fl_Gl_Window
{
	public:
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();

		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();

		//set ubo
		void setUBO();

		int LoadTexture(const char* path);
	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube;  // simple - just remember which cube is selected

		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene

		//Shader* shader		= nullptr;
		//Texture2D* texture	= nullptr;
		//VAO* plane			= nullptr;
		//UBO* commom_matrices= nullptr;

		//OpenAL
		glm::vec3 source_pos;
		glm::vec3 listener_pos;
		ALCdevice* device	= nullptr;
		ALCcontext* context = nullptr;
		ALuint source;
		ALuint buffer;

		BoxShader boxShader;
		LightCubeShader lightCubeShader;

		ModelShader modelShader;
};