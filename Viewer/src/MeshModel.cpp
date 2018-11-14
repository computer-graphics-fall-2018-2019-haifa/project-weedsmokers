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
	int NumVertices = vertices.size(),i;
	 MeshModel::vertices = vertices;
	double min_x = MeshModel::vertices[0][0], min_y = MeshModel::vertices[0][1], min_z = MeshModel::vertices[0][2], max_x = MeshModel::vertices[0][0], max_y = MeshModel::vertices[0][1], max_z = MeshModel::vertices[0][2];
	for (i = 1; i < NumVertices; i++) {
		if (MeshModel::vertices[i][0] < min_x) min_x = MeshModel::vertices[i][0];
		if (MeshModel::vertices[i][1] < min_y) min_y = MeshModel::vertices[i][1];
		if (MeshModel::vertices[i][2] < min_z) min_z = MeshModel::vertices[i][2];
		if (MeshModel::vertices[i][0] > max_x) max_x = MeshModel::vertices[i][0];
		if (MeshModel::vertices[i][1] > max_y) max_y = MeshModel::vertices[i][1];
		if (MeshModel::vertices[i][2] > max_z) max_z = MeshModel::vertices[i][2];
	}
	if (min_x < 0) {
		for (i = 0; i < NumVertices; i++) {
			MeshModel::vertices[i][0] -= min_x;
			max_x -= min_x;
		}
	}
	if (min_y < 0) {
		for (i = 0; i < NumVertices; i++) {
			MeshModel::vertices[i][1] -= min_y;
			max_y -= min_y;
		}
	}
	if (min_z < 0) {
		for (i = 0; i < NumVertices; i++) {
			MeshModel::vertices[i][2] -= min_z;
			max_z -= min_z;
		}
	}
	for (i = 0; i < NumVertices; i++) {
		MeshModel::vertices[i][0] = MeshModel::vertices[i][0] / max_x * 1280;
		MeshModel::vertices[i][1] = MeshModel::vertices[i][1] / max_y * 720;
		MeshModel::vertices[i][2] = MeshModel::vertices[i][2] / max_z;
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