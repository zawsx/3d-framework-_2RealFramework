#version 330 core

in vec3 position;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	gl_Position = matProj * matView * matModel * vec4( position, 1.0 );
}