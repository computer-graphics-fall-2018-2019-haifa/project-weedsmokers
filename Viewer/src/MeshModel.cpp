#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1))
{
<<<<<<< HEAD
	scaleX = 1;
	scaleY = 1;
	TranslateX = 0;
	TranslateY = 0;
	deg = 0;
	MeshModel::faces = faces;
	MeshModel::normals = normals;
	MeshModel::modelName = modelName;
	int NumVertices = vertices.size(), i, j;
	MeshModel::vertices = vertices;

	double min[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
	double max[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };

	/*for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 2; j++) {
			MeshModel::vertices[i][j]  = MeshModel::vertices[i][j] / MeshModel::vertices[i][2];
			MeshModel::vertices[i][j]  = MeshModel::vertices[i][j] / MeshModel::vertices[i][2];
		}
	}*/


=======
	MeshModel::normals = normals;
	MeshModel::modelName = modelName;
	int NumVertices = vertices.size(),i,j;
	 MeshModel::vertices = vertices;
	 double min[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
	 double max[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
>>>>>>> c30a5e23d5f041a0efaba9ff9d68c6373c313ebd
	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (MeshModel::vertices[i][j] < min[j]) min[j] = MeshModel::vertices[i][j];
			if (MeshModel::vertices[i][j] > max[j]) max[j] = MeshModel::vertices[i][j];
		}
	}
	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (min[j] < 0) {
<<<<<<< HEAD
				MeshModel::vertices[i][j] -= min[j];				
			}
		}
	}

	for (j = 0; j < 3; j++)
		max[j] -= min[j];
	for (i = 0; i < NumVertices; i++) {
		MeshModel::vertices[i][0] = (MeshModel::vertices[i][0] / max[0])*0.8;
		MeshModel::vertices[i][1] = (MeshModel::vertices[i][1] / max[1])*0.8;
		MeshModel::vertices[i][2] = (MeshModel::vertices[i][2] / max[2])*0.8;
	}










=======
				MeshModel::vertices[i][j] -= min[j];
				max[j] -= min[j];
			}
		}
	}
	for (i = 0; i < NumVertices; i++) {
		MeshModel::vertices[i][0] = MeshModel::vertices[i][0] / max[0] * 1280;
		MeshModel::vertices[i][1] = MeshModel::vertices[i][1] / max[1] * 720;
		MeshModel::vertices[i][2] = MeshModel::vertices[i][2] / max[2];
	}
>>>>>>> c30a5e23d5f041a0efaba9ff9d68c6373c313ebd
}


MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	glm::mat4x4 A= worldTransform;
	A = glm::translate(A, glm::vec3(TranslateX, TranslateY, 1));
	A =  glm::rotate(A, deg, { 1,1,1 });
	A =  glm::scale(A, glm::vec3(scaleX, scaleY, 1));
	//return worldTransform;
	return  A;
}

void MeshModel::SetColor(const glm::vec4& color)
{
	this->color = color;
}

const glm::vec4& MeshModel::GetColor() const
{
	return color;
}

const std::string& MeshModel::GetModelName()
{
	return modelName;
}

void MeshModel::SetScale(float scaleX, float scaleY)
{
	/*for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i][0] *= scaleX;
		vertices[i][1] *= scaleY;
	}*/
	//worldTransform = glm::scale(worldTransform, glm::vec3(scaleX,scaleY,1));
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void MeshModel::Translate(float x, float y)
{
	this->TranslateX = x;
	this->TranslateY = y;
	//worldTransform = glm::translate(worldTransform, glm::vec3(scaleX, scaleY, 1));
}

void MeshModel::Rotate(float a)
{
	this->deg = a;
	//worldTransform = glm::rotate(worldTransform, a, { 1,1,1 });
}
