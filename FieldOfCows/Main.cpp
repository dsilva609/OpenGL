#include "GLInitializer.cpp"

int main(int argCount, char* argValues[])
{
	GLInitializer initializer;

	initializer.Initialize(argCount, argValues, 1600, 900, "triangulatedCowNoNormals.obj", "VertexShader.vert", "ColorShader.frag");

	exit(EXIT_SUCCESS);
}