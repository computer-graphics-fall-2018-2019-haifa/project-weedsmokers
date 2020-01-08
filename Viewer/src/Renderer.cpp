#define _USE_MATH_DEFINES
#include <iostream>
//#include <armadillo>
#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "ImguiMenus.h"
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define INDEX2(width,x,y) ((x)+(y)*(width))*3
static int X_width = 500;
static int Y_width = 280;

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	zBuffer = NULL;
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
}

Renderer::~Renderer()
{
	if (colorBuffer!=NULL)
	{
		delete[] colorBuffer;
	}
	if (zBuffer!=NULL)
	{

		delete[] zBuffer;
	}
}

void Renderer::putPixel(int i, int j, const glm::vec3& color) const
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;

	
	colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x; 
	colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
	colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
}

void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{

	if (colorBuffer)
	{
		delete[] colorBuffer;
		colorBuffer = NULL;

	}
	if (zBuffer!=NULL)
	{
		delete[] zBuffer;
		zBuffer = NULL;
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	zBuffer = new float[ viewportWidth * (viewportHeight +1)];

	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f));
			zBuffer[x + y * viewportWidth] = INT_MIN;
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			putPixel(i, j, color);
		}
	}
}

void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	createBuffers(viewportWidth, viewportHeight);
	createOpenGLBuffer();
}

