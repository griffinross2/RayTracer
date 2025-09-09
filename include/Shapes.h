#pragma once

#include <array>
#include <vector>

#include "Types.h"

class Shape {

public:
	Shape();
	~Shape();

	virtual void draw(std::vector<RayFace>& faceList);
};

class Cube : public Shape {

public:
	Cube(glm::vec3 center, glm::vec3 size, glm::vec3 color, bool light);
	~Cube();
	void draw(std::vector<RayFace>& faceList) override;

private:
	std::array<glm::vec4, 8> m_vertices;
	std::array<glm::vec4, 6> m_normals;
	glm::vec4 m_color;
	bool m_light;
};