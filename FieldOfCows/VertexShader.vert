#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 offset;
// Output data ; will be interpolated for each fragment.
// vec3 fragmentColor;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(position.x + offset.x, position.y + offset.y, position.z + offset.z , 1);

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
//	fragmentColor = color;
}