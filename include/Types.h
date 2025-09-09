#pragma once

#include <glm/glm.hpp>

typedef struct {
	glm::vec3 origin;
	glm::vec3 direction;
} Ray;

#define RAY_FLAG_NONE (0)
#define RAY_FLAG_LIGHT (1U << 0)

typedef struct {
	glm::vec4 v1, v2, v3;
	glm::vec4 normal;
	glm::vec4 color;
	unsigned int flags;
	unsigned int pad[3];
} RayFace;