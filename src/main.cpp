#include <Application.h>

#include <deque>
#include <vector>
#include <numbers>

#include <Primitives.h>
#include <Shader.h>
#include <Input.h>

#include "Shapes.h"

IMGL::Shader* g_rayTraceShader;
IMGL::Shader* g_textureShader;
std::vector<RayFace> g_objFaces;

unsigned int g_objSSBO = 0;
unsigned int g_framebufferVAO = 0;
unsigned int g_textureVAO = 0;
unsigned int g_FBO = 0;
unsigned int g_texture = 0;
unsigned int g_lastWidth = 0, g_lastHeight = 0;
unsigned int g_frameCount = 0;
glm::vec3 g_camLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
float g_camDist = 30.0f;
float g_camAzimuth = 0.0f;
float g_camElevation = 0.0f;
bool g_camLeft = false;
bool g_camRight = false;
bool g_camUp = false;
bool g_camDown = false;
bool g_camIn = false;
bool g_camOut = false;

void keyboardCallback(int key, int action, int mod) {
	if (key == GLFW_KEY_A) {
		g_camLeft = action != GLFW_RELEASE;
	}
	else if (key == GLFW_KEY_D) {
		g_camRight = action != GLFW_RELEASE;
	}
	else if (key == GLFW_KEY_W) {
		g_camUp = action != GLFW_RELEASE;
	}
	else if (key == GLFW_KEY_S) {
		g_camDown = action != GLFW_RELEASE;
	}
	else if (key == GLFW_KEY_Q) {
		g_camIn = action != GLFW_RELEASE;
	}
	else if (key == GLFW_KEY_E) {
		g_camOut = action != GLFW_RELEASE;
	}
}

