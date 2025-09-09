#include "Shapes.h"

Shape::Shape() {
}

Shape::~Shape() {
}

void Shape::draw(std::vector<RayFace>& faceList) {
}

Cube::Cube(glm::vec3 center, glm::vec3 size, glm::vec3 color, bool light) {
//	   5-------7
// 	  /|      /|
//	 / |     / |
//	2--4----3  6
//	| /     | /
//	|/      |/
//	0-------1
//    
// z
// | y
// |/__x

	m_vertices[0] = { center.x - size.x / 2, center.y - size.y / 2, center.z - size.z / 2, 0.0f };
	m_vertices[1] = { center.x + size.x / 2, center.y - size.y / 2, center.z - size.z / 2, 0.0f };
	m_vertices[2] = { center.x - size.x / 2, center.y - size.y / 2, center.z + size.z / 2, 0.0f };
	m_vertices[3] = { center.x + size.x / 2, center.y - size.y / 2, center.z + size.z / 2, 0.0f };
	m_vertices[4] = { center.x - size.x / 2, center.y + size.y / 2, center.z - size.z / 2, 0.0f };
	m_vertices[5] = { center.x - size.x / 2, center.y + size.y / 2, center.z + size.z / 2, 0.0f };
	m_vertices[6] = { center.x + size.x / 2, center.y + size.y / 2, center.z - size.z / 2, 0.0f };
	m_vertices[7] = { center.x + size.x / 2, center.y + size.y / 2, center.z + size.z / 2, 0.0f };

// -x = 0
// +x = 1
// -y = 2
// +y = 3
// -z = 4
// +z = 5

	m_normals[0] = { -1.0f, 0.0f, 0.0f, 0.0f };
	m_normals[1] = { 1.0f, 0.0f, 0.0f, 0.0f };
	m_normals[2] = { 0.0f, -1.0f, 0.0f, 0.0f };
	m_normals[3] = { 0.0f, 1.0f, 0.0f, 0.0f };
	m_normals[4] = { 0.0f, 0.0f, -1.0f, 0.0f };
	m_normals[5] = { 0.0f, 0.0f, 1.0f, 0.0f };

	m_color = glm::vec4(color.x, color.y, color.z, 0.0);
	m_light = light;
}

Cube::~Cube() {
}

void Cube::draw(std::vector<RayFace>& faceList) {
	unsigned int flags = RAY_FLAG_NONE;
	flags |= m_light ? RAY_FLAG_LIGHT : 0;

	faceList.push_back({ m_vertices[0], m_vertices[2], m_vertices[4], m_normals[0], m_color, flags, {0} });
	faceList.push_back({ m_vertices[2], m_vertices[5], m_vertices[4], m_normals[0], m_color, flags, {0} });

	faceList.push_back({ m_vertices[1], m_vertices[6], m_vertices[3], m_normals[1], m_color, flags, {0} });
	faceList.push_back({ m_vertices[6], m_vertices[7], m_vertices[3], m_normals[1], m_color, flags, {0} });

	faceList.push_back({ m_vertices[0], m_vertices[1], m_vertices[3], m_normals[2], m_color, flags, {0} });
	faceList.push_back({ m_vertices[0], m_vertices[3], m_vertices[2], m_normals[2], m_color, flags, {0} });

	faceList.push_back({ m_vertices[4], m_vertices[5], m_vertices[7], m_normals[3], m_color, flags, {0} });
	faceList.push_back({ m_vertices[4], m_vertices[7], m_vertices[6], m_normals[3], m_color, flags, {0} });

	faceList.push_back({ m_vertices[0], m_vertices[4], m_vertices[6], m_normals[4], m_color, flags, {0} });
	faceList.push_back({ m_vertices[0], m_vertices[6], m_vertices[1], m_normals[4], m_color, flags, {0} });

	faceList.push_back({ m_vertices[2], m_vertices[3], m_vertices[7], m_normals[5], m_color, flags, {0} });
	faceList.push_back({ m_vertices[2], m_vertices[7], m_vertices[5], m_normals[5], m_color, flags, {0} });
}