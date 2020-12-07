#pragma once

#include <glad/glad.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>

#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"

namespace Lighting
{
	enum class EmLightType : int
	{
		Base = -1,
		Dir = 0,
		Point = 1,
		Spot = 2,
	};

	#pragma region Light_Interface
	class BaseLight
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	public:
		/// <summary>
		/// notice : id will setup get from environment;
		///			 if didn't put in environment restores;
		///			 this id is -1
		/// </summary>
		int ID = -1;
		EmLightType Type = EmLightType::Base;

		BaseLight() { };

		BaseLight(BaseLight& _light)
		{
			this->SetAmbient(_light.GetAmbient());
			this->SetDiffuse(_light.GetDiffuse());
			this->SetSpecular(_light.GetSpecular());
		};

		void SetAmbient(glm::vec3 _ambient)
		{
			ambient = _ambient;
		}
		void SetDiffuse(glm::vec3 _diffuse)
		{
			diffuse = _diffuse;

		}
		void SetSpecular(glm::vec3 _specular)
		{
			specular = _specular;
		}
		glm::vec3& GetAmbient()
		{
			return ambient;
		}
		glm::vec3& GetDiffuse()
		{
			return diffuse;
		}
		glm::vec3& GetSpecular()
		{
			return specular;
		}
	};
	class IDir
	{
	public:
		virtual void SetDirection(glm::vec3) = 0;
		virtual glm::vec3& GetDirection() = 0;
	};

	class IAttenuation
	{
	public:
		virtual void SetConstant(float) = 0;
		virtual float& GetConstant() = 0;

		virtual void SetLinear(float) = 0;
		virtual float& GetLinear() = 0;

		virtual void SetQuadratic(float) = 0;
		virtual float& GetQuadratic() = 0;
	};

	class IPoint
	{
	public:
		virtual void SetPosition(glm::vec3) = 0;
		virtual glm::vec3& GetPosition() = 0;
	};

	class ISpot
	{
	public:
		virtual void SetCutOff(float) = 0;
		virtual float& GetCutOff() = 0; 
	};

	class ISmoothSpot
	{
	public:
		virtual void SetCutOff(float) = 0;
		virtual float& GetCutOff() = 0;
		virtual void SetOuterCutOff(float) = 0;
		virtual float& GetOuterCutOff() = 0;
	};
	#pragma endregion Light_Interface

	class DirLight : public BaseLight, public IDir
	{
		glm::vec3 direction;
	public:
		DirLight()
		{
			Type = EmLightType::Dir;
		}
		virtual void SetDirection(glm::vec3 _direction) override
		{
			direction = glm::normalize(_direction);
		}
		virtual glm::vec3& GetDirection() override
		{
			return direction;
		}
	};

	class PointLight : public BaseLight, public IPoint, public IAttenuation
	{
		glm::vec3 position;

		float constant = 0;
		float linear = 0;
		float quadratic = 0;
	public:		
		PointLight()
		{
			Type = EmLightType::Point;
		}
		virtual void SetPosition(glm::vec3 _position) override
		{
			position = _position;
		}
		virtual glm::vec3& GetPosition() override
		{
			return position;
		}

		virtual void SetConstant(float _constant) override
		{
			constant = _constant;
		}
		virtual float& GetConstant() override
		{
			return constant;
		}

		virtual void SetLinear(float _linear) override
		{
			linear = _linear;
		}
		virtual float& GetLinear() override
		{
			return linear;
		}

		virtual void SetQuadratic(float _quadratic) override
		{
			quadratic = _quadratic;
		}
		virtual float& GetQuadratic() override
		{
			return quadratic;
		}
	};

	class SpotLight : public BaseLight, public IDir, public IPoint, public IAttenuation, public ISmoothSpot
	{
		glm::vec3 position;
		glm::vec3 direction;

		float cutOff = 0;
		float outerCutOff = 0;

		float constant = 0;
		float linear = 0;
		float quadratic = 0;
	public:
		SpotLight()
		{
			Type = EmLightType::Spot;
		}

		SpotLight(SpotLight& _spot)
		{
			Type = EmLightType::Spot;
			this->SetDirection(_spot.GetDirection());
			this->SetPosition(_spot.GetPosition());
			this->SetCutOff(_spot.GetCutOff());
			this->SetOuterCutOff(_spot.GetOuterCutOff());
			this->SetConstant(_spot.GetConstant());
			this->SetLinear(_spot.GetLinear());
			this->SetQuadratic(_spot.GetQuadratic());
		}

		virtual void SetDirection(glm::vec3 _dir) override
		{
			direction = glm::normalize(_dir);
		}
		virtual glm::vec3& GetDirection() override
		{
			return direction;
		}

		virtual void SetPosition(glm::vec3 _position) override
		{
			position = _position;
		}
		virtual glm::vec3& GetPosition() override
		{
			return position;
		}

		virtual void SetCutOff(float _cutOff) override
		{
			cutOff = _cutOff;
		}
		virtual float& GetCutOff() override
		{
			return cutOff;
		}

		virtual void SetOuterCutOff(float _outerCutOff) override
		{
			outerCutOff = _outerCutOff;
		}
		virtual float& GetOuterCutOff() override
		{
			return outerCutOff;
		}

		virtual void SetConstant(float _constant) override
		{
			constant = _constant;
		}
		virtual float& GetConstant() override
		{
			return constant;
		}

		virtual void SetLinear(float _linear) override
		{
			linear = _linear;
		}
		virtual float& GetLinear() override
		{
			return linear;
		}

		virtual void SetQuadratic(float _quadratic) override
		{
			quadratic = _quadratic;
		}
		virtual float& GetQuadratic() override
		{
			return quadratic;
		}
	};
}