void Renderer::drawModel(const Scene & scene,const MeshModel &model)
{
	Camera camera = scene.GetActiveCamera();		// Data needed to draw the model
	glm::mat4x4 modelViewMatrix = model.GetWorldTransformation();
	glm::mat4x4 projectionMatrix = camera.getProjectionTransformation();
	glm::mat4x4 viewTransformation = camera.getviewTransformation();
	glm::mat3x2 scaleMatrix;
	bool drawBox = scene.getdrawBox();//model.getDrawBox();
	bool drawNormals = scene.getdrawNormals();
	bool drawFacesNormals = scene.getdrawFacesNormals();


	glm::vec4 verticesColor = model.getverticesColor();
	glm::vec4 fillingrColor = model.getfillingrColor();
	Material material = model.getMaterial();
	glm::vec4 ambient = scene.getAmbientLightColor();

	int i,j;
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	glm::vec4 point;
	glm::vec3 faceNormal,temp, tempNormal, normal, pointNormal;
	glm::vec3 vertex2, vertex3;
	glm::vec4 v1, v2;

	point = glm::vec4(model.vertices[0], 1);
	point = projectionMatrix * viewTransformation * modelViewMatrix * point;
	point.x = point.x / point.w;
	point.y = point.y / point.w;
	point.z = point.z / point.w;
	scaleMatrix[0][0] = point.x;
	scaleMatrix[0][1] = point.x;
	scaleMatrix[1][0] = point.y;
	scaleMatrix[1][1] = point.y;
	scaleMatrix[2][0] = point.z;
	scaleMatrix[2][1] = point.z;
	for (i = 1; i < model.vertices.size(); i++)
	{
		point = glm::vec4(model.vertices[i],1);
		point =projectionMatrix * viewTransformation * modelViewMatrix * point;
		point.x = point.x / point.w;
		point.y = point.y / point.w;
		point.z = point.z / point.w;

		scaleMatrix[0][0] = std::max(scaleMatrix[0][0], point.x);
		scaleMatrix[0][1] = std::min(scaleMatrix[0][0], point.x);
		scaleMatrix[1][0] = std::max(scaleMatrix[1][0], point.y);
		scaleMatrix[1][1] = std::min(scaleMatrix[1][1], point.y);
		scaleMatrix[2][0] = std::max(scaleMatrix[2][0], point.z);
		scaleMatrix[2][1] = std::min(scaleMatrix[2][1], point.z);


	}
	std::vector<glm::vec4> tempbox;
	std::vector<glm::vec3> box;
	if (drawBox)
	{

		point = glm::vec4(scaleMatrix[0][0], scaleMatrix[1][0], scaleMatrix[2][0], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][0], scaleMatrix[1][0], scaleMatrix[2][1], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][0], scaleMatrix[1][1], scaleMatrix[2][0], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][0], scaleMatrix[1][0], scaleMatrix[2][1], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][1], scaleMatrix[1][0], scaleMatrix[2][0], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][1], scaleMatrix[1][0], scaleMatrix[2][1], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][1], scaleMatrix[1][1], scaleMatrix[2][0], 1);
		tempbox.push_back(point);
		point = glm::vec4(scaleMatrix[0][1], scaleMatrix[1][1], scaleMatrix[2][1], 1);
		tempbox.push_back(point);


		for (int a = 0; a < 8; a++)
		{
			temp = tempbox[a];
			temp = Scale(temp);
			if (a == 0)
				temp.x + 1;
			box.push_back(temp);
		}
		int moveX = 25;
		int moveY=25;
		int minY = box[2].y;
		int maxY = box[0].y;
		int minX = box[5].x;
		int maxX = box[0].x;

		for (int i = minY; i < maxY; i++)
		{
			
			if (i < maxY - moveX && i>minY + moveY)
				putPixel(minX + moveX, i, glm::vec3(1.0f, 1.0f, 1.0f));

		}
		for (int i = minY; i < maxY; i++) {
			if (i < maxY - moveX && i>minY + moveY)
				putPixel(maxX + moveX, i, glm::vec3(1.0f, 1.0f, 1.0f));

		}

		drawLine2(glm::vec3(minX + moveX, minY + moveY, 0), box[6], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		drawLine2(glm::vec3(maxX + moveX, maxY - moveY, 0), box[1], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		drawLine2(glm::vec3(maxX + moveX, minY + moveY, 0), box[2], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		drawLine2(glm::vec3(minX + moveX, maxY - moveY, 0), box[5], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));


		drawLine2(glm::vec3(minX + moveX, minY + moveY, 0), glm::vec3(maxX + moveX, minY + moveY, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		drawLine2(glm::vec3(minX + moveX, maxY - moveY, 0), glm::vec3(maxX + moveX, maxY - moveY, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));




	}
	glm::vec4 vertex = glm::vec4(0, 0,0, 0);
	glm::mat3x3 normMatrix = glm::transpose(glm::inverse(modelViewMatrix));
	for (i = 0; i < model.faces.size(); i++)
	{
		for (int j = 0; j < 3; j++) {	// filling each face info in vector and sending it to be drawn.
			point = glm::vec4(model.vertices[(model.faces[i].GetVertexIndex(j)) - 1], 1);
			normal = model.normals[(model.faces[i].GetNormalIndex(j)) - 1];
			normal = normMatrix * normal;
			normal = normalize(normal);
			//normal = normal * inv(modelViewMatrix);
			point = projectionMatrix*viewTransformation*modelViewMatrix * point;
			point.x = point.x / point.w;
			point.y = point.y / point.w;
			point.z = point.z / point.w;
			

			if (drawNormals)
			{
				temp = point;
				temp = Scale(temp);

				//pointNormal = glm::vec4(temp, 1) + glm::vec4(tempNormal, 1) * float(scene.getNoramlsLength());
				tempNormal.x = int(normal.x*scene.getNoramlsLength());
				tempNormal.y = int(normal.y*scene.getNoramlsLength());
				tempNormal.z = int(normal.z*scene.getNoramlsLength());

				pointNormal = temp + tempNormal;

				drawLine2(temp, pointNormal, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));

			}
			points.push_back(model.vertices[(model.faces[i].GetVertexIndex(j)) - 1]);
			normals.push_back(normal);
		
		}
		if (scene.getdrawFacesNormals())
		{

			 //vertex = projectionMatrix*(glm::vec4(points[0],1) + glm::vec4(points[1], 1) + glm::vec4(points[2], 1)) / float(3);
			for (int k = 0; k < 3; k++)
			{
				point = glm::vec4(model.vertices[(model.faces[i].GetVertexIndex(k)) - 1], 1);
				 point = projectionMatrix * viewTransformation*modelViewMatrix * point;
				 point.x = point.x / point.w;
				 point.y = point.y / point.w;
				 point.z = point.z / point.w;

				vertex = vertex +(point/float(3));

			}
			vertex2 = vertex;
			vertex2 = Scale(vertex2);
			//vertex.x = ReScaleX(vertex.x);
			//vertex.y = ReScaleY(vertex.y);

			faceNormal = normalize(cross(points[1] - points[0], points[2] - points[0]));
			faceNormal.x = int(faceNormal.x*float(scene.getFacesNoramlsLength()));
			faceNormal.y = int(faceNormal.y*float(scene.getFacesNoramlsLength()));
			faceNormal.z = int(faceNormal.z*float(scene.getFacesNoramlsLength()));

			vertex3 = vertex2 + faceNormal;
			
			drawLine2(vertex2, vertex3, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
			vertex = glm::vec4(0, 0, 0, 0);

		}
		drawTriangles(&points, &normals, scene, model,scaleMatrix);
		points.clear();
		normals.clear();
	}
	if (drawBox)
	{
		
		int move = 25;
		int minY = box[2].y;
		int maxY = box[0].y;
		int minX = box[5].x;
		int maxX = box[0].x;
		
		for (int i = minY; i < maxY; i++)
		{
			putPixel(minX, i, glm::vec3(1.0f, 1.0f, 1.0f));
			//if (i < maxY - move && i>minY+move)
				//putPixel(minX + move, i, glm::vec3(1.0f, 1.0f, 1.0f));

		}
		for (int i = minY ; i < maxY  ; i++) {
			putPixel(maxX, i, glm::vec3(1.0f, 1.0f, 1.0f));
		//	if (i < maxY - move && i>minY + move)
				//putPixel(maxX + move, i, glm::vec3(1.0f, 1.0f, 1.0f));

		}

		drawLine2(box[0], box[4], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		//drawLine2(glm::vec3(box[5].x+move,box[0].y-move,0), box[4], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		drawLine2(box[2], box[7], glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), material, scene, 0, glm::vec3(1.0f, 1.0f, 1.0f));
		






	}

}


void Renderer::Render(const Scene& scene)
{
	
	int i;
	for (i = 0; i < scene.GetModelCount() && scene.getDraw(); i++)
	{
		MeshModel model = scene.getModelAtIndex(i);
		std::vector<glm::vec3> box = model.getBox();
		drawModel(scene, model);	
	}
	int blur = scene.getBlur();
	for (int i = 0; i < blur; i++)
		blurScreen(colorBuffer,true);
	if (scene.getBloom())
		Bloom(scene, colorBuffer);
}
void Renderer::Bloom(const Scene& scene, float *buffer)
{
	int i, j, k;
	glm::vec3 color;

	
	float *temp = new float[3 * viewportWidth * viewportHeight];
	for (i = 0; i <= 3 * viewportWidth * viewportHeight; i++)
		temp[i] = 0;
	std::vector<PointLight> PointLight1= scene.getPointLights();
	PointLight tempParallel;
	glm::vec3 pos;
	for (k = 0; k < PointLight1.size(); k++)
	{
		tempParallel = PointLight1[k];
		pos = tempParallel.position;
		//pos.x = int(pos.x*viewportWidth);
	//	pos.y = int(pos.y*viewportHeight);
		color = tempParallel.color;
		for (i = pos.x - 5; i < pos.x + 5 && i < viewportWidth - 5 && i>0; i++)
			for (j = pos.y - 5; j < pos.y + 5 && j < viewportHeight - 5 && j>0; j++)
			{
				
				temp[(i + j * viewportWidth) * 3 + 0] = color.x;
				temp[(i + j * viewportWidth) * 3 + 1] = color.y;
				temp[(i + j * viewportWidth) * 3 + 2] = color.z;
			}
	}
	
	blurScreen(temp,false);
	blurScreen(temp,false);
	blurScreen(temp,false);
	float k1, k2;
	for (i = 0; i <= 3 * viewportWidth * viewportHeight; i++)
	{
		k1 = buffer[i];
		k2 = temp[i];
		buffer[i] = buffer[i] + temp[i];
	}
	for (i = 3; i < viewportWidth - 3; i++)
		for (j = 3; j < viewportHeight - 3; j++)
		{
			color.x = buffer[(i + j * viewportWidth) * 3 + 0];
			color.y = buffer[(i + j * viewportWidth) * 3 + 1];
			color.z = buffer[(i + j * viewportWidth) * 3 + 2];
			putPixel(i, j, color);
		}
//	delete[] temp;
}


void Renderer::blurScreen(float *buffer,bool print)
{
	float *temp = new float[3 * viewportWidth * viewportHeight];
	int i, j;
	float x1, x2, x3;
	float k1, k2, k3;
	for (i = 0; i <= 3 * viewportWidth * viewportHeight; i++)
		temp[i] = 0;
	glm::vec3 color;
	for(i=3; i<viewportWidth-3;i++)
		for (j = 3; j < viewportHeight - 3; j++)
		{
			k1 = buffer[(i + j * viewportWidth) * 3 + 0];
			x1 = buffer[(i + j * viewportWidth) * 3 + 0] * 0.2 + buffer[(i - 1 + j * viewportWidth) * 3 + 0] * 0.1 + buffer[(i+1 + j * viewportWidth) * 3 + 0] * 0.1;
			x2 = buffer[(i + (j+1) * viewportWidth) * 3 + 0] * 0.1 + buffer[(i - 1 + (j+1) * viewportWidth) * 3 + 0] * 0.1 + buffer[(i + 1 + (j+1) * viewportWidth) * 3 + 0] * 0.1;
			x3 = buffer[(i + (j -1) * viewportWidth) * 3 + 0] * 0.1 + buffer[(i - 1 + (j - 1) * viewportWidth) * 3 + 0] * 0.1 + buffer[(i + 1 + (j - 1) * viewportWidth) * 3 + 0] * 0.0275;
			temp[(i + j * viewportWidth) * 3 + 0] = x1 + x2 + x3;
			k2 = temp[(i + j * viewportWidth) * 3 + 0];
			x1 = buffer[(i + j * viewportWidth) * 3 + 1] * 0.2 + buffer[(i - 1 + j * viewportWidth) * 3 + 1] * 0.1 + buffer[(i + 1 + j * viewportWidth) * 3 + 1] * 0.1;
			x2 = buffer[(i + (j + 1) * viewportWidth) * 3 + 1] * 0.1 + buffer[(i - 1 + (j + 1) * viewportWidth) * 3 + 1] * 0.1 + buffer[(i + 1 + (j + 1) * viewportWidth) * 3 + 1] * 0.1;
			x3 = buffer[(i + (j - 1) * viewportWidth) * 3 + 1] * 0.1 + buffer[(i - 1 + (j - 1) * viewportWidth) * 3 + 1] * 0.1 + buffer[(i + 1 + (j - 1) * viewportWidth) * 3 + 1] * 0.1;
			temp[(i + j * viewportWidth) * 3 + 1] = x1 + x2 + x3;
			x1 = buffer[(i + j * viewportWidth) * 3 + 2] * 0.2+ buffer[(i - 1 + j * viewportWidth) * 3 + 2] * 0.1 + buffer[(i + 1 + j * viewportWidth) * 3 + 2] * 0.1;
			x2 = buffer[(i + (j + 1) * viewportWidth) * 3 + 2] * 0.1 + buffer[(i - 1 + (j + 1) * viewportWidth) * 3 + 2] * 0.1 + buffer[(i + 1 + (j + 1) * viewportWidth) * 3 + 2] * 0.1;
			x3 = buffer[(i + (j - 1) * viewportWidth) * 3 + 2] * 0.1 + buffer[(i - 1 + (j - 1) * viewportWidth) * 3 + 2] * 0.1 + buffer[(i + 1 + (j - 1) * viewportWidth) * 3 + 2] * 0.1;
			temp[(i + j * viewportWidth) * 3 + 2] = x1 + x2 + x3;

		}
	for (i = 3; i < viewportWidth - 3; i++)
		for (j = 3; j < viewportHeight - 3; j++)
		{
			buffer[(i + j * viewportWidth) * 3 + 0] = temp[(i + j * viewportWidth) * 3 + 0];
			buffer[(i + j * viewportWidth) * 3 + 1] = temp[(i + j * viewportWidth) * 3 + 1];
			buffer[(i + j * viewportWidth) * 3 + 2] = temp[(i + j * viewportWidth) * 3 + 2];
			if (print)
			{
				color.x = buffer[(i + j * viewportWidth) * 3 + 0];
				color.y = buffer[(i + j * viewportWidth) * 3 + 1];
				color.z = buffer[(i + j * viewportWidth) * 3 + 2];
				putPixel(i, j, color);
			}
		}
	//delete[] temp;
}

void Renderer::drawBox(std::vector<glm::vec3> box, MeshModel model, const Scene& scene, glm::vec3 color)
{
	int i, j;
	glm::mat4x4 modelMatrix = model.GetWorldTransformation();
	glm::vec3 v1 = modelMatrix*glm::vec4(0,1,100,1);
	glm::vec3 v2 = modelMatrix*glm::vec4(1, 1, 100,1);
	glm::vec3 v3 = modelMatrix*glm::vec4(1, 0, 100,1);
	glm::vec3 v4 = modelMatrix*glm::vec4(0, 0, 100,1);
	v1.x = int(v1.x*viewportWidth);
	v2.x = int(v2.x*viewportWidth);
	v3.x = int(v3.x*viewportWidth);
	v4.x = int(v4.x*viewportWidth);
	v1.y = int(v1.y*viewportWidth);
	v2.y = int(v2.y*viewportWidth);
	v3.y = int(v3.y*viewportWidth);
	v4.y = int(v4.y*viewportWidth);

	glm::vec3 temp= v1;
	for (; temp.x <= v2.x; temp.x++)
		putPixel(temp.x,temp.y,color);
	temp = v4;
	for (; temp.x <= v3.x; temp.x++)
		putPixel(temp.x, temp.y, color);

	temp = v4;
	for (; temp.y <= v1.y; temp.y++)
		putPixel(temp.x, temp.y, color);

	temp = v3;
	for (; temp.y <= v2.y; temp.y++)
		putPixel(temp.x, temp.y, color);

}
//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}





void Renderer::drawLine(const glm::vec3 point0, const glm::vec3 point1, const std::vector<glm::vec3>* points, const std::vector<glm::vec3>* normals, Material material, const Scene & scence, int shadingType, glm::vec3 color) const
{
	//if (std::abs(point0.x - point1.x) > 100)
		//return;

	if (point0.x == point1.x || point0.x < 0 || point1.x < 0 || point0.y < 0 || point1.y < 0)
		return;
	Camera camera = scence.GetActiveCamera();
	glm::mat4x4 projectionMatrix = camera.getProjectionTransformation();
	glm::mat4x4 viewTransformation = camera.getviewTransformation();
	glm::vec3 vertex, glVertex;
	glm::vec3 normal;
	float w1=1, w2=0, w3=0;
	glm::vec3 colorOfp0;
	glm::vec3 colorOfp1;
	glm::vec3 v1 = ((*points)[0]);
	glm::vec3 v2 = ((*points)[1]);
	glm::vec3 v3 = ((*points)[2]);
	glm::vec3 color1, color2, color3;
	
	// Bresenham's line algorithm
	const bool steep = (std::abs(point1.y - point0.y) > std::abs(point1.x - point0.x));
	glm::vec4 newColor;
	glm::vec3 temp_p0 = point0, temp_p1 = point1;
	if (point0.x < point1.x)
	{
	    temp_p0 = point1;
		temp_p1 = point0;
	}
	temp_p0.x = int(temp_p0.x);
	temp_p0.y = int(temp_p0.y);
	temp_p1.x = int(temp_p1.x);
	temp_p1.y = int(temp_p1.y);


	int LengthX = point1.x - point0.x;
	if (steep)
	{
		std::swap(temp_p0.x, temp_p0.y);
		std::swap(temp_p1.x, temp_p1.y);
	}
	


	if (temp_p0.x > temp_p1.x)
	{
		std::swap(temp_p0.x, temp_p1.x);
		std::swap(temp_p0.y, temp_p1.y);
	}

	const float dx = temp_p1.x - temp_p0.x;
	const float dy = fabs(temp_p1.y - temp_p0.y);

	float error = dx / 2.0f;
	const int ystep = (temp_p0.y < temp_p1.y) ? 1 : -1;
	int y = (int)temp_p0.y;

	const int maxX = (int)temp_p1.x;

	for (int x = (int)temp_p0.x; x < maxX; x++)
	{
		if (steep)
		{
			

 			//vertex.x = float(y) / float(X_width);
			//vertex.y = float(x) / float(Y_width);

			vertex.x = int(y);
			vertex.y = int(x);
			
			w1 = ((v2.y - v3.y)*(vertex.x - v3.x) + (v3.x - v2.x)*(vertex.y - v3.y)) / ((v2.y - v3.y)*(v1.x - v3.x) + (v3.x - v2.x)*(v1.y - v3.y));
			w2 = ((v3.y - v1.y)*(vertex.x - v3.x) + (v1.x - v3.x)*(vertex.y - v3.y)) / ((v2.y - v3.y)*(v1.x - v3.x) + (v3.x - v2.x)*(v1.y - v3.y));
			w3 = 1 - (w1 + w2);
			vertex.z = w1 * (*points)[0].z + w2 * (*points)[1].z + w3 * (*points)[2].z;
			vertex.z = point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX;
			//glVertex= projectionMatrix /**viewTransformation */ * glm::vec4(vertex,1);
			//glVertex.x = int(glVertex.x*X_width)-1;
			//glVertex.y = int(glVertex.y *Y_width)-1;
			glVertex = vertex;
			if (shadingType >= 0)
			{
				

				if (shadingType == 1)
				{
					color1= getcolor((*points)[0], (*normals)[0], material, scence);
					color2 = getcolor((*points)[1], (*normals)[1], material, scence);
					color3 = getcolor((*points)[2], (*normals)[2], material, scence);
					color = w1 * color1 + w2 * color2 + w3 * color3;
					
					//color = colorOfp0 * (point0.x - x) / float(LengthX) + colorOfp1 * (x - point1.x) / float(LengthX);
				}
				if (shadingType == 2)
				{
					
					//normal = normalP0 * (point0.x - x) / float(LengthX) + normalP1 * (x - point1.x) / float(LengthX);
					normal = w1 * (*normals)[0] + w2 * (*normals)[1] + w3 * (*normals)[2];
					color = getcolor(vertex, normal, material, scence);

				}
			}

			if (glVertex.x >= 0 && glVertex.x <= viewportWidth && glVertex.y >= 0 && glVertex.y <= viewportHeight)
			{
				if ((zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] <= glVertex.z) )
				{
					zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] = glVertex.z;
					putPixel(glVertex.x, glVertex.y, color);
				}
			}
		}
		else
		{
			

			//vertex.x = float(x) / float(X_width);
			//vertex.y = float(y) / float(Y_width);
			vertex.x = int(x);
			vertex.y = int(y);

			w1 = ((v2.y - v3.y)*(vertex.x - v3.x) + (v3.x - v2.x)*(vertex.y - v3.y)) / ((v2.y - v3.y)*(v1.x - v3.x) + (v3.x - v2.x)*(v1.y - v3.y));
			w2 = ((v3.y - v1.y)*(vertex.x - v3.x) + (v1.x - v3.x)*(vertex.y - v3.y)) / ((v2.y - v3.y)*(v1.x - v3.x) + (v3.x - v2.x)*(v1.y - v3.y));
			w3 = 1 - (w1 + w2);
			vertex.z = w1 * (*points)[0].z + w2 * (*points)[1].z + w3 * (*points)[2].z;
			vertex.z= point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX;
			vertex = glm::vec3(float(x ), float(y ), point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX);
			//glVertex = projectionMatrix*  /*viewTransformation*  */ glm::vec4(vertex,1);
			//glVertex.x = int(glVertex.x*X_width)-1;
			//glVertex.y = int(glVertex.y *Y_width)-1;
			glVertex = vertex;
			if (shadingType >= 0)
			{
				


				if (shadingType == 1)
				{
					color1 = getcolor((*points)[0], (*normals)[0], material, scence);
					color2 = getcolor((*points)[1], (*normals)[1], material, scence);
					color3 = getcolor((*points)[2], (*normals)[2], material, scence);
					color = w1 * color1 + w2 * color2 + w3 * color3;

					//color = colorOfp0 * (point0.x - x) / float(LengthX) + colorOfp1 * (x - point1.x) / float(LengthX);
				}
				if (shadingType == 2)
				{

					//normal = normalP0 * (point0.x - x) / float(LengthX) + normalP1 * (x - point1.x) / float(LengthX);
					normal = w1 * (*normals)[0] + w2 * (*normals)[1] + w3 * (*normals)[2];
					color = getcolor(vertex, normal, material, scence);


				}
			}

			if (glVertex.x >= 0 && glVertex.x <= viewportWidth && glVertex.y >= 0 && glVertex.y <= viewportHeight)
			{

				if ((zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] <= glVertex.z) );
				{
					zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] = glVertex.z;
					putPixel(glVertex.x, glVertex.y, color);
				}
			}
		}
		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}


}
void Renderer::drawLine2(const glm::vec3 point0, const glm::vec3 point1, const glm::vec3 normalP0, const glm::vec3 normalP1, Material material, const Scene &scence, int shadingType, glm::vec3 color) const
{
	//if (std::abs(point0.x - point1.x) > 100)
		//return;

	if ( point0.x < 0 || point1.x < 0 || point0.y < 0 || point1.y < 0)
		return;
	Camera camera = scence.GetActiveCamera();
	glm::mat4x4 projectionMatrix = camera.getProjectionTransformation();
	glm::mat4x4 viewTransformation = camera.getviewTransformation();
	glm::vec3 vertex, glVertex;
	glm::vec3 normal;

	glm::vec3 colorOfp0;
	glm::vec3 colorOfp1;
	if (shadingType == 1)
	{
		colorOfp0 = getcolor(point0, normalP0, material, scence);
		colorOfp1 = getcolor(point1, normalP1, material, scence);
	}
	// Bresenham's line algorithm
	const bool steep = (std::abs(point1.y - point0.y) > std::abs(point1.x - point0.x));
	glm::vec4 newColor;
	glm::vec3 temp_p0 = point0, temp_p1 = point1;
	if (point0.x < point1.x)
	{
		temp_p0 = point1;
		temp_p1 = point0;
	}
	temp_p0.x = int(temp_p0.x);
	temp_p0.y = int(temp_p0.y);
	temp_p1.x = int(temp_p1.x);
	temp_p1.y = int(temp_p1.y);


	int LengthX = point1.x - point0.x;
	if (point0.x == point1.x)
		LengthX = 0.5;
	if (steep)
	{
		std::swap(temp_p0.x, temp_p0.y);
		std::swap(temp_p1.x, temp_p1.y);
	}


	if (temp_p0.x > temp_p1.x)
	{
		std::swap(temp_p0.x, temp_p1.x);
		std::swap(temp_p0.y, temp_p1.y);
	}

	const float dx = temp_p1.x - temp_p0.x;
	const float dy = fabs(temp_p1.y - temp_p0.y);

	float error = dx / 2.0f;
	const int ystep = (temp_p0.y < temp_p1.y) ? 1 : -1;
	int y = (int)temp_p0.y;

	const int maxX = (int)temp_p1.x;

	for (int x = (int)temp_p0.x; x < maxX; x++)
	{
		if (steep)
		{
			vertex.x = float(y) / float(X_width);
			vertex.y = float(x) / float(Y_width);
			vertex.z = point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX;
			//vertex = glm::vec3(y / viewportWidth, x / viewportHeight, point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX);
			glVertex = glm::vec4(vertex, 1); //projectionMatrix * viewTransformation * glm::vec4(vertex, 1);
			glVertex.x = int(glVertex.x*X_width);
			glVertex.y = int(glVertex.y *Y_width);

			if (shadingType == 1)
			{
				color = colorOfp0 * (point0.x - x) / float(LengthX) + colorOfp1 * (x - point1.x) / float(LengthX);
			}
			if (shadingType == 2)
			{
				normal = normalP0 * (point0.x - x) / float(LengthX) + normalP1 * (x - point1.x) / float(LengthX);
				color = getcolor(vertex, normal, material, scence);

			}

			if (glVertex.x >= 0 && glVertex.x <= viewportWidth && glVertex.y >= 0 && glVertex.y <= viewportHeight)
				if (zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] <= glVertex.z)
				{
					zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] = glVertex.z;
					putPixel(glVertex.x, glVertex.y, color);
				}
		}
		else
		{
			vertex.x = float(x) / float(X_width);
			vertex.y = float(y) / float(X_width);
			vertex.z = point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX;
			//vertex = glm::vec3(float(x / viewportWidth), float(y / viewportHeight), point0.z *(point0.x - x) / LengthX + point1.z *(x - point1.x) / LengthX);
			glVertex = glm::vec4(vertex, 1); //projectionMatrix * viewTransformation * glm::vec4(vertex, 1);
			glVertex.x = int(glVertex.x*X_width);
			glVertex.y = int(glVertex.y *X_width);
			if (shadingType == 1)
			{
				color = colorOfp0 * (point1.x - x) / float(LengthX) + colorOfp1 * (x - point0.x) / float(LengthX);
			}
			if (shadingType == 2)
			{
				normal = normalP1 * (point1.x - x) / float(LengthX) + normalP0 * (x - point0.x) / float(LengthX);
				color = getcolor(vertex, normal, material, scence);

			}
			if (glVertex.x >= 0 && glVertex.x <= viewportWidth && glVertex.y >= 0 && glVertex.y <= viewportHeight)
				if (zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] <= glVertex.z)
				{
					zBuffer[int((glVertex.x) + (glVertex.y)*(viewportWidth))] = glVertex.z;
					putPixel(glVertex.x, glVertex.y, color);
				}
		}
		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}


}

