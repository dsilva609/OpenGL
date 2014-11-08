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

#define WINDOW_TITLE_PREFIX "Project 2 - Field of Cows"

static int currentWidth;
static int currentHeight;
static unsigned frameCount;
static int dataSize;
static int cowVerts;
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

		//CreateVAO(objectFilename);

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
		Projection = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 200.0f);
		// Camera matrix
		View = glm::lookAt(
			glm::vec3(100, 3, 0), // Camera is at (x, y, z), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);

		// Draw the Cows
		DrawObject(VertexArrayID[0], vertexbuffer[0], 5, cowVerts, 0.0f, 0.0f, 0.0f, 0.0f);

		//Draw fence posts
		DrawObject(VertexArrayID[1], vertexbuffer[1], 51, fencePostVerts, 0.0f, 0.35f, 0.16f, 0.14f);

		//Draw field
		DrawObject(VertexArrayID[2], vertexbuffer[2], 1, fieldVerts, 0.0f, 0.184314f, 0.309804f, 0.184314f);

		//Draw Beams
		DrawObject(VertexArrayID[3], vertexbuffer[3], 2, fenceBeamVerts, 0.0f, 0.35f, 0.16f, 0.14f);

		//Draw Rotated Beams
		DrawObject(VertexArrayID[4], vertexbuffer[4], 2, fenceRotatedVerts, 90.0f, 0.35f, 0.16f, 0.14f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_3D);

		glutSwapBuffers();
		glutPostRedisplay();

		glFlush();
	}

	void SetupObjects()
	{
		vector<GLfloat> cowOffsets =
		{
			5.0f, 0.0f, -40.0f,
			-10.0f, 0.0f, -20.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 20.0f,
			12.0f, 0.0f, 40.0f
		};

		cowVerts = CreateVAO("triangulatedCowNoNormals.obj", 0, cowOffsets);
		cowOffsets.clear();

		vector<GLfloat> fenceOffsets =
		{
			60.0f, -3.0f, -50.0f,
			50.0f, -3.0f, -50.0f,
			40.0f, -3.0f, -50.0f,
			30.0f, -3.0f, -50.0f,
			20.0f, -3.0f, -50.0f,
			10.0f, -3.0f, -50.0f,
			0.0f, -3.0f, -50.0f,
			-10.0f, -3.0f, -50.0f,
			-20.0f, -3.0f, -50.0f,
			-30.0f, -3.0f, -50.0f,
			-40.0f, -3.0f, -50.0f,
			-50.0f, -3.0f, -50.0f,
			-60.0f, -3.0f, -50.0f,

			60.0f, -3.0f, -40.0f,
			60.0f, -3.0f, -30.0f,
			60.0f, -3.0f, -20.0f,
			60.0f, -3.0f, -10.0f,
			60.0f, -3.0f, 0.0f,
			60.0f, -3.0f, 10.0f,
			60.0f, -3.0f, 20.0f,
			60.0f, -3.0f, 30.0f,
			60.0f, -3.0f, 40.0f,
			60.0f, -3.0f, 50.0f,
			60.0f, -3.0f, 60.0f,
			60.0f, -3.0f, 70.0f,

			60.0f, -3.0f, 70.0f,
			50.0f, -3.0f, 70.0f,
			40.0f, -3.0f, 70.0f,
			30.0f, -3.0f, 70.0f,
			20.0f, -3.0f, 70.0f,
			10.0f, -3.0f, 70.0f,
			0.0f, -3.0f, 70.0f,
			-10.0f, -3.0f, 70.0f,
			-20.0f, -3.0f, 70.0f,
			-30.0f, -3.0f, 70.0f,
			-40.0f, -3.0f, 70.0f,
			-50.0f, -3.0f, 70.0f,
			-60.0f, -3.0f, 70.0f,

			-60.0f, -3.0f, -50.0f,
			-60.0f, -3.0f, -40.0f,
			-60.0f, -3.0f, -30.0f,
			-60.0f, -3.0f, -20.0f,
			-60.0f, -3.0f, -10.0f,
			-60.0f, -3.0f, 0.0f,
			-60.0f, -3.0f, 10.0f,
			-60.0f, -3.0f, 20.0f,
			-60.0f, -3.0f, 30.0f,
			-60.0f, -3.0f, 40.0f,
			-60.0f, -3.0f, 50.0f,
			-60.0f, -3.0f, 60.0f,
			-60.0f, -3.0f, 70.0f,
		};

		fencePostVerts = CreateVAO("fencePost.obj", 1, fenceOffsets);
		fenceOffsets.clear();

		vector<GLfloat> fieldOffsets =
		{
			60.0f, 0.0f, -50.0f
		};

		fieldVerts = CreateVAO("field.obj", 2, fieldOffsets);
		fieldOffsets.clear();

		vector<GLfloat> fenceBeamOffsets =
		{
			60.0f, -5.0f, 70.0f,
			60.0f, -5.0f, -50.0f
		};

		fenceBeamVerts = CreateVAO("fenceBeams.obj", 3, fenceBeamOffsets);
		fenceBeamOffsets.clear();

		vector<GLfloat> fenceRotatedOffsets =
		{
			50.0f, -5.0f, -60.0f,
			50.0f, -5.0f, 60.0f
		};

		fenceRotatedVerts = CreateVAO("fenceBeams.obj", 4, fenceRotatedOffsets);
		fenceRotatedOffsets.clear();
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

	//	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
	//	{
	//
	//		// Create the shaders
	//		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	//		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	//
	//		// Read the Vertex Shader code from the file
	//		std::string VertexShaderCode;
	//		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	//		if (VertexShaderStream.is_open())
	//		{
	//			std::string Line = "";
	//			while (getline(VertexShaderStream, Line))
	//				VertexShaderCode += "\n" + Line;
	//			VertexShaderStream.close();
	//		}
	//		else
	//		{
	//			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
	//			getchar();
	//			return 0;
	//		}
	//
	//		// Read the Fragment Shader code from the file
	//		std::string FragmentShaderCode;
	//		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	//		if (FragmentShaderStream.is_open())
	//		{
	//			std::string Line = "";
	//			while (getline(FragmentShaderStream, Line))
	//				FragmentShaderCode += "\n" + Line;
	//			FragmentShaderStream.close();
	//		}
	//
	//		GLint Result = GL_FALSE;
	//		int InfoLogLength;
	//
	//		// Compile Vertex Shader
	//		printf("Compiling shader : %s\n", vertex_file_path);
	//		char const * VertexSourcePointer = VertexShaderCode.c_str();
	//		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	//		glCompileShader(VertexShaderID);
	//
	//		// Check Vertex Shader
	//		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	//		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//		if (InfoLogLength > 0)
	//		{
	//			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
	//			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	//			printf("%s\n", &VertexShaderErrorMessage[0]);
	//		}
	//
	//		// Compile Fragment Shader
	//		printf("Compiling shader : %s\n", fragment_file_path);
	//		char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	//		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	//		glCompileShader(FragmentShaderID);
	//
	//		// Check Fragment Shader
	//		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	//		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//		if (InfoLogLength > 0)
	//		{
	//			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
	//			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	//			printf("%s\n", &FragmentShaderErrorMessage[0]);
	//		}
	//
	//		// Link the program
	//		printf("Linking program\n");
	//		GLuint ProgramID = glCreateProgram();
	//		glAttachShader(ProgramID, VertexShaderID);
	//		glAttachShader(ProgramID, FragmentShaderID);
	//		glLinkProgram(ProgramID);
	//
	//		// Check the program
	//		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	//		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//		if (InfoLogLength > 0)
	//		{
	//			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
	//			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	//			printf("%s\n", &ProgramErrorMessage[0]);
	//		}
	//
	//		glDeleteShader(VertexShaderID);
	//		glDeleteShader(FragmentShaderID);
	//
	//		return ProgramID;
	//	}
};