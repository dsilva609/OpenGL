#include <iostream>
#include <cstdlib>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "ObjectParser.cpp"

using namespace glm;
using namespace std;

#define WINDOW_TITLE_PREFIX "Project 2 - Field of Cows"

static int currentWidth;
static int currentHeight;
static unsigned frameCount;
static int dataSize;
static int numVertices;
static int fencePostVerts;
static int fieldVerts;
static int fenceBeamVerts;
static int fenceRotatedVerts;
static float rotationAngle;

static GLuint programID;
static GLuint MatrixID;
static GLuint VertexArrayID[5];
static GLuint offsetsBufferID[5];
static GLuint vertexbuffer[5];
static GLuint colorbuffer[5];

static mat4 MVP;
static mat4 Projection;
static mat4 View;
static mat4 Model;

class GLInitializer
{
public:
	void Initialize(int argCount, char* argValues[], int windowWidth, int windowHeight, string objectFilename, const char* vertexShaderFilename, const char* fragmentShaderFilename)
	{
		GLenum glewInitResult;

		currentWidth = windowWidth;
		currentHeight = windowHeight;

		InitWindow(argCount, argValues);

		glewExperimental = GL_TRUE;

		glewInitResult = glewInit();

		if (glewInitResult != GLEW_OK)
		{
			cerr << "ERROR: " << glewGetErrorString(glewInitResult) << endl;

			exit(EXIT_FAILURE);
		}

		cout << "OpenGL Version : " << glGetString(GL_VERSION) << endl;

		CreateVAO(objectFilename, vertexShaderFilename, fragmentShaderFilename);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

		glutMainLoop();
	}
private:
	int _windowHandle = 0;

	void InitWindow(int &arcCount, char* argValues[])
	{
		glutInit(&arcCount, argValues);

		glutInitContextVersion(4, 2);
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
		glutInitContextProfile(GLUT_CORE_PROFILE);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

		glutInitWindowSize(currentWidth, currentHeight);
		glutInitWindowPosition(0, 0);

		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

		this->_windowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

		if (this->_windowHandle < 1)
		{
			cerr << "ERROR: Could not create a new rendering window" << endl;
			exit(EXIT_FAILURE);
		}

		glutReshapeFunc(ResizeFunction);
		glutDisplayFunc(RenderFunction);
		glutIdleFunc(IdleFunction);
		glutTimerFunc(0, TimerFunction, 0);
	}

	static void ResizeFunction(int width, int height)
	{
		currentWidth = width;
		currentHeight = height;
		glViewport(0, 0, currentWidth, currentHeight);
	}

	static void RenderFunction(void)
	{
		frameCount++;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Projection matrix : 90° Field of View, 16:9 ratio, display range : 0.1 unit <-> 150 units
		Projection = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 200.0f);
		// Camera matrix
		View = glm::lookAt(
			glm::vec3(100, 3, 0), // Camera is at (x, y, z), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);


		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);