void Renderer::fillBottomFlatTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 normalP1, glm::vec3 normalP2, glm::vec3 normalP3, const Scene & scene, Material material, bool drawFacesNormal, int shadingType)
{
	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

	glm::vec3 temp1 = v1;
	glm::vec3 temp2 = v1;
	temp1.y--;
	temp2.y--;
	glm::vec3  normal1;
	glm::vec3  normal2;
	glm::vec3 color = getcolor(v1, normalP1, material, scene);
	
	for (; temp1.y >= v2.y; temp1.y--,temp2.y--)
	{
		normal1 = normalP1 * (v1.y - temp1.y) / (v1.y - v2.y) + normalP2 * (temp1.y-v2.y) / (v1.y-v2.y);
		normal2 = normalP1 * (v1.y - temp1.y) / (v1.y - v2.y) + normalP3 * (temp1.y - v2.y) / (v1.y - v2.y);
		temp1.z= v1.z * (v1.y - temp1.y) / (v1.y - v2.y) + v2.z * (temp1.y - v2.y) / (v1.y - v2.y);
		temp2.z= v1.z * (v1.y - temp1.y) / (v1.y - v2.y) + v3.z * (temp1.y - v2.y) / (v1.y - v2.y);
		//drawLine(temp1,temp2,normal1,normal2,material,scene,shadingType,color);
		//drawLine(temp1,temp2,normal1,normal2,material,scene,shadingType,color);
		temp1.x =temp1.x+ invslope1;
		temp2.x = temp2.x + invslope2;
	}






}

