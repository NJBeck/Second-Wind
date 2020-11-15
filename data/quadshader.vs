#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 screenPos;

out vec2 TexCoord;

void main()
{
	vec2 newPos;
	newPos.x = aPos.x + screenPos.x;
	newPos.y = aPos.y + screenPos.y;
	gl_Position = vec4(newPos, 1.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}