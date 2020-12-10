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
#include "Utilities/Pnt3f.H"

#include "../build/Model.h"
#include "../src/GameObject/GameObject.h"

#define HeightWaveGridSize 64

class SkyboxCubeShader : public ObjectShader
{
public:
	unsigned int skyboxID;

	virtual void InitShaderAttribute() override
	{
		shader->setInt("u_skybox", skyboxID);
	}

	virtual void SetVAO() override
	{
		if (this->Vaos == nullptr)
			this->Vaos = new VAO;

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

		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);

		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	virtual void Use(glm::vec3 viewPos) override
	{
		shader->Use();

		shader->setVec3("viewPos", viewPos);
		shader->setFloat("material.shininess", 32.0f);

		Environment::GetInstance()->lights.SetShader(this->shader);

		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};
class SkyboxTestCube
{
public:
	glm::vec3 Position = glm::vec3(0, 0, 0);

	void Draw(SkyboxCubeShader* shader)
	{
		glBindVertexArray(shader->Vaos->vao);

		glActiveTexture(GL_TEXTURE0 + shader->skyboxID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shader->skyboxID);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		shader->shader->setMat4("model", model);

		shader->GLDraw();
	}
};

class CubeShader : public ObjectShader
{
public:
	virtual void SetVAO() override
	{
		if (this->Vaos == nullptr)
			this->Vaos = new VAO;

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

		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);

		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	virtual void Use(glm::vec3 viewPos) override
	{
		shader->Use();

		shader->setVec3("viewPos", viewPos);
		shader->setFloat("material.shininess", 32.0f);

		Environment::GetInstance()->lights.SetShader(this->shader);

		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		BindTextures();
	}

	virtual void InitShaderAttribute() override
	{

	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};
class Cube
{
public:
	glm::vec3 Position = glm::vec3(0, 0, 0);

	void Draw(CubeShader* shader)
	{
		glBindVertexArray(shader->Vaos->vao);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		shader->shader->setMat4("model", model);

		shader->GLDraw();
	}
};


class HeightMapShader : public ObjectShader
{
public:
	unsigned int skyboxID;
	virtual void InitShaderAttribute() override
	{
		shader->setVec3("material.diffuse", .12f, .26f, .624f);
		shader->setVec3("material.specular", .12f, .26f, .624f);
		shader->setFloat("material.shininess", 32.0f);

		shader->setInt("material.u_skybox", 0);
	}

	/// <summary>
	/// create data : 2020.12.01 - 17:43
	/// reference : https://github.com/h2570su/WaterSurface/blob/master/src/TrainView.cpp  
	///				aSurface::generateVAO()
	/// </summary>
	virtual void SetVAO() override
	{
		if (this->Vaos == nullptr)
			Vaos = new VAO;

		const int clipAmount = 1600;

		float size = 100;

		float sourceVertices[] = {
			-size ,0.0f , -size,
			-size ,0.0f , size ,
			size ,0.0f ,size ,
			size ,0.0f ,-size };
		GLfloat  sourceNormal[] = {
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f };
		GLuint sourceElement[] = {
			0, 1, 2,
			0, 2, 3 };

		int length = ceil(sqrt(clipAmount));

		std::vector<float> vertices;
		std::vector<float> textureCoords;
		std::vector<int> elements;

		GLfloat VzInc = (sourceVertices[5] - sourceVertices[2]) / length;
		GLfloat VzStart = sourceVertices[2];
		GLfloat VzCurr = VzStart;
		GLfloat TyInc = 1.0f / length;
		GLfloat TyStart = 0.0f;
		GLfloat TyCurr = TyStart;

		for (int i = 0; i < length; i++)
		{
			GLfloat VxInc = (sourceVertices[6] - sourceVertices[0]) / length;
			GLfloat VxStart = sourceVertices[0];
			GLfloat VxCurr = VxStart;

			GLfloat TxInc = 1.0f / length;
			GLfloat TxStart = 0.0f;
			GLfloat TxCurr = TxStart;

			for (int j = 0; j < length; j++)
			{
				vertices.push_back(VxCurr);
				vertices.push_back(sourceVertices[1]);
				vertices.push_back(VzCurr);

				vertices.push_back(VxCurr);
				vertices.push_back(sourceVertices[4]);
				vertices.push_back(VzCurr + VzInc);

				vertices.push_back(VxCurr + VxInc);
				vertices.push_back(sourceVertices[7]);
				vertices.push_back(VzCurr + VzInc);

				vertices.push_back(VxCurr + VxInc);
				vertices.push_back(sourceVertices[10]);
				vertices.push_back(VzCurr);

				textureCoords.push_back(TxCurr);
				textureCoords.push_back(TyCurr);

				textureCoords.push_back(TxCurr);
				textureCoords.push_back(TyCurr + TyInc);

				textureCoords.push_back(TxCurr + TxInc);
				textureCoords.push_back(TyCurr + TyInc);

				textureCoords.push_back(TxCurr + TxInc);
				textureCoords.push_back(TyCurr);

				int idx = i * length + j;
				elements.push_back(sourceElement[0] + (idx * 4));
				elements.push_back(sourceElement[1] + (idx * 4));
				elements.push_back(sourceElement[2] + (idx * 4));

				elements.push_back(sourceElement[3] + (idx * 4));
				elements.push_back(sourceElement[4] + (idx * 4));
				elements.push_back(sourceElement[5] + (idx * 4));

				VxCurr += VxInc;
				TxCurr += TxInc;
			}

			VzCurr += VzInc;
			TyCurr += TyInc;
		}

		this->Vaos = new VAO;
		this->Vaos->element_amount = elements.size();

		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(2, this->Vaos->vbo);
		glGenBuffers(1, &this->Vaos->ebo);

		glBindVertexArray(this->Vaos->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Vaos->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLint), elements.data(), GL_STATIC_DRAW);
	}