void Renderer::fillTopFlatTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 normalP1, glm::vec3 normalP2, glm::vec3 normalP3, const Scene & scene, Material material, bool drawFacesNormal, int shadingType)
{
	float invslope1 = (v1.x - v3.x) / (v1.y - v3.y);
	float invslope2 = (v1.x - v2.x) / (v1.y - v2.y);


	glm::vec3 temp1 = v1;
	glm::vec3 temp2 = v1;
	glm::vec3  normal1;
	glm::vec3  normal2;
	glm::vec3 color = getcolor(v1, normalP1, material, scene);



	for (; temp1.y <= v2.y; temp1.y++, temp2.y++)
	{
		normal1 = normalP1 * (v1.y - temp1.y) / (v1.y - v2.y) + normalP2 * (temp1.y - v2.y) / (v1.y - v2.y);
		normal2 = normalP1 * (v1.y - temp1.y) / (v1.y - v2.y) + normalP3 * (temp1.y - v2.y) / (v1.y - v2.y);
		temp1.z = v1.z * (v1.y - temp1.y) / (v1.y - v2.y) + v2.z * (temp1.y - v2.y) / (v1.y - v2.y);
		temp2.z = v1.z * (v1.y - temp1.y) / (v1.y - v2.y) + v3.z * (temp1.y - v2.y) / (v1.y - v2.y);
		//drawLine(temp1, temp2, normal1, normal2, material, scene, shadingType, color);
		temp1.x = temp1.x + invslope1;
		temp2.x = temp2.x + invslope2;
	}




}





