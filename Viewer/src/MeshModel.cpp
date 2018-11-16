#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1))
{
	MeshModel::normals = normals;
	MeshModel::modelName = modelName;
	int NumVertices = vertices.size(),i,j;
	 MeshModel::vertices = vertices;
	 double min[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
	 double max[3] = { MeshModel::vertices[0][0],MeshModel::vertices[0][1],MeshModel::vertices[0][2] };
	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (MeshModel::vertices[i][j] < min[j]) min[j] = MeshModel::vertices[i][j];
			if (MeshModel::vertices[i][j] > max[j]) max[j] = MeshModel::vertices[i][j];
		}
	}
	for (i = 1; i < NumVertices; i++) {
		for (j = 0; j < 3; j++) {
			if (min[j] < 0) {
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
	return worldTransform;
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