	virtual void Use(glm::vec3 viewPos) override
	{
		shader->Use();

		shader->setVec3("u_cameraPos", viewPos);

		//加入光效
		Environment::GetInstance()->lights.SetShader(shader);

		shader->setVec3("RippleCenter", glm::vec3(0, 0, 0));
		//轉到螢幕座標
		SetModelView7ProjectionMatrix();
	}

	virtual void GLDraw() override
	{
		glDrawElements(GL_PATCHES, this->Vaos->element_amount, GL_UNSIGNED_INT, 0);
	}

	virtual void UnBindTexture() override
	{
		Texture2D::unbind(0);
		Texture2D::unbind(1);
	}
};
class HeightMapWave
{
	int timer = 0;

	const int WavePictureAmount= 200;

	int animateTimer = 0;
public:
	glm::vec3 Position = glm::vec3(0, 0, 0);

	float WaveHeights[HeightWaveGridSize][HeightWaveGridSize];

	/// <summary>
	/// have to use shader first
	/// </summary>
	/// <param name="aShader"></param>
	virtual void Draw(ObjectShader* aShader, unsigned int reflectSkyTextureID)
	{
		aShader->textures[timer % WavePictureAmount]->bind(0);
		glUniform1i(glGetAttribLocation(aShader->shader->ID, "u_heightMap"), 0);

		glBindVertexArray(aShader->Vaos->vao);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, reflectSkyTextureID);

		aShader->shader->setFloat("u_time", animateTimer);

		//world transformation
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, Position);
		
		aShader->shader->setMat4("u_model", modelMatrix);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		aShader->GLDraw();
	}

	void WaveGoGOo(int _speed)
	{
		timer += _speed;
		animateTimer += _speed;

		if (timer >= WavePictureAmount)
			timer -= WavePictureAmount;
	}

	void ResetAnimate()
	{
		animateTimer = 0;
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

	void Draw(/*object attribute*/glm::vec3 objectPosition, glm::vec3 scale = glm::vec3(1, 1, 1))
	{
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, objectPosition);
		model = glm::scale(model, scale);
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
		modelMat = glm::translate(modelMat, glm::vec3(0.0f, -5.0f, 0.0f)); // translate it down so it's at the center of the scene
		modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader->setMat4("model", modelMat);
		model->Draw(*shader);
	}
};

class SkyBoxShader : public ObjectShader
{
public:
	std::vector<std::string> faces
	{
		"../Images/skybox/right.jpg",
		"../Images/skybox/left.jpg",
		"../Images/skybox/top.jpg",
		"../Images/skybox/bottom.jpg",
		"../Images/skybox/front.jpg",
		"../Images/skybox/back.jpg"
	};
	unsigned int cubemapTexture;
	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	void LoadCubemap()
	{
		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	virtual void SetVAO()override
	{
		if (this->Vaos == nullptr)
			this->Vaos = new VAO;

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		// skybox VAO
		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);
		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	virtual void InitShaderAttribute() override
	{
		shader->setInt("skybox", 0);
	}

	virtual void Use(glm::vec3 viewPos) override
	{
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		shader->Use();
	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	virtual void UnBindTexture() override
	{
		Texture2D::unbind(0);
	}
};
class SkyBox
{
public:

	void Draw(SkyBoxShader* aShader,  glm::mat4 view)
	{
		glm::mat4 projection;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		aShader->shader->setMat4("u_projection", projection);
		aShader->shader->setMat4("u_view", view);

		// skybox cube
		glBindVertexArray(aShader->Vaos->vao);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, aShader->cubemapTexture);
		aShader->GLDraw();
		glDepthFunc(GL_LESS); // set depth function back to default
	}
};

class ScreenShader : public ObjectShader
{
public:
	FBO* Fbos = nullptr;

