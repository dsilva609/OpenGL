#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <GL\glew.h>
using namespace std;

typedef struct
{
	float XYZ[3];
} Vertex;

typedef struct
{
	float XYZ[3];
} Face;

class ObjectParser
{
private:
	vector<Vertex> vertices;
	vector<Face> faces;
	vector <GLfloat>data;

public:
	vector<GLfloat> Execute(string filename)
	{
		fstream file;
		string lineIn;
		Vertex tempVert;
		Face tempFace;
		string buffer;
		float flt;
		int num;
		stringstream stream;

		file.open(filename);

		if (file.good())
		{
			cout << "Input file \"" << filename << "\" opened successfully" << endl;

			vertices.push_back(tempVert);

			while (file.good())
			{
				getline(file, lineIn);

				if (lineIn.substr(0, 1) == "v")
				{
					stream.str(lineIn);
					stream >> buffer;

					stream >> flt;
					tempVert.XYZ[0] = flt;
					stream >> flt;
					tempVert.XYZ[1] = flt;
					stream >> flt;
					tempVert.XYZ[2] = flt;
					stream.clear();

					vertices.push_back(tempVert);
				}
				else if (lineIn.substr(0, 1) == "f")
				{
					stream.str(lineIn);
					stream >> buffer;

					stream >> num;
					tempFace.XYZ[0] = num;

					stream >> num;
					tempFace.XYZ[1] = num;

					stream >> num;
					tempFace.XYZ[2] = num;

					stream.clear();
					faces.push_back(tempFace);
				}
			}
			file.close();

			for (int i = 0; i < faces.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					data.push_back(vertices.at(faces.at(i).XYZ[j]).XYZ[0]);
					data.push_back(vertices.at(faces.at(i).XYZ[j]).XYZ[1]);
					data.push_back(vertices.at(faces.at(i).XYZ[j]).XYZ[2]);
				}
			}
			return data;
		}

		else
			cout << "Could not open file \"" << filename << "\"" << endl;
		file.close();
	}
};