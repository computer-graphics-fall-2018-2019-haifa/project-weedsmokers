#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<Camera> cameras;
	bool drawNormals = false;
	int activeCameraIndex;
	int activeModelIndex;
	bool drawFacesNormals = false;
	bool Box = false;
public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	const int GetModelCount() const;

	void AddCamera(const Camera& camera);
	const int GetCameraCount() const;

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;
	void ScaleActiveModel(float scaleX, float scaleY);
	void TranslateActiveModel(float x, float y);
	void RotateActiveModel( float AroundX, float AroundY, float AroundZ);
	  MeshModel GetActiveModel() const ;
	  Camera GetActiveCamera() const;
	  void SetdrawNormals(bool drawNormals);
	  bool getdrawNormals() const;
	  void SetdrawFacesNormals(bool drawFacesNormals);
	  bool getdrawFacesNormals() const;
	  void SetdrawBox(bool Box);
	  bool getdrawBox() const;
	// Add more methods as needed...
};