void Renderer::drawTriangles(const std::vector<glm::vec3>* points, const std::vector<glm::vec3>* normals,  const Scene & scene, const MeshModel &model,glm::mat3x2 scaleMatrix)
{
	Camera camera = scene.GetActiveCamera();		// Data needed to draw the model
	glm::mat4x4 modelViewMatrix = model.GetWorldTransformation();
	glm::mat4x4 projectionMatrix = camera.getProjectionTransformation();
		glm::mat4x4 viewTransformation = camera.getviewTransformation();
	Material material = model.getMaterial();
	int shadingType = scene.getshadingType(); 
	glm::vec3 v1, v2, v3,temp, v4, S, E;
	glm::vec3 normal1, normal2;
	glm::vec3 normalP1, normalP2, normalP3;
	float dx1, dx2, dx3;
	glm::vec3 color;
	glm::vec4 temp2;
	float zCoordinate1, zCoordinate2;
	v1 = (*points)[0];
	v2 = (*points)[1];
	v3 = (*points)[2];
	normalP1 = (*normals)[0];
	normalP2 = (*normals)[1];
	normalP3 = (*normals)[2];
	color = getcolor(v1, normalP1, material, scene);

	temp2 = projectionMatrix * viewTransformation*modelViewMatrix * glm::vec4(v1, 1);
	v1.x = temp2.x / temp2[3];
	v1.y = temp2.y / temp2[3];
	v1.z = temp2.z / temp2[3];
	temp2 = projectionMatrix * viewTransformation*modelViewMatrix * glm::vec4(v2, 1);
	v2.x = temp2.x / temp2[3];
	v2.y = temp2.y / temp2[3];
	v2.z = temp2.z / temp2[3];
	temp2 = projectionMatrix * viewTransformation*modelViewMatrix * glm::vec4(v3, 1);
	//temp2 =  viewTransformation*modelViewMatrix * glm::vec4(v3, 1);
	v3.x = temp2.x / temp2[3];
	v3.y = temp2.y / temp2[3];
	v3.z = temp2.z / temp2[3];

	if (v1.y > v2.y)
	{
		temp = v1;
		v1 = v2;
		v2 = temp;

		temp = normalP1;
		normalP1 = normalP2;
		normalP2 = temp;
	}
	if (v2.y > v3.y)
	{
		temp = v2;
		v2 = v3;
		v3 = temp;

		temp = normalP2;
		normalP2 = normalP3;
		normalP3 = temp;
	}
	if (v1.y > v2.y)
	{
		temp = v1;
		v1 = v2;
		v2 = temp;

		temp = normalP1;
		normalP1 = normalP2;
		normalP2 = temp;
	}
	

	//v1 = ReScale(v1, scaleMatrix);
	//v2 = ReScale(v2, scaleMatrix);
	//v3 = ReScale(v3, scaleMatrix);
	v1 = Scale(v1);
	v2 = Scale(v2);
	v3 = Scale(v3);


	//drawLine(v1, v2, points, normals, material, scene, shadingType, color);
	//drawLine(v2, v3, points, normals, material, scene, shadingType, color);
	//drawLine(v1, v3, points, normals, material, scene, shadingType, color);


	
	

	if (v2.y - v1.y > 0)
		dx1 = (v2.x - v1.x) / (v2.y - v1.y);
	else
		dx1 = 0;
	if (v3.y - v1.y > 0)
		dx2 = (v3.x - v1.x) / (v3.y - v1.y);
	else 
		dx2 = 0;
	if (v3.y - v2.y > 0)
		dx3 = (v3.x - v2.x) / (v3.y - v2.y);
	else
		dx3 = 0;

	S = v1;
	E = v1;
	if (dx1 > dx2) {
		for (; S.y <= v2.y; S.y++, E.y++, S.x += dx2, E.x += dx1)
		{
			if (S.y >= v2.y)
			{
				normal1 = (normalP3 * (v3.y - S.y) / (v3.y - v2.y)) + (normalP2*((S.y - v2.y) / (v3.y - v2.y)));
				zCoordinate1 = (v3.z * (v3.y - S.y) / (v3.y - v2.y)) + (v2.z*((S.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal1 = (normalP2 * (v2.y - S.y) / (v2.y - v1.y)) + (normalP1 * ((S.y - v1.y) / (v2.y - v1.y)));
				zCoordinate1 = (v2.z * (v2.y - S.y) / (v2.y - v1.y)) + (v1.z * ((S.y - v1.y) / (v2.y - v1.y)));
			}
			if (E.y >= v2.y) {
				normal2 = (normalP3 * (v3.y - E.y) / (v3.y - v2.y)) + (normalP2*((E.y - v2.y) / (v3.y - v2.y)));
				zCoordinate2 = (v3.z * (v3.y - E.y) / (v3.y - v2.y)) + (v2.z*((E.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal2 = (normalP2 * (v2.y - E.y) / (v2.y - v1.y)) + (normalP1 * ((E.y - v1.y) / (v2.y - v1.y)));
				zCoordinate2 = (v2.z * (v2.y - E.y) / (v2.y - v1.y)) + (v1.z * ((E.y - v1.y) / (v2.y - v1.y)));

			}
			drawLine(glm::vec3(int(S.x) , int(S.y) , zCoordinate1), glm::vec3(int(E.x) , int(S.y) , zCoordinate2),points,normals, material, scene, shadingType, color);
			//drawLine(glm::vec3(int(S.x) , int(S.y) , zCoordinate1), glm::vec3(int(E.x) , int(S.y) , zCoordinate2), normal1, normal2, material, scene, shadingType, color);


		}
		E = v2;
		for (; S.y <= v3.y; S.y++, E.y++, S.x += dx2, E.x += dx3)
		{
			if (S.y >= v2.y)
			{
				normal1 = (normalP3 * (v3.y - S.y) / (v3.y - v2.y)) + (normalP2*((S.y - v2.y) / (v3.y - v2.y)));
				zCoordinate1 = (v3.z * (v3.y - S.y) / (v3.y - v2.y)) + (v2.z*((S.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal1 = (normalP2 * (v2.y - S.y) / (v2.y - v1.y)) + (normalP1 * ((S.y - v1.y) / (v2.y - v1.y)));
				zCoordinate1 = (v2.z * (v2.y - S.y) / (v2.y - v1.y)) + (v1.z * ((S.y - v1.y) / (v2.y - v1.y)));
			}
			if (E.y >= v2.y) {
				normal2 = (normalP3 * (v3.y - E.y) / (v3.y - v2.y)) + (normalP2*((E.y - v2.y) / (v3.y - v2.y)));
				zCoordinate2 = (v3.z * (v3.y - E.y) / (v3.y - v2.y)) + (v2.z*((E.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal2 = (normalP2 * (v2.y - E.y) / (v2.y - v1.y)) + (normalP1 * ((E.y - v1.y) / (v2.y - v1.y)));
				zCoordinate2 = (v2.z * (v2.y - E.y) / (v2.y - v1.y)) + (v1.z * ((E.y - v1.y) / (v2.y - v1.y)));

			}
			drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2),points,normals, material, scene, shadingType, color);
			//drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2), normal1, normal2, material, scene, shadingType, color);

		}
	}
	else {
		for (; S.y <= v2.y; S.y++, E.y++, S.x += dx1, E.x += dx2)
		{
			if (S.y >= v2.y)
			{
				normal1 = (normalP3 * (v3.y - S.y) / (v3.y - v2.y)) + (normalP2*((S.y - v2.y) / (v3.y - v2.y)));
				zCoordinate1 = (v3.z * (v3.y - S.y) / (v3.y - v2.y)) + (v2.z*((S.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal1 = (normalP2 * (v2.y - S.y) / (v2.y - v1.y)) + (normalP1 * ((S.y - v1.y) / (v2.y - v1.y)));
				zCoordinate1 = (v2.z * (v2.y - S.y) / (v2.y - v1.y)) + (v1.z * ((S.y - v1.y) / (v2.y - v1.y)));
			}
			if (E.y >= v2.y) {
				normal2 = (normalP3 * (v3.y - E.y) / (v3.y - v2.y)) + (normalP2*((E.y - v2.y) / (v3.y - v2.y)));
				zCoordinate2 = (v3.z * (v3.y - E.y) / (v3.y - v2.y)) + (v2.z*((E.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal2 = (normalP2 * (v2.y - E.y) / (v2.y - v1.y)) + (normalP1 * ((E.y - v1.y) / (v2.y - v1.y)));
				zCoordinate2 = (v2.z * (v2.y - E.y) / (v2.y - v1.y)) + (v1.z * ((E.y - v1.y) / (v2.y - v1.y)));

			}
			drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2), points, normals, material, scene, shadingType, color);
			//drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2), normal1, normal2, material, scene, shadingType, color);

		}	
		S = v2;
		for (; S.y <= v3.y; S.y++, E.y++, S.x += dx3, E.x += dx2)
		{
			if (S.y >= v2.y)
			{
				normal1 = (normalP3 * (v3.y - S.y) / (v3.y - v2.y)) + (normalP2*((S.y - v2.y) / (v3.y - v2.y)));
				zCoordinate1 = (v3.z * (v3.y - S.y) / (v3.y - v2.y)) + (v2.z*((S.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal1 = (normalP2 * (v2.y - S.y) / (v2.y - v1.y)) + (normalP1 * ((S.y - v1.y) / (v2.y - v1.y)));
				zCoordinate1 = (v2.z * (v2.y - S.y) / (v2.y - v1.y)) + (v1.z * ((S.y - v1.y) / (v2.y - v1.y)));
			}
			if (E.y >= v2.y) {
				normal2 = (normalP3 * (v3.y - E.y) / (v3.y - v2.y)) + (normalP2*((E.y - v2.y) / (v3.y - v2.y)));
				zCoordinate2 = (v3.z * (v3.y - E.y) / (v3.y - v2.y)) + (v2.z*((E.y - v2.y) / (v3.y - v2.y)));
			}
			else {
				normal2 = (normalP2 * (v2.y - E.y) / (v2.y - v1.y)) + (normalP1 * ((E.y - v1.y) / (v2.y - v1.y)));
				zCoordinate2 = (v2.z * (v2.y - E.y) / (v2.y - v1.y)) + (v1.z * ((E.y - v1.y) / (v2.y - v1.y)));

			}
			drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2), points, normals, material, scene, shadingType, color);
			//drawLine(glm::vec3(int(S.x), int(S.y), zCoordinate1), glm::vec3(int(E.x), int(S.y), zCoordinate2), normal1, normal2, material, scene, shadingType, color);

		}

	}
	










	
}





int	Renderer::ReScaleX(float num,  glm::mat3x2 scaleMatrix)
{
	return (((num- scaleMatrix[0][1]) / (scaleMatrix[0][0] - scaleMatrix[0][1])));
	
}

int	Renderer::ReScaleY(float num , glm::mat3x2 scaleMatrix)
{
	return (((num - scaleMatrix[1][1]) / (scaleMatrix[1][0] - scaleMatrix[1][1])));
}

int	Renderer::ReScaleZ(float num, glm::mat3x2 scaleMatrix)
{
	return (((num - scaleMatrix[2][1]) / (scaleMatrix[2][0] - scaleMatrix[2][1])));
}

glm::vec3	Renderer::ReScale(glm::vec3& v, glm::mat3x2 scaleMatrix)
{
	//v.x = (((v.x - scaleMatrix[0][1]) / (scaleMatrix[0][0] - scaleMatrix[0][1])));
	//v.y = (((v.y - scaleMatrix[1][1]) / (scaleMatrix[1][0] - scaleMatrix[1][1])));
	//v.z = (((v.z - scaleMatrix[2][1]) / (scaleMatrix[2][0] - scaleMatrix[2][1])));
	return v;
}

glm::vec3 Renderer::Scale(glm::vec3& v)
{
	v.x = int((v.x*X_width)/2 + viewportWidth/2);
	v.y = int((v.y*Y_width)/2 + viewportHeight/2);
	return v;
}









glm::vec3 Renderer::getcolor(const glm::vec3 vertex, const glm::vec3 normal, Material material, const Scene & scene) const
{
	int i;
	glm::vec3 color=glm::vec3(0,0,0);
	std::vector<ParallelLight> parallelLights = scene.getParallelLights();
	for (i = 0; i < parallelLights.size(); i++)
		color = color + calculateParallelLight(vertex, normal, material, parallelLights[i]);
	color = color / float(parallelLights.size());
	std::vector<PointLight> pointLights = scene.getPointLights();
	for ( i = 0; i < pointLights.size(); i++)
		color = color + calculatepointLight(vertex, normal, material, pointLights[i]);
	return color;
	

}

glm::vec3 Renderer::calculateParallelLight(const glm::vec3 vertex, const glm::vec3 normal, Material material, ParallelLight light) const
{
	Camera camera;
	glm::vec3 eye= camera.getEye();
	glm::vec3 surfaceToCamera = normalize(eye - vertex);
	glm::vec3 surfaceToLight = normalize(light.direction);
	//a,bient
	glm::vec3 ambient = light.ambientCoefficient * material.ambient * light.color ;
	//diffuse
	float diffuseCoefficient = std::max(float(0.0), dot(normal, surfaceToLight));
	glm::vec3 diffuse = diffuseCoefficient * material.diffuse * light.color;
	//specular
	float specularCoefficient = 0.0;
	if (diffuseCoefficient > 0.0)
		specularCoefficient = pow(std::max(float(0.0), dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
	glm::vec3 specular = specularCoefficient * material.specular *light.color;
		
	return (ambient + diffuse + specular) *light.attenuation;
}

glm::vec3 Renderer::calculatepointLight(const glm::vec3 vertex, const glm::vec3 normal, Material material, PointLight light) const
{
	Camera camera;
	glm::vec3 eye = camera.getEye();
	glm::vec3 surfaceToCamera = normalize(eye - vertex);
	glm::vec3 surfaceToLight = normalize(glm::vec3(light.position) - vertex);
	float distanceToLight = length(glm::vec3(light.position) - vertex);
	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
	float lightToSurfaceAngle = (acos(dot(-surfaceToLight, normalize(light.coneDirection))))*(180 / 3.14);
	if (lightToSurfaceAngle > light.coneAngle) 
		attenuation = 0.0;


		//diffuse
		float diffuseCoefficient = std::max(float(0.0), dot(normal, surfaceToLight));
		glm::vec3 diffuse = diffuseCoefficient * material.diffuse*light.color;

		float specularCoefficient = 0.0;
		if (diffuseCoefficient > 0.0)
			specularCoefficient = pow(std::max(float(0.0), dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
		glm::vec3 specular = specularCoefficient * material.specular*light.color;

		return attenuation * (diffuse + specular);

	
}