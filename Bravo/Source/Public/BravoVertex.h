#pragma once
#include "stdafx.h"

struct Vertex
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Normal = glm::vec3(0.0f);
	glm::vec2 TexCoords = glm::vec2(0.0f);
	glm::vec3 Tangent = glm::vec3(0.0f);
	glm::vec3 Bitangent = glm::vec3(0.0f);
	glm::vec4 Color = glm::vec4(0.0f);
};