		//3rd attribute buffer : offsets
		glEnableVertexAttribArray(2);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(VertexArrayID[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);

		// Draw the Cows
		glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices / 3, 5);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(VertexArrayID[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
		//draw ico fence
		glDrawArraysInstanced(GL_TRIANGLES, 0, fencePostVerts / 3, 52);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		//draw field
		glBindVertexArray(VertexArrayID[2]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
		glDrawArrays(GL_TRIANGLES, 0, fieldVerts / 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		//draw fence
		glBindVertexArray(VertexArrayID[3]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, fenceBeamVerts / 3, 2);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		//draw fence
		glBindVertexArray(VertexArrayID[4]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[4]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, fenceRotatedVerts / 3, 2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_3D);


		glutSwapBuffers();
		glutPostRedisplay();

		glFlush();
	}

	static void IdleFunction(void)
	{
		float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;  // X° per second
		glm::vec3 axis(0.0, 1.0, 0.0);
		glm::mat4 Rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

		MVP = Projection * View * Model *Rotation;
		glutPostRedisplay();
	}

	static void TimerFunction(int value)
	{
		char* title = (char*)malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		if (value != 0)
		{
			sprintf(
				title,
				"%s: %d Frames Per Second @ %d x %d",
				WINDOW_TITLE_PREFIX,
				frameCount * 4,
				currentWidth,
				currentHeight
				);
		}
		glutSetWindowTitle(title);
		delete title;
		rotationAngle += 1.0f;
		frameCount = 0;
		glutTimerFunc(250, TimerFunction, 1);
	}

	void CreateVAO(string objectFilename, const char*  vertexShaderFilename, const char* fragmentShaderFilename)
	{
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		//VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID[0]);
		glBindVertexArray(VertexArrayID[0]);

		// Create and compile our GLSL program from the shaders
		programID = LoadShaders(vertexShaderFilename, fragmentShaderFilename);

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation(programID, "MVP");

		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		ObjectParser parser;

		vector <GLfloat> data = parser.Execute(objectFilename);
		numVertices = data.size();
		dataSize = numVertices * sizeof(GLfloat);

		glGenBuffers(1, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		data.clear();

		cout << dataSize << endl;
		cout << numVertices << endl;

		static const GLfloat g_color_buffer_data[] = {
			0.583f, 0.771f, 0.014f,
			0.609f, 0.115f, 0.436f,
			0.327f, 0.483f, 0.844f,

			0.822f, 0.569f, 0.201f,
			0.435f, 0.602f, 0.223f,
			0.310f, 0.747f, 0.185f,

			0.597f, 0.770f, 0.761f,
			0.559f, 0.436f, 0.730f,
			0.359f, 0.583f, 0.152f,

			0.483f, 0.596f, 0.789f,
			0.559f, 0.861f, 0.639f,
			0.195f, 0.548f, 0.859f,

			0.014f, 0.184f, 0.576f,
			0.771f, 0.328f, 0.970f,
			0.406f, 0.615f, 0.116f,

			0.676f, 0.977f, 0.133f,
			0.971f, 0.572f, 0.833f,
			0.140f, 0.616f, 0.489f,

			0.997f, 0.513f, 0.064f,
			0.945f, 0.719f, 0.592f,
			0.543f, 0.021f, 0.978f,

			0.279f, 0.317f, 0.505f,
			0.167f, 0.620f, 0.077f,
			0.347f, 0.857f, 0.137f,

			0.055f, 0.953f, 0.042f,
			0.714f, 0.505f, 0.345f,
			0.783f, 0.290f, 0.734f,

			0.722f, 0.645f, 0.174f,
			0.302f, 0.455f, 0.848f,
			0.225f, 0.587f, 0.040f,

			0.517f, 0.713f, 0.338f,
			0.053f, 0.959f, 0.120f,
			0.393f, 0.621f, 0.362f,

			0.673f, 0.211f, 0.457f,
			0.820f, 0.883f, 0.371f,
			0.982f, 0.099f, 0.879f,

			0.014f, 0.184f, 0.576f,
			0.771f, 0.328f, 0.970f,
			0.406f, 0.615f, 0.116f,

			0.676f, 0.977f, 0.133f,
			0.971f, 0.572f, 0.833f,
			0.140f, 0.616f, 0.489f,

			0.997f, 0.513f, 0.064f,
			0.945f, 0.719f, 0.592f,
			0.543f, 0.021f, 0.978f,

			0.279f, 0.317f, 0.505f,
			0.167f, 0.620f, 0.077f,
			0.347f, 0.857f, 0.137f,

			0.055f, 0.953f, 0.042f,
			0.714f, 0.505f, 0.345f,
			0.783f, 0.290f, 0.734f,

			0.722f, 0.645f, 0.174f,
			0.302f, 0.455f, 0.848f,
			0.225f, 0.587f, 0.040f,

			0.517f, 0.713f, 0.338f,
			0.053f, 0.959f, 0.120f,
			0.393f, 0.621f, 0.362f,

			0.673f, 0.211f, 0.457f,
			0.820f, 0.883f, 0.371f,
			0.982f, 0.099f, 0.879f
		};

		glGenBuffers(1, &colorbuffer[0]);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		GLfloat cowOffsets[][3] = {
				{ 5.0f, 0.0f, -40.0f },
				{ -10.0f, 0.0f, -20.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 20.0f },
				{ 12.0f, 0.0f, 40.0f },
		};

		glGenBuffers(1, &offsetsBufferID[0]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cowOffsets), cowOffsets, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		ObjectParser fencePostParser;

		vector<GLfloat> fencePosts = fencePostParser.Execute("fencePost.obj");

		fencePostVerts = fencePosts.size();

		glGenVertexArrays(1, &VertexArrayID[1]);
		glBindVertexArray(VertexArrayID[1]);

		glGenBuffers(1, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, fencePostVerts * sizeof(GLfloat), fencePosts.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLfloat fencePostColors[] = {
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
		};

		glGenBuffers(1, &colorbuffer[1]);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fencePostColors), fencePostColors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


		GLfloat fenceOffsets[][3] = {
				{ 60.0f, -3.0f, -50.0f },
				{ 50.0f, -3.0f, -50.0f },
				{ 40.0f, -3.0f, -50.0f },
				{ 30.0f, -3.0f, -50.0f },
				{ 20.0f, -3.0f, -50.0f },
				{ 10.0f, -3.0f, -50.0f },
				{ 0.0f, -3.0f, -50.0f },
				{ -10.0f, -3.0f, -50.0f },
				{ -20.0f, -3.0f, -50.0f },
				{ -30.0f, -3.0f, -50.0f },
				{ -40.0f, -3.0f, -50.0f },
				{ -50.0f, -3.0f, -50.0f },
				{ -60.0f, -3.0f, -50.0f },

				{ 60.0f, -3.0f, -40.0f },
				{ 60.0f, -3.0f, -30.0f },
				{ 60.0f, -3.0f, -20.0f },
				{ 60.0f, -3.0f, -10.0f },
				{ 60.0f, -3.0f, 0.0f },
				{ 60.0f, -3.0f, 10.0f },
				{ 60.0f, -3.0f, 20.0f },
				{ 60.0f, -3.0f, 30.0f },
				{ 60.0f, -3.0f, 40.0f },
				{ 60.0f, -3.0f, 50.0f },
				{ 60.0f, -3.0f, 60.0f },
				{ 60.0f, -3.0f, 70.0f },
				{ 60.0f, -3.0f, 80.0f },

				{ 60.0f, -3.0f, 80.0f },
				{ 50.0f, -3.0f, 80.0f },
				{ 40.0f, -3.0f, 80.0f },
				{ 30.0f, -3.0f, 80.0f },
				{ 20.0f, -3.0f, 80.0f },
				{ 10.0f, -3.0f, 80.0f },
				{ 0.0f, -3.0f, 80.0f },
				{ -10.0f, -3.0f, 80.0f },
				{ -20.0f, -3.0f, 80.0f },
				{ -30.0f, -3.0f, 80.0f },
				{ -40.0f, -3.0f, 80.0f },
				{ -50.0f, -3.0f, 80.0f },
				{ -60.0f, -3.0f, 80.0f },

				{ -60.0f, -3.0f, -50.0f },
				{ -60.0f, -3.0f, -40.0f },
				{ -60.0f, -3.0f, -30.0f },
				{ -60.0f, -3.0f, -20.0f },
				{ -60.0f, -3.0f, -10.0f },
				{ -60.0f, -3.0f, 0.0f },
				{ -60.0f, -3.0f, 10.0f },
				{ -60.0f, -3.0f, 20.0f },
				{ -60.0f, -3.0f, 30.0f },
				{ -60.0f, -3.0f, 40.0f },
				{ -60.0f, -3.0f, 50.0f },
				{ -60.0f, -3.0f, 60.0f },
				{ -60.0f, -3.0f, 70.0f },
		};
		glGenBuffers(1, &offsetsBufferID[1]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fenceOffsets), fenceOffsets, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		ObjectParser fieldParser;

		vector<GLfloat> field = fieldParser.Execute("field.obj");
		fieldVerts = field.size();

		GLfloat fieldColors[] = {
			0.184314f, 0.309804f, 0.184314f,
			0.184314f, 0.309804f, 0.184314f,
			0.184314f, 0.309804f, 0.184314f,

			0.184314f, 0.309804f, 0.184314f,
			0.184314f, 0.309804f, 0.184314f,
			0.184314f, 0.309804f, 0.184314f
		};
		glGenVertexArrays(1, &VertexArrayID[2]);
		glBindVertexArray(VertexArrayID[2]);

		glGenBuffers(1, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, fieldVerts * sizeof(GLfloat), field.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &colorbuffer[2]);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fieldColors), fieldColors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLfloat fieldOffsets[][3] = {
				{ 60.0f, 0.0f, -50.0f }
		};
		glGenBuffers(1, &offsetsBufferID[1]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fieldOffsets) * sizeof(GLfloat), fieldOffsets, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		ObjectParser fenceParser;

		vector<GLfloat> fenceBeams = fenceParser.Execute("fenceBeams.obj");
		fenceBeamVerts = fenceBeams.size();

		glGenVertexArrays(1, &VertexArrayID[3]);
		glBindVertexArray(VertexArrayID[3]);

		glGenBuffers(1, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, fenceBeamVerts * sizeof(GLfloat), fenceBeams.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLfloat fenceColors[] = {
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,

			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
			0.35f, 0.16f, 0.14f,
		};

		glGenBuffers(1, &colorbuffer[3]);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fenceColors), fenceColors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLfloat fenceBeamOffsets[][3] = {
				{ 60.0f, -5.0f, 80.0f },
				{ 60.0f, -5.0f, -50.0f }
		};
		glGenBuffers(1, &offsetsBufferID[3]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fenceBeamOffsets), fenceBeamOffsets, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		ObjectParser fenceRotatedParser;

		vector<GLfloat> fenceBeamsRotated = fenceRotatedParser.Execute("fenceBeamsRotated.obj");
		fenceRotatedVerts = fenceBeamsRotated.size();

		glGenVertexArrays(1, &VertexArrayID[4]);
		glBindVertexArray(VertexArrayID[4]);

		glGenBuffers(1, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, fenceRotatedVerts * sizeof(GLfloat), fenceBeamsRotated.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &colorbuffer[4]);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fenceColors), fenceColors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLfloat fenceRotatedOffsets[][3] = {
				{ -60.0f, -5.0f, 80.0f },
				{ 60.0f, -5.0f, 80.0f }
		};
		glGenBuffers(1, &offsetsBufferID[4]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fenceRotatedOffsets), fenceRotatedOffsets, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);
	}

	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
	{

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(VertexShaderStream, Line))
				VertexShaderCode += "\n" + Line;
			VertexShaderStream.close();
		}
		else
		{
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
			getchar();
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(FragmentShaderStream, Line))
				FragmentShaderCode += "\n" + Line;
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const * VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}

		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const * FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}

		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}
};