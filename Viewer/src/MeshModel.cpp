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
	/*this->material.ambient= glm::vec3(0.7, 0.5, 0.4);
	this->material.diffuse = glm::vec3(0.7, 0.5, 0.4);
	this->material.specular = glm::vec3(0.7, 0.5, 0.4);
	this->material.shininess = 0.7;
	*/

	Aroundx = 0;
	AroundY = 0;
	AroundZ = 0;
	scaleX = 10;
	scaleY = 10;
	TranslateX = 0;
	TranslateY = 0;
	MeshModel::faces = faces;
	MeshModel::normals = normals;
	MeshModel::modelName = modelName;
	int NumVertices = vertices.size(), i, j;
	int NumNormals = normals.size();
	MeshModel::vertices = vertices;
	glm::vec3 p;

	double min[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
	double max[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };

	/*for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 2; j++) {
			MeshModel::vertices[i][j]  = MeshModel::vertices[i][j] / MeshModel::vertices[i][2];
			MeshModel::vertices[i][j]  = MeshModel::vertices[i][j] / MeshModel::vertices[i][2];
		}
	}*/

	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (MeshModel::vertices[i][j] < min[j]) min[j] = MeshModel::vertices[i][j];
			if (MeshModel::vertices[i][j] > max[j]) max[j] = MeshModel::vertices[i][j];
		}
	}
	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (i == 9 && j == 2)
			{
				double k;
				MeshModel::vertices[i][j] -= (min[j]);
				MeshModel::vertices[i][j] *= 2;
				MeshModel::vertices[i][j] = MeshModel::vertices[i][j] / (max[j] - min[j]);				
				MeshModel::vertices[i][j] = MeshModel::vertices[i][j] - 1;
			}
			else
			{
				MeshModel::vertices[i][j] -= (min[j]);
				MeshModel::vertices[i][j] *= 2;
				MeshModel::vertices[i][j] = MeshModel::vertices[i][j] / (max[j] - min[j]);
				MeshModel::vertices[i][j] = MeshModel::vertices[i][j] - 1;
			}
		}
	}

	/*for (j = 0; j < 3; j++)
		max[j] -= min[j];*/
	/*for (i = 0; i < NumVertices; i++) {
			MeshModel::vertices[i][0] = ((2 * (MeshModel::vertices[i][0] - min[0]) / (max[0] - min[0])) - 1);
			MeshModel::vertices[i][1] = ((2 * (MeshModel::vertices[i][1] - min[1]) / (max[1] - min[1])) - 1);
			MeshModel::vertices[i][2] = ((2 * (MeshModel::vertices[i][2] - min[2]) / (max[2] - min[2])) - 1);
			MeshModel::vertices[i][0]
		
	}*/

	
	Box.push_back({ min[0],max[1],100 });
	Box.push_back({ max[0],max[1],100 });
	Box.push_back({ min[0],min[1],100 });
	Box.push_back({ max[0],min[1],100 });

	
	/*min[0] = MeshModel::normals[0][0];
	min[1] = MeshModel::normals[0][1];
	min[2] = MeshModel::normals[0][2];
	max[0] =  MeshModel::normals[0][0];
	max[1] = MeshModel::normals[0][1];
	max[2] = MeshModel::normals[0][2];



	for (i = 1; i < NumNormals; i++) {
		for (j = 0; j < 3; j++) {
			if (MeshModel::normals[i][j] < min[j]) min[j] = MeshModel::normals[i][j];
			if (MeshModel::normals[i][j] > max[j]) max[j] = MeshModel::normals[i][j];
		}
	}
	for (i = 1; i < NumNormals; i++) {
		for (j = 0; j < 3; j++) {
			if (min[j] < 0) {

				MeshModel::normals[i][j] -= min[j];
			}
		}
	}
	for (j = 0; j < 3; j++)
		max[j] -= min[j];*/

/*
	for (i = 0; i < NumNormals; i++) {
		MeshModel::normals[i][0] = (MeshModel::normals[i][0] / max[0]);
		MeshModel::normals[i][1] = (MeshModel::normals[i][1] / max[1]);
		MeshModel::normals[i][2] = (MeshModel::normals[i][2] / max[2]);
	}*/









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
	
	glm::mat4x4 C,A= worldTransform;
	A= glm::scale(A, glm::vec3(scaleX, scaleY, 1));
	A = glm::rotate(A, Aroundx, { 1, 0, 0 });
	A = glm::rotate(A, AroundY, { 0, 1, 0 });
	A = glm::rotate(A, AroundZ, { 0, 0, 1 });
	A = glm::translate(A, glm::vec3(TranslateX, TranslateY, 1));
	C = glm::translate(A, glm::vec3(TranslateX, TranslateY, 1));

	//A = glm::rotate(A, deg, { Aroundx, AroundY, AroundZ });
	
	C[3][2] = 0;
	//return worldTransform;
	return  C;
}

void MeshModel::setDrawing(bool drawBox, bool drawNormals, bool drawFacesNormals)
{
	this->drawBox = drawBox;
	this->drawNormals = drawNormals;
	this->drawFacesNormals = drawFacesNormals;
}

bool MeshModel::getDrawBox() const
{
	return this->drawBox;
}

bool MeshModel::getDrawNormals() const
{
	return this->drawNormals;
}

bool MeshModel::getdrawFacesNormals() const
{
	return this->drawFacesNormals;
}

const glm::vec4 & MeshModel::getverticesColor() const
{
	return glm::vec4(this->verticesColor);
}

const glm::vec4 & MeshModel::getfillingrColor() const
{
	return glm::vec4(this->fillingrColor);
}

void MeshModel::SetColor(const glm::vec4& verticesColor, const glm::vec4& fillingrColor)
{
	this->verticesColor = verticesColor;
	this->fillingrColor = fillingrColor;

}

void MeshModel::SetMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->material.ambient = ambient;
	this->material.diffuse = diffuse;
	this->material.specular = specular;
	this->material.shininess = shininess;
}

Material MeshModel::getMaterial() const
{
	return this->material;
}



const std::string& MeshModel::GetModelName()
{
	return modelName;
}

std::vector<glm::vec3> MeshModel::getBox()
{
	return this->Box;
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

void MeshModel::Rotate(float AroundX,float AroundY,float AroundZ)
{

	this->Aroundx = AroundX;
	this->AroundY = AroundY;
	this->AroundZ = AroundZ;

	//worldTransform = glm::rotate(worldTransform, a, { 1,1,1 });
}
