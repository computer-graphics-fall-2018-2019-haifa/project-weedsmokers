#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include <cmath>
#include <memory>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>
float AroundX = 0;
float AroundY = 0;
float AroundZ = 0;
bool showDemoWindow = false;
bool showAnotherWindow = false;
bool drawNormals = false;
bool drawFacesNormals = false;
bool Box;
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &showAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Add camera"))
		{// Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
			scene.AddCamera(Camera());
		}
		ImGui::SameLine();
		ImGui::Text("Cameras # = %d", counter);
		static int i1 = 0;
		ImGui::SliderInt("Choose camera", &i1, 0, scene.GetCameraCount());
		scene.SetActiveCameraIndex(i1);

		static float scaleX = 0.5;
		ImGui::SliderFloat("scaleX", &scaleX, 0.01, 10);
		static float scaleY = 0.5;
		ImGui::SliderFloat("scaleY", &scaleY, 0.01, 10);
		scene.ScaleActiveModel(scaleX, scaleY);

		static float TranslateX = 0.3;
		ImGui::SliderFloat("TranslateX", &TranslateX, 0, 1);
		static float TranslateY = 0.3;
		ImGui::SliderFloat("TranslateY", &TranslateY, 0, 1);
		scene.TranslateActiveModel(TranslateX, TranslateY);


		//ImGui::Checkbox("Around X", &AroundX);
		//ImGui::Checkbox("Around Y", &AroundY);
		//ImGui::Checkbox("Around Z", &AroundZ);
		ImGui::SliderFloat("RoateAroundX", &AroundX, -1.57, 1.57);
		ImGui::SliderFloat("RotateAroundY", &AroundY, -1.57, 1.57);
		ImGui::SliderFloat("RotateAroundZ", &AroundZ, -1.57, 1.57);
		scene.RotateActiveModel( AroundX, AroundY, AroundZ);

		ImGui::Checkbox("Draw Normals", &drawNormals);
		scene.SetdrawNormals(drawNormals);

		ImGui::Checkbox("Draw Faces Normals", &drawFacesNormals);
		scene.SetdrawFacesNormals(drawFacesNormals);

		ImGui::Checkbox("Draw Box", &Box);
		scene.SetdrawBox(Box);

		if (ImGui::Button("Reset"))
		{
			scaleX = 0.5;
			scaleY = 0.5;
			TranslateX = 0.3;
			TranslateY = 0.3;
			AroundX = 0;
			AroundY = 0;
			AroundZ = 0;
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (showAnotherWindow)
	{
		ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
		{
			showAnotherWindow = false;
		}
		ImGui::End();
	}

	// 4. Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Model...", "CTRL+O"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}