	virtual void Use(glm::vec3 viewPos) override
	{
		this->shader->Use();

		glBindVertexArray(this->Vaos->vao);
	}

	virtual void InitShaderAttribute() override
	{

	}

	virtual void SetVAO() override
	{
		if (this->Vaos == nullptr)
			this->Vaos = new VAO;

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);
		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void SetFBO()
	{
		if (this->Fbos == nullptr)
			this->Fbos = new FBO;

		glGenFramebuffers(1, &this->Fbos->fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, this->Fbos->fbo);
		// create a color attachment texture
		glGenTextures(1, this->Fbos->textures);
		glBindTexture(GL_TEXTURE_2D, this->Fbos->textures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 590, 590, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Fbos->textures[0], 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &this->Fbos->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, this->Fbos->rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 590, 590); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->Fbos->rbo); // now actually attach it
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindFBO()
	{
		// render
		// ------
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, this->Fbos->fbo);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

		// make sure we clear the framebuffer's content
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void  UnBindFBO()
	{
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// disable depth test so screen-space quad isn't discarded due to depth test.
		glDisable(GL_DEPTH_TEST); 
		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	virtual void Draw(glm::vec3 _)
	{
		this->shader->Use();

		glBindVertexArray(this->Vaos->vao);

		this->shader->setInt("u_screenTexture", this->Fbos->textures[0]);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, this->Fbos->textures[0]);
		this->GLDraw();
		glEnable(GL_DEPTH_TEST);

		Texture2D::unbind(0);
	}
};

class BSplineGeometryShader : public ObjectShader
{
public:
	virtual void InitShaderAttribute() override
	{

	}

	virtual void Use(glm::vec3) override
	{
		shader->Use();

		SetModelView7ProjectionMatrix();

		glBindVertexArray(this->Vaos->vao);
	}

	virtual void SetVAO() override
	{
		if (this->Vaos == nullptr)
			Vaos = new VAO;
		float vertices[] = {
			// positions        
			0, 0, 0
		};

		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);

		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_POINTS, 0, 1);
	}
};
class BSpline
{
public:
	glm::vec3 Position;

	void Draw(BSplineGeometryShader* aShader)
	{
		aShader->GLDraw();
	}
};

class RoadShader : public ObjectShader
{
public:
	const int clipSize = 246;
	virtual void InitShaderAttribute() override
	{

	}

	virtual void SetVAO()
	{
		if (this->Vaos == nullptr)
			this->Vaos = new VAO;

		float vertices[] = {
			// positions         
			-0.5f,  0, -0.5f,  0.0f,  1.0f,
			 0.5f,  0, -0.5f,  1.0f,  1.0f,
			 0.5f,  0,  0.5f,  1.0f,  0.0f,
			 0.5f,  0,  0.5f,  1.0f,  0.0f,
			-0.5f,  0,  0.5f,  0.0f,  0.0f,
			-0.5f,  0, -0.5f,  0.0f,  1.0f
		};

		glGenVertexArrays(1, &this->Vaos->vao);
		glGenBuffers(1, this->Vaos->vbo);

		glBindVertexArray(this->Vaos->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->Vaos->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	virtual void Use(glm::vec3) override
	{
		this->shader->Use();

		this->SetModelView7ProjectionMatrix();

		glBindVertexArray(this->Vaos->vao);

		this->BindTextures();
	}

	virtual void GLDraw() override
	{
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};
class Road
{
public:
	void Draw(RoadShader* aShader)
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		aShader->shader->setMat4("u_model", model);

		aShader->GLDraw();
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

		void UpdataAllShader()
		{
			heightMapShader.UpdataShader();
			HeightWave.ResetAnimate();

			boxShader.UpdataShader();
			lineShader.UpdataShader();
			roadShader.UpdataShader();
		}

		void Distance2T(float& _distance, int& _turnCounter, float& _trainU);
		void Distance2Pos(float _distance, Pnt3f& _pos, Pnt3f& _orient);
		float T2Distance(int& _turnCounter, float& _trianU);
		void ComputeDistance();
		void ARCGoGo(float);
		float TotalDistance = 0;

	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube;  // simple - just remember which cube is selected

		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene
		
		void GetPos(float const t, Pnt3f& pos, Pnt3f& orient);

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

		SkyBoxShader skyBoxShader;
		SkyBox skyBox;

		SkyboxCubeShader boxShader;
		SkyboxTestCube testCube;

		CubeShader cubeShader;
		Cube cube;

		ScreenShader screenShader;

		BSplineGeometryShader lineShader;
		BSpline line;

		RoadShader roadShader;
		Road road;
};