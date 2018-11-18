#include "Scene.h"
#include "MeshModel.h"
#include <string>

Scene::Scene() :
	activeCameraIndex(0),
	activeModelIndex(0)
{

}



void Scene::AddModel(const std::shared_ptr<MeshModel>& model)
{
	models.push_back(model);
}

const int Scene::GetModelCount() const
{
	return models.size();
}

void Scene::AddCamera(const Camera& camera)
{
	cameras.push_back(camera);
}

const int Scene::GetCameraCount() const
{
	return cameras.size();
}

void Scene::SetActiveCameraIndex(int index)
{
	// implementation suggestion...
	if (index >= 0 && index < cameras.size())
	{
		activeCameraIndex = index;
	}
}

const int Scene::GetActiveCameraIndex() const
{
	return activeCameraIndex;
}

void Scene::SetActiveModelIndex(int index)
{
	// implementation suggestion...
	if (index >= 0 && index < models.size())
	{
		activeModelIndex = index;
	}
}

const int Scene::GetActiveModelIndex() const
{
	return activeModelIndex;
}

void Scene::ScaleActiveModel(float scaleX, float scaleY)
{	if(GetModelCount())
		models[GetActiveModelIndex()]->SetScale(scaleX, scaleY);
}

void Scene::TranslateActiveModel(float x, float y)
{
	if (GetModelCount())
		models[GetActiveModelIndex()]->Translate(x, y);
}





void Scene::RotateActiveModel(float a)
{
	if (GetModelCount())
		models[GetActiveModelIndex()]->Rotate(a);
}

MeshModel Scene::GetActiveModel() const
{
	
	
	 return *models[GetActiveModelIndex()];

	
	 
}
