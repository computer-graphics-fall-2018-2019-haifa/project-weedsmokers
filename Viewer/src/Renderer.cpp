#define _USE_MATH_DEFINES
#include <iostream>
#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
}

Renderer::~Renderer()
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}
}

void Renderer::putPixel(int i, int j, const glm::vec3& color)
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
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f));
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

void Renderer::drawActiveModel(const Scene & scene, bool drawNormals,bool drawFacesNormals,bool Box)
{
	glm::vec4 p1, p2,p3;
	int i,j,k;
	if (scene.GetModelCount() > 0)
	{
		MeshModel model = scene.GetActiveModel();
		glm::mat4x4 A = model.GetWorldTransformation();	
		Camera camera;
		if (scene.GetCameraCount())
			camera = scene.GetActiveCamera();
		glm::mat4x4 C = camera.getviewTransformation();
		for (i = 0; i < model.faces.size(); i++)
		{
			std::vector<glm::vec3> points;
			std::vector<glm::vec3> normals;

			glm::vec3 p = model.vertices[(model.faces[i].GetVertexIndex(0)) - 1];
			glm::vec4 new_p = { p.x,p.y,p.z,1};
			 A = model.GetWorldTransformation();
			glm::vec4 transformed_p = { 0,0,0,0 };
			transformed_p = C*A * new_p;

			points.push_back(transformed_p);
			transformed_p = { 0,0,0,0 };
			p = model.vertices[(model.faces[i].GetVertexIndex(1)) - 1];
			new_p = { p.x,p.y,p.z,1 };
			
			transformed_p =  C*A * new_p;
			points.push_back(transformed_p);
			transformed_p = { 0,0,0,0 };
			p = model.vertices[(model.faces[i].GetVertexIndex(2)) - 1];
			new_p = { p.x,p.y,p.z,1 };
			transformed_p =C* A * new_p;
			points.push_back(transformed_p);
			drawTriangles(&points, drawFacesNormals);
		}

		
		if (drawNormals)
		{
			for (i = 0; i < model.faces.size(); i++)
				for (j = 0; j < model.faces[i].getNormalsNum(); j++)
				{
					int k = model.faces[i].GetNormalIndex(j);
					int t = model.faces[i].GetVertexIndex(j);
					p1 = { model.normals[model.faces[i].GetNormalIndex(j) - 1],1 };
					p2 = { model.vertices[model.faces[i].GetVertexIndex(j) - 1],1 };

					p1.x /= viewportWidth;
					p1.y /= viewportHeight;
					p3 = p1 + p2;
					drawLine((A*p3), (A*p2));
				}
		}
		if (Box)
		{
			glm::vec4 p1 = { 0,1,1,1 };
			glm::vec4 p2 = {1,1,1,1};
			p1 = A * p1;
			p2 = A * p2;
			drawLine(p1, p2);
			p1 = { 1,1,1,1 };
			p2 = { 1,0,1,1 };
			p1 = A * p1;
			p2 = A * p2;
			drawLine(p1, p2);
			p1 = { 1,0,1,1 };
			p2 = { 0,0,1,1 };
			p1 = A * p1;
			p2 = A * p2;
			drawLine(p1, p2);
			p1 = { 0, 0, 1, 1 };
			p2 = { 0,1,1,1 };
			p1 = A * p1;
			p2 = A * p2;
			drawLine(p1, p2);

		}
	}
}