class Environment
{
private:
	static Environment* instance;

	Environment() { }
public:
	//this should not be cloneable
	Environment(Environment& other) = delete;
	void operator=(const Environment&) = delete;

	static Environment* GetInstance();

	class Lights
	{
		//報明牌
		int generateID()
		{
			int id = rand() % 624;

			int skipCounter = 0;
			while (Lightings.find(id) != Lightings.end())
			{
				id = rand() % 624;
				skipCounter++;

				if (skipCounter > 246)
					throw new std::exception("light restores full");
			}
			return id;
		}

		const int Max_DirectionLight_Amount = 1;
		const int Max_PositionLight_Amount = 4;
		const int Max_SpotLight_Amount = 1;
	public:
		Lights()
		{
			Lightings.clear();
		}

		std::map<int, Lighting::BaseLight*> Lightings;

		void AddLight(Lighting::BaseLight* _light)
		{
			int id = generateID();
			_light->ID = id;
			Lightings[id] = _light;
		}
	
		void RemoveLight(Lighting::BaseLight* _light)
		{
			int id = _light->ID;
			
			Lightings.erase(id);
		}

		/// <summary>
		/// set this shader within light effect
		/// </summary>
		/// <param name="shader"></param>
		void SetShader(Shader* shader)
		{
			const std::string dirLightInShaderName = "dirLights";
			const std::string pointLightInShaderName = "pointLights";
			const std::string spotLightInShaderName = "spotLights";

			int dirCounter = 0;
			int pointCounter = 0;
			int spotCounter = 0;

			for (auto& light : Lightings)
			{
				if (light.second->Type == Lighting::EmLightType::Dir)
				{
					if (dirCounter > Max_DirectionLight_Amount)
					{
						std::cout << "number of direction lighting bigger than the shader setting\n";
						continue;
					}
					Lighting::DirLight* dirL = static_cast<Lighting::DirLight*>(light.second);
					std::string lightName = dirLightInShaderName + "[";
					lightName += std::to_string(dirCounter);
					lightName += "].";

					shader->setVec3((lightName + "direction"), glm::vec3(dirL->GetDirection().x, dirL->GetDirection().y, dirL->GetDirection().z));
					shader->setVec3((lightName + "ambient"), glm::vec3(dirL->GetAmbient().x, dirL->GetAmbient().y, dirL->GetAmbient().z));
					shader->setVec3((lightName + "diffuse"), glm::vec3(dirL->GetDiffuse().x, dirL->GetDiffuse().y, dirL->GetDiffuse().z));
					shader->setVec3((lightName + "specular"), glm::vec3(dirL->GetSpecular().x, dirL->GetSpecular().y, dirL->GetSpecular().z));
					dirCounter++;
				}
				else if (light.second->Type == Lighting::EmLightType::Point)
				{
					if (pointCounter > Max_PositionLight_Amount)
					{
						std::cout << "number of position lighting bigger than the shader setting\n";
						continue;
					}
					Lighting::PointLight* pointL = static_cast<Lighting::PointLight*>(light.second);
					std::string lightName = pointLightInShaderName + "[";
					lightName += std::to_string(pointCounter);
					lightName += "].";

					shader->setVec3((lightName + "position"), glm::vec3(pointL->GetPosition().x, pointL->GetPosition().y, pointL->GetPosition().z));
					shader->setVec3((lightName + "ambient"), glm::vec3(pointL->GetAmbient().x, pointL->GetAmbient().y, pointL->GetAmbient().z));
					shader->setVec3((lightName + "diffuse"), glm::vec3(pointL->GetDiffuse().x, pointL->GetDiffuse().y, pointL->GetDiffuse().z));
					shader->setVec3((lightName + "specular"), glm::vec3(pointL->GetSpecular().x, pointL->GetSpecular().y, pointL->GetSpecular().z));
					shader->setFloat((lightName + "constant"), float(pointL->GetConstant()));
					shader->setFloat((lightName + "linear"), float(pointL->GetLinear()));
					shader->setFloat((lightName + "quadratic"), float(pointL->GetQuadratic()));
					pointCounter++;
				}
				else if (light.second->Type == Lighting::EmLightType::Spot)
				{					
					if (spotCounter > Max_SpotLight_Amount)
					{
						std::cout << "number of spot lighting bigger than the shader setting\n";
						continue;
					}
					Lighting::SpotLight* spotL = static_cast<Lighting::SpotLight*>(light.second);
					std::string lightName = spotLightInShaderName + "[";
					lightName += std::to_string(spotCounter);
					lightName += "].";

					shader->setVec3((lightName + "position"), glm::vec3(spotL->GetPosition().x, spotL->GetPosition().y, spotL->GetPosition().z));
					shader->setVec3((lightName + "direction"), glm::vec3(spotL->GetDirection().x, spotL->GetDirection().y, spotL->GetDirection().z));
					shader->setVec3((lightName + "ambient"), glm::vec3(spotL->GetAmbient().x, spotL->GetAmbient().y, spotL->GetAmbient().z));
					shader->setVec3((lightName + "diffuse"), glm::vec3(spotL->GetDiffuse().x, spotL->GetDiffuse().y, spotL->GetDiffuse().z));
					shader->setVec3((lightName + "specular"), glm::vec3(spotL->GetSpecular().x, spotL->GetSpecular().y, spotL->GetSpecular().z));
					shader->setFloat((lightName + "constant"), float(spotL->GetConstant()));
					shader->setFloat((lightName + "linear"), float(spotL->GetLinear()));
					shader->setFloat((lightName + "quadratic"), float(spotL->GetQuadratic()));
					shader->setFloat((lightName + "cutOff"), glm::cos(glm::radians(spotL->GetCutOff())));
					shader->setFloat((lightName + "outerCutOff"), glm::cos(glm::radians(spotL->GetOuterCutOff())));
					spotCounter++;
				}
				else
					throw new std::exception("error light type");

				shader->setInt("u_PointLightAmount", pointCounter);
				shader->setInt("u_DirLightAmount", dirCounter);
				shader->setInt("u_SpotLightAmount", spotCounter);
			}

			/*at liast setting once time*/ 
			[=](Shader* _shader) mutable -> void
			{
				if (dirCounter == 0)
				{
					std::string lightName = dirLightInShaderName + "[";
					lightName += std::to_string(dirCounter);
					lightName += "].";

					shader->setVec3((lightName + "direction"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "ambient"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "diffuse"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "specular"), glm::vec3(0, 0, 0));
				}
				if (pointCounter == 0)
				{
					std::string lightName = pointLightInShaderName + "[";
					lightName += std::to_string(pointCounter);
					lightName += "].";

					shader->setVec3((lightName + "position"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "ambient"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "diffuse"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "specular"), glm::vec3(0, 0, 0));
					shader->setFloat((lightName + "constant"), float(0));
					shader->setFloat((lightName + "linear"), float(0));
					shader->setFloat((lightName + "quadratic"), float(0));
				}
				if (spotCounter == 0)
				{
					std::string lightName = spotLightInShaderName + "[";
					lightName += std::to_string(spotCounter);
					lightName += "].";

					shader->setVec3((lightName + "position"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "direction"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "ambient"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "diffuse"), glm::vec3(0, 0, 0));
					shader->setVec3((lightName + "specular"), glm::vec3(0, 0, 0));
					shader->setFloat((lightName + "constant"), float(0));
					shader->setFloat((lightName + "linear"), float(0));
					shader->setFloat((lightName + "quadratic"), float(0));
					shader->setFloat((lightName + "cutOff"), glm::cos(glm::radians(0.0f)));
					shader->setFloat((lightName + "outerCutOff"), glm::cos(glm::radians(0.0f)));
				}
			} (shader);
		}