void initRays() {
	g_rayTraceShader = new IMGL::Shader("app_shaders/ray_trace.vert", "app_shaders/ray_trace.frag");
	g_textureShader = new IMGL::Shader("app_shaders/ray_texture.vert", "app_shaders/ray_texture.frag");

	glGenVertexArrays(1, &g_framebufferVAO);
	glGenVertexArrays(1, &g_textureVAO);
	glGenBuffers(1, &g_objSSBO);
	glGenFramebuffers(1, &g_FBO);
	glGenTextures(1, &g_texture);

	glBindVertexArray(g_framebufferVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, IMGL::Application::width(), IMGL::Application::height(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_texture, 0);
}

void renderRays(void* data) {
	// Setup ray tracing shader
	glEnable(GL_FRAMEBUFFER_SRGB);
	g_rayTraceShader->use();
	g_rayTraceShader->setUint("time", static_cast<unsigned int>(glfwGetTime() * 1000));
	g_rayTraceShader->setUint("frameCount", g_frameCount++);
	g_rayTraceShader->setVec2("fov", 40.0f, 20.0f);
	g_rayTraceShader->setVec2("aperture", 0.1f, 0.1f);
	g_rayTraceShader->setUVec2("windowSize", IMGL::Application::width(), IMGL::Application::height());
	glm::vec3 pos = g_camDist * glm::vec3(cosf(g_camAzimuth) * cosf(g_camElevation), sinf(g_camAzimuth) * cosf(g_camElevation), sinf(g_camElevation)) + g_camLookAt;
	glm::vec3 dir = glm::normalize(g_camLookAt - pos);
	g_rayTraceShader->setVec3("cameraPos", pos.x, pos.y, pos.z);
	g_rayTraceShader->setVec3("cameraDir", dir.x, dir.y, dir.z);
	g_rayTraceShader->setVec3("cameraUp", -1.0 * cosf(g_camAzimuth) * sinf(g_camElevation), -1.0 * sinf(g_camAzimuth) * sinf(g_camElevation), cosf(g_camElevation));

	// Setup framebuffer state
	glBindVertexArray(g_framebufferVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);
	glViewport(0, 0, IMGL::Application::width(), IMGL::Application::height());

	// Setup texture to render to
	glActiveTexture(GL_TEXTURE1);

	// Clear the framebuffer if the scene changed
	if (g_lastWidth != IMGL::Application::width() || g_lastHeight != IMGL::Application::height() || g_camDown || g_camUp || g_camLeft || g_camRight || g_camIn || g_camOut) {
		g_lastWidth = IMGL::Application::width();
		g_lastHeight = IMGL::Application::height();
		glBindTexture(GL_TEXTURE_2D, g_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMGL::Application::width(), IMGL::Application::height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		g_frameCount = 0;
	}

	if (g_camDown) {
		g_camElevation -= 0.03f;
		if (g_camElevation < std::numbers::pi / -2.0f) std::numbers::pi / -2.0f;
	}
	if (g_camUp) {
		g_camElevation += 0.03f;
		if (g_camElevation > std::numbers::pi / 2.0f) g_camElevation = std::numbers::pi / 2.0f;
	}
	if (g_camLeft) {
		g_camAzimuth -= 0.03f;
		if (g_camAzimuth < -std::numbers::pi) g_camAzimuth += std::numbers::pi * 2.0f;
	}
	if (g_camRight) {
		g_camAzimuth += 0.03f;
		if (g_camAzimuth > std::numbers::pi) g_camAzimuth -= std::numbers::pi * 2.0f;
	}
	if (g_camIn) {
		g_camDist -= 0.3f;
		if (g_camDist < 1.0f) g_camDist = 1.0f;
	}
	if (g_camOut) {
		g_camDist += 0.3f;
		if (g_camDist > 100.0f) g_camDist = 100.0f;
	}

	g_camDown = false;
	g_camUp = false;
	g_camLeft = false;
	g_camRight = false;
	g_camIn = false;
	g_camOut = false;

	// Setup SSBOs
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_objSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, g_objFaces.size() * sizeof(RayFace), g_objFaces.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_objSSBO);

	// Draw to framebuffer
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	g_objFaces.clear();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render texture to screen
	glBindVertexArray(g_textureVAO);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	g_textureShader->use();
	g_textureShader->setUVec2("windowSize", IMGL::Application::width(), IMGL::Application::height());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void deinitRays() {
	glDeleteVertexArrays(1, &g_framebufferVAO);
	glDeleteVertexArrays(1, &g_textureVAO);
	glDeleteBuffers(1, &g_objSSBO);
	glDeleteFramebuffers(1, &g_FBO);
	glDeleteTextures(1, &g_texture);
	delete g_rayTraceShader;
}

int main() {
	IMGL::Application app;
	app.setWindowTitle("Ray Tracer Demo");
	app.setWindowSize(1000, 500);

	Cube light1({ -1.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, true);
	Cube light2({ 0.0f, 1.5f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, true);
	Cube light3({ 1.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, true);
	Cube light4({ -4.0f, 0.0f, 1.0f }, { 1.0f, 4.0f, 3.0f }, { 1.0f, 1.0f, 1.0f }, true);
	Cube obj1({ 0.0f, 0.0f, -1.0f }, { 10.0f, 10.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, false);
	Cube obj2({ 0.0f, -1.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, false);
	Cube obj3({ -2.5f, 0.0f, 0.5f }, { 0.5f, 2.0f, 2.0f }, { 1.0f, 1.0f, 1.0f }, false);

	 
	initRays();

	double lastTime = glfwGetTime();
	int fpsFrameCount = 0;

	while (!app.shouldClose()) {
		IMGL::AddKeyEventCallback(keyboardCallback);

		light1.draw(g_objFaces);
		light2.draw(g_objFaces);
		light3.draw(g_objFaces);
		light4.draw(g_objFaces);
		obj1.draw(g_objFaces);
		obj2.draw(g_objFaces);
		obj3.draw(g_objFaces);

		IMGL::CustomCallback cb = { .callback=renderRays, .data=nullptr };
		IMGL::Renderer::get()->renderList.commands.push_back(cb);

		IMGL::Application::draw();

		fpsFrameCount++;
		if (glfwGetTime() - lastTime >= 1.0) {
			IMGL::Application::setWindowTitle(("FPS: " + std::to_string(fpsFrameCount)).c_str());
			fpsFrameCount = 0;
			lastTime = glfwGetTime();
		}
	}

	deinitRays();

	return 0;
}