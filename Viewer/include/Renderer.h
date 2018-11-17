#pragma once
#include "Scene.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

/*
 * Renderer class.
 */
class Renderer :public Scene
{
private:
	float *colorBuffer;
	float *zBuffer;
	int viewportWidth;
	int viewportHeight;
	int viewportX;
	int viewportY;

	void putPixel(int x, int y, const glm::vec3& color);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	//Renderer(int viewportWidth=0, int viewportHeight=0, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render(const Scene& scene);
	void SwapBuffers();
	void drawLine(const glm::vec2 & p0, const glm::vec2 & p1);
	void drawLineLow(const glm::vec2 & p0, const glm::vec2 & p1);
	void drawLineHight(const glm::vec2 & p0, const glm::vec2 & p1);
	void drawTriangles(const std::vector<glm::vec3> * points, const glm::vec3 * normals= NULL );
	int ReScaleX(float num);
	int ReScaleY(float num);
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);


	void putPixel2(int i, int j, const glm::vec3& color);

	// write function that takes a model and draw it
	// Add more methods/functionality as needed...
};