void Renderer::Render(const Scene& scene)
{
	


	
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################

	// Draw a chess board in the middle of the screen
	/*for (int i = 100; i < viewportWidth - 100; i++)
	{
		for (int j = 100; j < viewportHeight - 100; j++)
		{
			int mod_i = i / 50;
			int mod_j = j / 50;

			int odd = (mod_i + mod_j) % 2;
			if (odd)
			{
				putPixel(i, j, glm::vec3(0, 1, 0));
			}
			else
			{
				putPixel(i, j, glm::vec3(1, 0, 0));
			}
		}
	}*/
	drawActiveModel(scene, scene.getdrawNormals(),scene.getdrawFacesNormals(),scene.getdrawBox());
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


void Renderer::drawLine(const glm::vec2& p0, const glm::vec2& p1)
{
	
	
/*	if (std::abs(p1.y - p0.y)  <= std::abs(p1.x - p0.x))
	{ // line slope is less than 1
		if (p0.x > p1.x)
			drawLineLow(p1, p0);
		else
			drawLineLow(p0, p1);
	}
	else
	{	// line slope is greater than 1
		if(p0.y > p1.y)
			drawLineHight(p1, p0);
		else
			drawLineHight(p0, p1);

	}*/


	// Bresenham's line algorithm
	const bool steep = (std::abs(p1.y - p0.y) > std::abs(p1.x - p0.x));
	glm::vec3 color;
	color = { 100,100,100 };
	glm::vec2 temp_p0 = p0, temp_p1 = p1;
	temp_p0.x = ReScaleX(p0.x);
	temp_p0.y = ReScaleY(p0.y);
	temp_p1.x = ReScaleX(p1.x);
	temp_p1.y = ReScaleY(p1.y);
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
			putPixel(y, x, color);
		}
		else
		{
			putPixel(x, y, color);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
	
	


}

void Renderer::drawLineLow(const glm::vec2& p0, const glm::vec2& p1)
{
	
	glm::vec2 temp_p0=p0, temp_p1=p1;
	temp_p0.x = ReScaleX(p0.x);
	temp_p0.y = ReScaleY(p0.y);
	temp_p1.x = ReScaleX(p1.x);
	temp_p1.y = ReScaleY(p1.y);
	//std::cout << " p0:" << temp_p0.x << "     " << temp_p0.y << std::endl;
	glm::vec3 color;
	int D,x,y,sign = 1;
	glm::vec2 temp;
	temp = temp_p1 - temp_p0;
	color.x = 0;
	color.y = 0;
	color.z = 0;

	if (temp.y < 0)
	{
		sign = -1;
		temp.y = temp.y * (-1);
	}

	D = 2 * temp.y - temp.x;
	y = temp_p0.y;

	for (x = temp_p0.x; x <= temp_p1.x; x++)
	{
		
		putPixel(x, y,color );
			if (D > 0)
			{
				y = y + sign;
				D = D - 2 * temp.x;
			}

		D = D + 2 * temp.y;
	}

}

void Renderer::drawLineHight(const glm::vec2& p0, const glm::vec2& p1)
{
	
	glm::vec3 color;
	glm::vec2 temp_p0, temp_p1;
	temp_p0.x = ReScaleX(p0.x);
	temp_p0.y = ReScaleY(p0.y);
	temp_p1.x = ReScaleX(p1.x);
	temp_p1.y = ReScaleY(p1.y);
	std::cout << " p0:" << temp_p0.x << "     " << temp_p0.y << std::endl;
	std::cout << " p1:" << temp_p1.x << "     " << temp_p1.y << std::endl;
	

	int D, x, y, sign = 1;
	glm::vec2 temp;
	temp = temp_p1 - temp_p0;
	color.x = 200;
	color.y = 0;
	color.z =  0;

	if (temp.x < 0)
	{
		sign = -1;
		temp.x = temp.x * (-1);
	}

	D = 2 * temp.x - temp.y;
	x = temp_p0.x;
	for (y = temp_p0.y; y <= temp_p1.y; y++)
	{
		std::cout << "x=" << x << " y=" << y << std::endl;
		putPixel(x, y, color);
		if (D > 0)
		{
			x = x + sign;
			D = D - 2 * temp.y;
		}

		D = D + 2 * temp.x;
	}
}

void Renderer::drawTriangles(const std::vector<glm::vec3>* points, bool drawFacesNormals)
{
	glm::vec3* temp;
	glm::vec3 v1, v2, v3;
	int i;
	for (i = 0; i < points->size(); i=i+3)
	{
		v1 = (*points)[i];
		v2 = (*points)[i+1];
		v3 = (*points)[i+2];
		//std::cout << "p1:" << v1.x << "   "  << v1.y << "     " <<  v1.z << std::endl;
		//std::cout << "p2:" << v2.x << "   " << v2.y << "     " << v2.z << std::endl;
		/*if (v1.z != 0)
			v1 = v1 / v1.z;
		if (v2.z != 0)
			v2 = v2 / v2.z;
		if (v3.z != 0)
			v3 = v3 / v3.z;*/

		drawLine(v1, v2);
		drawLine(v2, v3);
		drawLine(v1, v3);
		if (drawFacesNormals)
		{
			glm::vec3 Dir = cross((v2 - v1), (v3 - v1));
			Dir = normalize(Dir);
			glm::vec3 center = { (v1[0] + v2[0] + v3[0]) / 3,(v1[1] + v2[1] + v3[1]) / 3,(v1[2] + v2[2] + v3[2]) / 3 };
			Dir.x /= viewportWidth;
			Dir.y /= viewportHeight;
			drawLine(center, center + Dir);

		}


	}
}



int	Renderer::ReScaleX(float num)
{
	return num * viewportWidth ;
}

int	Renderer::ReScaleY(float num)
{
	return num * viewportHeight;
}


void Renderer::putPixel2(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;

	//std::cout << "i=" << i << " j=" << j << std::endl;
	colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
	colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
	colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
}