		~Lights()
		{
			for (int i = 0; i < Lightings.size(); i++)
			{
				delete &Lightings[i];
			}
			Lightings.clear();
		}
	};
	Lights lights;

	~Environment()
	{
		delete instance;
	}
};

class ObjectShader
{
public:
	std::vector<Texture2D*> textures;
	Shader* shader = nullptr;
	VAO* Vaos = nullptr;

	std::string strVert ;
	std::string strTesc ;
	std::string strTese ;
	std::string strGeom ;
	std::string strFrag ;

	/// <summary>
	/// setting constantly attribute
	/// </summary>
	virtual void InitShaderAttribute()
	{
		shader->setVec3("material.diffuse", 1, 1, 1);
		shader->setVec3("material.specular", 1, 1, 1);
		shader->setFloat("material.shininess", 32.0f);
	}

	void UpdataShader()
	{
		if (shader != nullptr)
		{
			delete shader;
			shader = nullptr;
		}

		const GLchar* vert;
		const GLchar* tesc;
		const GLchar* tese;
		const GLchar* geom;
		const GLchar* frag;
		if (strVert == "")
			vert = nullptr;
		else
			vert = strVert.c_str();

		if (strTesc == "")
			tesc = nullptr;
		else
			tesc = strTesc.c_str();

		if (strTese == "")
			tese = nullptr;
		else
			tese = strTese.c_str();

		if (strGeom == "")
			geom = nullptr;
		else
			geom = strGeom.c_str();

		if (strFrag == "")
			frag = nullptr;
		else
			frag = strFrag.c_str();

		shader = new Shader(vert, tesc, tese, geom, frag);

		shader->Use();
		InitShaderAttribute();
	}

