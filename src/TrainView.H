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
#include "../src/GameObject/GameObject.h"

#define HeightWaveGridSize 64

//class BoxShader
//{
//public:
//	Texture2D* DiffuseMap = nullptr;
//	Texture2D* SpecularMap = nullptr;
//	
//	Shader* shader = nullptr;
//
//	VAO Vao;
//
//	BoxShader()
//	{
//
//	}
//
//	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
//	{
//		shader = new Shader(vert, tesc, tese, geom, frag);
//	}
//
//	void SetVAO()
//	{
//		float vertices[] = {
//			// positions          // normals           // texture coords
//			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
//			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
//			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//
//			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
//			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
//			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//
//			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
//			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
//			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//
//			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
//			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
//			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
//			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
//		};
//
//		glGenVertexArrays(1, &this->Vao.vao);
//		glGenBuffers(1, this->Vao.vbo);
//
//		glBindVertexArray(this->Vao.vao);
//		glBindBuffer(GL_ARRAY_BUFFER, this->Vao.vbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//		glEnableVertexAttribArray(2);
//	}
//
//	void SetTexture(const GLchar* diffuse, const GLchar* specular)
//	{
//		if (DiffuseMap != nullptr)
//		{
//			delete DiffuseMap;
//			DiffuseMap = nullptr;
//		}
//		if (SpecularMap != nullptr)
//		{
//			delete SpecularMap;
//			SpecularMap = nullptr;
//		}
//
//		DiffuseMap = new Texture2D(diffuse);
//		SpecularMap = new Texture2D(specular);
//	}
//
//	/// <summary>
//	/// setting material which is never change
//	/// </summary>
//	void Init()
//	{
//		shader->setInt("material.diffuse", 0);
//		shader->setInt("material.specular", 1);
//	}
//
//	void Use(glm::vec3 viewPos)
//	{
//		shader->Use();
//		shader->setVec3("viewPos", viewPos);
//		shader->setFloat("material.shininess", 32.0f);
//
//		Environment::GetInstance()->lights.SetShader(this->shader);
//
//		glm::mat4 projection;
//		glm::mat4 view;
//		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
//		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
//		shader->setMat4("projection", projection);
//		shader->setMat4("view", view);
//
//		DiffuseMap->bind(0);
//		SpecularMap->bind(1);
//	}
//
//	void Draw(/*object attribute*/glm::vec3 objectPosition, GLfloat angle, glm::vec3 rotateAxis)
//	{
//		glBindVertexArray(this->Vao.vao);
//		// world transformation
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, objectPosition);
//		model = glm::rotate(model, glm::radians(angle), rotateAxis);
//		shader->setMat4("model", model);
//
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//	}
//};

class HeightMapShader : public ObjectShader
{
public:
	virtual void InitShaderAttribute() override
	{
		shader->setVec3("material.diffuse", .24f, .6f, 1);
		shader->setVec3("material.specular", .3f, .3f, 1);
		shader->setFloat("material.shininess", 32.0f);
	}

	virtual void Use(glm::vec3 viewPos) override
	{
		shader->Use();

		shader->setVec3("u_cameraPos", viewPos);

		//加入光效
		Environment::GetInstance()->lights.SetShader(shader);

		//轉到螢幕座標
		SetModelView7ProjectionMatrix();
	}
};
class HeightMapWave
{
public:
	glm::vec3 Position = glm::vec3(0, 0, 0);

	float WaveHeights[HeightWaveGridSize][HeightWaveGridSize];
	int timer = 0;

	/// <summary>
	/// have to use shader first
	/// </summary>
	/// <param name="aShader"></param>
	virtual void Draw(ObjectShader* aShader)
	{
		aShader->textures[timer % 200]->bind(0);
		glUniform1i(glGetAttribLocation(aShader->shader->ID, "u_heightMap"), 0);

		glBindVertexArray(aShader->Vaos->vao);
		//world transformation
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, Position);
		
		aShader->shader->setMat4("u_model", modelMatrix);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		aShader->GLDraw();
		timer++;
	}
};

//class RayCastShader : public ObjectShader
//{
//public:
//	RayCastShader() { }
//
//	virtual void SetVao() override
//	{
//		if (this->Vaos == nullptr)
//			Vaos = new VAO;
//
//		float size = 1;
//
//		float sourceVertices[] = {
//			0 ,0.0f , 0,
//			size ,0.0f ,size  };
//		GLuint sourceElement[] = {
//			0, 1, 1, };
//
//		
//		this->Vaos->element_amount = 3;
//
//		glGenVertexArrays(1, &this->Vaos->vao);
//		glGenBuffers(1, this->Vaos->vbo);
//		glGenBuffers(1, &this->Vaos->ebo);
//
//		glBindVertexArray(this->Vaos->vao);
//
//		// Position attribute
//		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(sourceVertices), sourceVertices, GL_DYNAMIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//		glEnableVertexAttribArray(0);
//
//		//Element attribute
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Vaos->ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sourceElement), sourceElement, GL_STATIC_DRAW);
//	}
//
//	void SetColor(glm::vec3 color)
//	{
//		shader->setVec3("u_color", color);
//	}
//
//	virtual void Use(glm::vec3 _) override
//	{
//		shader->Use();
//
//		glm::mat4 projection;
//		glm::mat4 view;
//		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
//		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
//		shader->setMat4("u_projection", projection);
//		shader->setMat4("u_view", view);
//
//		glBindVertexArray(this->Vaos->vao);
//	}
//
//	void Draw(/*object attribute*/glm::vec3 objectPosition, glm::vec3 dir)
//	{
//		float size = 1;
//		float sourceVertices[] = {
//			0 ,0.0f , 0,
//			size ,0.0f ,size };
//
//		sourceVertices[3] = objectPosition.x + dir.x;
//		sourceVertices[4] = objectPosition.y + dir.y;
//		sourceVertices[5] = objectPosition.z + dir.z;
//
//		glBindVertexArray(this->Vaos->vao);
//		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
//
//		glBufferData(GL_ARRAY_BUFFER, sizeof(sourceVertices), sourceVertices, GL_DYNAMIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//		glEnableVertexAttribArray(0);
//
//		// world transformation
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, objectPosition);
//		model = glm::scale(model, glm::vec3(10, 10, 10));
//		shader->setMat4("u_model", model);
//
//		GLDraw();
//	}
//};

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

	void SetColor(glm::vec3 color)
	{
		shader->setVec3("u_color", color);
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


		LightCubeShader lightCubeShader;

		ModelShader modelShader;

		//WaveShader waveShader;

		HeightMapShader heightMapShader;
		HeightMapWave HeightWave;

		float lightid;
};