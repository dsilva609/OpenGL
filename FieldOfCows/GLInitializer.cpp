#include <iostream>
#include <cstdlib>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "ObjectParser.cpp"
#include "shader.hpp"

using namespace glm;
using namespace std;

#define WINDOW_TITLE_PREFIX "Project 3 - Luxo"

static int currentWidth;
static int currentHeight;
static unsigned frameCount;
static int dataSize;
static int baseVerts;
static int headVerts;
static int fieldVerts;
static int jointVerts;
static int armVerts;
static float rotationAngle;

static GLuint programID;
static GLuint MatrixID;
static GLuint VertexArrayID[5];
static GLuint offsetsBufferID[5];
static GLuint vertexbuffer[5];
static GLuint colorbuffer[5];
static GLuint color;

static mat4 MVP;
static mat4 Projection;
static mat4 View;
static mat4 Model;

static mat4 Rotation;
static vec3 axis;
static float angle = 0;

class GLInitializer
{
public:
	void Initialize(int argCount, char* argValues[], int windowWidth, int windowHeight, const char* vertexShaderFilename, const char* fragmentShaderFilename)
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

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Create and compile our GLSL program from the shaders
		programID = LoadShaders(vertexShaderFilename, fragmentShaderFilename);

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation(programID, "MVP");

		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		SetupObjects();

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
		Projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 200.0f);
		// Camera matrix
		View = glm::lookAt(
			glm::vec3(50, 3, 0), // Camera is at (x, y, z), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);
		MVP = Projection * View * Model * Rotation;

		//Draw Head
		DrawObject(VertexArrayID[0], vertexbuffer[0], 1, headVerts, 0.0f, 0.35f, 0.16f, 0.14f);

		//Draw joints
		DrawObject(VertexArrayID[1], vertexbuffer[1], 2, jointVerts, 0.0f, 0.35f, 0.16f, 0.14f);

		//Draw Arms
		DrawObject(VertexArrayID[2], vertexbuffer[2], 2, armVerts, 90.0f, 0.35f, 0.16f, 0.14f);

		// Draw the Base
		DrawObject(VertexArrayID[3], vertexbuffer[3], 1, baseVerts, 0.0f, 0.0f, 0.0f, 0.0f);

		//Draw field
		DrawObject(VertexArrayID[4], vertexbuffer[4], 1, fieldVerts, 0.0f, 0.184314f, 0.309804f, 0.184314f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glutSwapBuffers();
		glutPostRedisplay();

		glFlush();
	}

	void SetupObjects()
	{
		vector<GLfloat> headOffsets =
		{
			0.0f, 9.0f, 0.0f
		};

		headVerts = CreateVAO("coneWithNormals.obj", 0, headOffsets);
		headOffsets.clear();

		vector<GLfloat> jointOffsets =
		{
			0.0f, 6.0f, 0.0f,
			0.0f, 8.0f, 0.0f
		};

		jointVerts = CreateVAO("sphereWithNormals.obj", 1, jointOffsets);
		jointOffsets.clear();

		vector<GLfloat> armOffsets =
		{
			0.0f, 5.0f, 0.0f,
			0.0f, 7.0f, 0.0f
		};

		armVerts = CreateVAO("cylinderWithNormals.obj", 2, armOffsets);
		armOffsets.clear();

		vector<GLfloat> baseOffsets =
		{
			0.0f, 0.0f, 0.0f,
		};

		baseVerts = CreateVAO("cylinderWithNormals.obj", 3, baseOffsets);
		baseOffsets.clear();

		vector<GLfloat> fieldOffsets =
		{
			60.0f, 0.0f, -50.0f
		};

		fieldVerts = CreateVAO("field.obj", 4, fieldOffsets);
		fieldOffsets.clear();
	}

	static void DrawObject(GLuint vertexArrayID, GLuint vertexBuffer, int numObjects, int vertices, float angle, GLfloat color1, GLfloat color2, GLfloat color3)
	{
		axis = { 0.0, 1.0, 0.0 };
		Rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

		MVP *= Rotation;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(vertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		color = glGetUniformLocation(programID, "fragmentColor");

		glUniform3f(color, color1, color2, color3);

		glDrawArraysInstanced(GL_TRIANGLES, 0, vertices / 3, numObjects);
	}

	static void IdleFunction(void)
	{
		angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;  // X° per second
		axis = { 0.0, 1.0, 0.0 };
		Rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

		MVP = Projection * View * Model * Rotation;

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

	int CreateVAO(string objectFilename, int objectNumber, vector<GLfloat> offsets)
	{
		//VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID[objectNumber]);
		glBindVertexArray(VertexArrayID[objectNumber]);

		ObjectParser parser;

		vector <GLfloat> data = parser.Execute(objectFilename);
		int numVerts = data.size();
		dataSize = numVerts * sizeof(GLfloat);

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

		glGenBuffers(1, &offsetsBufferID[0]);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID[0]);
		glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(GLfloat), offsets.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		return numVerts;
	}
};