	void SetShader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag)
	{
		strVert = std::string(vert);
		if(tesc != nullptr)
			strTesc = std::string(tesc);
		if(tese != nullptr)
			strTese = std::string(tese);
		if(geom != nullptr)
			strGeom = std::string(geom);
		strFrag = std::string(frag);

		UpdataShader();
	}

	void AddTexture(const GLchar* path)
	{
		Texture2D* texture = new Texture2D(path);
		textures.push_back(texture);
	}
	
	/// <summary>
	/// create data : 2020.12.01 - 17:43
	/// reference : https://github.com/h2570su/WaterSurface/blob/master/src/TrainView.cpp  
	///				aSurface::generateVAO()
	/// </summary>
	virtual void SetVAO()
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

	/// <summary>
	/// setting shader model view matrix and proection matrix
	/// </summary>
	void SetModelView7ProjectionMatrix()
	{
		glm::mat4 projection;
		glm::mat4 view;
		glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
		shader->setMat4("u_projection", projection);
		shader->setMat4("u_view", view);
	}

	void BindTextures()
	{
		for (int i = 0; i < textures.size(); i++)
		{
			textures[i]->bind(i);
		}
	}

	/// <summary>
	/// setting shader attribute and bind texture
	/// </summary>
	virtual void Use(glm::vec3 viewPos)
	{
		shader->Use();
		shader->setVec3("viewPos", viewPos);

		//加入光效
		Environment::GetInstance()->lights.SetShader(shader);

		//轉到螢幕座標
		SetModelView7ProjectionMatrix();

		BindTextures();
	}

	/// <summary>
	/// if SetVAO change, this function maybe change
	/// </summary>
	virtual void GLDraw()
	{
		glDrawElements(GL_PATCHES, this->Vaos->element_amount, GL_UNSIGNED_INT, 0);
	}

	~ObjectShader()
	{
		glDeleteVertexArrays(1, &this->Vaos->vao);
		glDeleteBuffers(4, this->Vaos->vbo);
		glDeleteBuffers(1, &this->Vaos->ebo);

		delete Vaos;
		delete shader;
		for (int i = 0; i < textures.size(); i++)
		{
			delete textures[i];
		}
		textures.clear();
	}
};

/// <summary>
/// example object
/// </summary>
class ExampleGameObject
{	
public:
	glm::vec3 Position;

	/// <summary>
	/// have to use shader first
	/// </summary>
	/// <param name="aShader"></param>
	void BasicDraw(ObjectShader* aShader)
	{
		glBindVertexArray(aShader->Vaos->vao);
		//world transformation
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, Position);
		
		aShader->shader->setMat4("model", modelMatrix);
		
		aShader->GLDraw();
	}
};