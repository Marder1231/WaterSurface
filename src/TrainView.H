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
	virtual void Use(glm::vec3 viewPos) override
	{
		shader->Use();

		shader->setVec3("viewPos", viewPos);

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
		glUniform1i(glGetAttribLocation(aShader->shader->ID, "heightMap"), 0);

		glBindVertexArray(aShader->Vaos->vao);
		//world transformation
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, Position);
		
		aShader->shader->setMat4("model", modelMatrix);
		
		aShader->GLDraw();
		timer++;
	}
};

//class HeightWaveShader
//{
//public:
//	Shader* shader = nullptr;
//	VAO* Vao = nullptr;
//
//	int Counter = 0;
//
//	Texture2D* HeightMaps[200] = { nullptr };
//
//	HeightWaveShader() { }
//
//	~HeightWaveShader()
//	{
//		delete shader;
//		delete Vao;
//		for (int i = 0; i < 200; i++)
//		{
//			delete HeightMaps[i];
//		}
//	}
//
//	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
//	{
//		shader = new Shader(vert, tesc, tese, geom, frag);
//	}
//
//	void SetVAO()
//	{
//		float sourceVertices[] = {
//			-1000.0f ,0.0f , -1000.0f,
//			-1000.0f ,0.0f , 1000.0f ,
//			1000.0f ,0.0f ,1000.0f ,
//			1000.0f ,0.0f ,-1000.0f };
//		GLuint sourceElement[] = {
//			0, 1, 2,
//			0, 2, 3};
//
//		int length = ceil(sqrt(1600));
//
//		std::vector<float> vertices;
//		std::vector<float> textureCoords;
//		std::vector<int> elements;
//
//		GLfloat VzInc = (sourceVertices[5] - sourceVertices[2]) / length;
//		GLfloat VzStart = sourceVertices[2];
//		GLfloat VzCurr = VzStart;
//		GLfloat TyInc = 1.0f / length;
//		GLfloat TyStart = 0.0f;
//		GLfloat TyCurr = TyStart;
//
//		for (int i = 0; i < length; i++)
//		{
//			GLfloat VxInc = (sourceVertices[6] - sourceVertices[0]) / length;
//			GLfloat VxStart = sourceVertices[0];
//			GLfloat VxCurr = VxStart;
//
//			GLfloat TxInc = 1.0f / length;
//			GLfloat TxStart = 0.0f;
//			GLfloat TxCurr = TxStart;
//
//			for (int j = 0; j < length; j++)
//			{
//				vertices.push_back(VxCurr);
//				vertices.push_back(sourceVertices[1]);
//				vertices.push_back(VzCurr);
//
//				vertices.push_back(VxCurr);
//				vertices.push_back(sourceVertices[4]);
//				vertices.push_back(VzCurr + VzInc);
//
//				vertices.push_back(VxCurr + VxInc);
//				vertices.push_back(sourceVertices[7]);
//				vertices.push_back(VzCurr + VzInc);
//
//				vertices.push_back(VxCurr + VxInc);
//				vertices.push_back(sourceVertices[10]);
//				vertices.push_back(VzCurr);
//
//				textureCoords.push_back(TxCurr);
//				textureCoords.push_back(TyCurr);
//
//				textureCoords.push_back(TxCurr);
//				textureCoords.push_back(TyCurr + TyInc);
//
//				textureCoords.push_back(TxCurr + TxInc);
//				textureCoords.push_back(TyCurr + TyInc);
//
//				textureCoords.push_back(TxCurr + TxInc);
//				textureCoords.push_back(TyCurr);
//
//				int idx = i * length + j;
//				elements.push_back(sourceElement[0] + (idx * 4));
//				elements.push_back(sourceElement[1] + (idx * 4));
//				elements.push_back(sourceElement[2] + (idx * 4));
//
//				elements.push_back(sourceElement[3] + (idx * 4));
//				elements.push_back(sourceElement[4] + (idx * 4));
//				elements.push_back(sourceElement[5] + (idx * 4));
//
//				VxCurr += VxInc;
//				TxCurr += TxInc;
//			}
//
//			VzCurr += VzInc;
//			TyCurr += TyInc;
//		}
//
//		this->Vao = new VAO;
//		this->Vao->element_amount = elements.size();
//
//		glGenVertexArrays(1, &this->Vao->vao);
//		glGenBuffers(2, this->Vao->vbo);
//		glGenBuffers(1, &this->Vao->ebo);
//
//		glBindVertexArray(this->Vao->vao);
//
//		// Position attribute
//		glBindBuffer(GL_ARRAY_BUFFER, this->Vao->vbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//		glEnableVertexAttribArray(0);
//
//		// Texture Coordinate attribute
//		glBindBuffer(GL_ARRAY_BUFFER, this->Vao->vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
//		glEnableVertexAttribArray(1);
//
//		//Element attribute
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Vao->ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLint), elements.data(), GL_STATIC_DRAW);
//
//	}
//
//	void Use(glm::vec3 viewPos,
//		/*environment attribute*/ LightingShader::DirLight dirL, LightingShader::PointLight pointL)
//	{
//		shader->Use();
//		shader->setVec3("material.diffuse", .3, .5, 1);
//		shader->setVec3("material.specular", .8, .8, 1);
//		shader->setVec3("viewPos", viewPos);
//		shader->setFloat("material.shininess", 32.0f);
//		//directional light
//		shader->setVec3("dirLight.direction", dirL.direction);
//		shader->setVec3("dirLight.ambient", dirL.ambient);
//		shader->setVec3("dirLight.diffuse", dirL.diffuse);
//		shader->setVec3("dirLight.specular", dirL.specular);
//
//		// point light 1
//		shader->setVec3("pointLights.position", pointL.position);
//		shader->setVec3("pointLights.ambient", pointL.ambient);
//		shader->setVec3("pointLights.diffuse", pointL.diffuse);
//		shader->setVec3("pointLights.specular", pointL.specular);
//		shader->setFloat("pointLights.constant", pointL.constant);
//		shader->setFloat("pointLights.linear", pointL.linear);
//		shader->setFloat("pointLights.quadratic", pointL.quadratic);
//
//		glm::mat4 projection;
//		glm::mat4 view;
//		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
//		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
//		shader->setMat4("projection", projection);
//		shader->setMat4("view", view);
//
//		HeightMaps[Counter % 200]->bind(1);
//	}
//
//	void Draw(glm::vec3 pos)
//	{
//		glUniform1i(glGetUniformLocation(this->shader->ID, "heightMap"), 1);
//		glBindVertexArray(this->Vao->vao);
//		// world transformation
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, pos);
//		model = glm::scale(model, glm::vec3( .1f, .1f, .1f));
//		shader->setMat4("model", model);
//
//		glDrawElements(GL_TRIANGLES, this->Vao->element_amount, GL_UNSIGNED_INT, 0);
//		Counter++;
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