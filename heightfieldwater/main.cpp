#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>
#include "../Util/Camera.h"
#include "../Util/Shader.h"
#include "../Util/Model.h"
#define DEBUG_ENABLED FALSE
#define SKYBOX_ENABLED TRUE

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseMove_callback(GLFWwindow* window, double xpos, double ypos);
void mouseButton_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
bool pathTracingWater(GLfloat &dropX, GLfloat &dropy, const glm::mat4 &view, const glm::mat4 &projection);
inline void createWaterMesh(GLfloat *&mesh, GLint detail);
GLuint loadCubemap(const GLchar** faces);

const GLuint WIDTH = 800, HEIGHT = 600;
const GLfloat TEXELSIZE = 1.0 / 256.0;
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

bool isAddDrop = true;
bool cameraRotEnabled = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "water", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouseMove_callback);
	glfwSetMouseButtonCallback(window, mouseButton_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}
	if (!gl3wIsSupported(3, 3)) {
		fprintf(stderr, "OpenGL 3.2 not supported\n");
		return -1;
	}
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	glViewport(0, 0, WIDTH, HEIGHT);

	Shader dropShader("drop.vert", "drop.frag");
	Shader processShader("drop.vert", "proc.frag");
	Shader normalShader("drop.vert", "normal.frag");
	Shader debugShader("debug.vert", "debug.frag");
	Shader watermeshShader("watermesh.vert", "watermesh.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader modelShader("model.vert", "model.frag");

	string a = "..\\models\\havuz.obj";
	Model cyborg(a);

	GLfloat inVertices[] = {

		-1.0f, 1.0f, 0.0,
		-1.0f, -1.0f, 0.0,
		1.0f, -1.0f, 0.0,

		-1.0f, 1.0f, 0.0,
		1.0f, -1.0f, 0.0,
		1.0f,  1.0f, 0.0 };


	GLfloat debugVertices[] = {

		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  -1.0f,0.0f, 0.0f, 0.0f,
		1.0f,  -1.0f, 0.0f,1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f,0.0f, 1.0f,
		1.0f,  -1.0f,0.0f, 1.0f, 0.0f,
		1.0f,   1.0f, 0.0f,1.0f, 1.0f
	};
#if SKYBOX_ENABLED
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
#endif

#if SKYBOX_ENABLED
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// SkyBox
	const GLchar* skyfaces[6];
	skyfaces[0] = "..\\Skybox\\sb_right.png";
	skyfaces[1] = "..\\Skybox\\sb_left.png";
	skyfaces[2] = "..\\Skybox\\sb_up.png";
	skyfaces[3] = "..\\Skybox\\sb_down.png";
	skyfaces[4] = "..\\Skybox\\sb_back.png";
	skyfaces[5] = "..\\Skybox\\sb_front.png";
	GLuint skyboxTexture = loadCubemap(skyfaces);

	const GLchar* poolfaces[6];
	poolfaces[0] = "..\\models\\havuz.jpg";
	poolfaces[1] = "..\\models\\havuz.jpg";
	poolfaces[2] = "..\\models\\havuz.jpg";
	poolfaces[3] = "..\\models\\havuz.jpg";
	poolfaces[4] = "..\\models\\havuz.jpg";
	poolfaces[5] = "..\\models\\havuz.jpg";
	GLuint poolboxTexture = loadCubemap(poolfaces);
#endif

	GLfloat *waterMesh = NULL;
	GLint detail = 256;
	createWaterMesh(waterMesh, detail);
	GLuint inVAO, inVBO;
	glGenVertexArrays(1, &inVAO);
	glGenBuffers(1, &inVBO);
	glBindVertexArray(inVAO);
	glBindBuffer(GL_ARRAY_BUFFER, inVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(inVertices), inVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glBindVertexArray(0);


	GLuint debugVAO, debugVBO;
	glGenVertexArrays(1, &debugVAO);
	glGenBuffers(1, &debugVBO);
	glBindVertexArray(debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLuint waterVAO, waterVBO;
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*detail*detail * 30, waterMesh, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);


	GLuint pingpongFBO[2];
	GLuint pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	glm::mat4 model, view, projection;
	GLint modelLoc, viewLoc, projLoc;
	short toggle = 0;
	GLfloat dropX, dropY;


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement();

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glViewport(0, 0, 256, 256);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (isAddDrop) {
			if (pathTracingWater(dropX, dropY, view, projection)) {
				dropShader.use();
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[toggle]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1 - toggle]);
				dropShader.setVec2("center", dropX, dropY);
				glBindVertexArray(inVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				toggle = 1 - toggle;
				isAddDrop = false;
			}
		}
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			processShader.use();
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[toggle]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1 - toggle]);
			processShader.setFloat("delta", TEXELSIZE);
			glBindVertexArray(inVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			toggle = 1 - toggle;
		}

		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			normalShader.use();
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[toggle]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1 - toggle]);
			normalShader.setFloat("delta", TEXELSIZE);
			glBindVertexArray(inVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			toggle = 1 - toggle;
		}

		{
			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			watermeshShader.use();
			watermeshShader.setMat4("model", model);
			watermeshShader.setMat4("view", view);
			watermeshShader.setMat4("projection", projection);
			watermeshShader.setVec3("camPos", camera.Position);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1 - toggle]);	// Use the color attachment texture as the texture of the quad plane
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glBindVertexArray(waterVAO);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, detail*detail * 6);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(0);
			modelShader.use();
			modelShader.setMat4("model", model);
			modelShader.setMat4("view", view);
			modelShader.setMat4("projection", projection);
			cyborg.Draw(modelShader);
		}


#if SKYBOX_ENABLED
		{
			// Draw skybox as last
			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.use();
			glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // Set depth function back to default
		}
#endif

#if DEBUG_ENABLED
		{
			glViewport(0, 0, 256, 256);
			debugShader.use();
			glBindVertexArray(debugVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1 - toggle]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
#endif
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &debugVAO);
	glDeleteBuffers(1, &debugVBO);
	glDeleteVertexArrays(1, &debugVAO);
	glDeleteBuffers(1, &debugVBO);
	//delete waterMesh;
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}


void do_movement()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool firstMouse = true;
void mouseMove_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;
	if (cameraRotEnabled)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouseButton_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		isAddDrop = true;
		cameraRotEnabled = true;
	}
	else if (button == GLFW_RELEASE) {
		cameraRotEnabled = false;
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


bool pathTracingWater(GLfloat &dropX, GLfloat &dropY, const glm::mat4 &view, const glm::mat4 &projection)
{
	dropX = (2.0f * lastX) / WIDTH - 1.0f;
	dropY = 1.0f - (2.0f * lastY) / HEIGHT;
	glm::vec4 drop_near = glm::vec4(dropX, dropY, 0, 1);
	glm::vec4 drop_far = glm::vec4(dropX, dropY, 1, 1);

	glm::mat4 inverse_MVP = glm::inverse(projection*view);
	drop_near = inverse_MVP*drop_near;
	drop_far = inverse_MVP*drop_far;
	glm::vec3 drop_near_norm = glm::vec3(drop_near / drop_near.w);
	glm::vec3 drop_far_norm = glm::vec3(drop_far / drop_far.w);
	glm::vec3 direction = glm::normalize(glm::vec3(drop_far_norm - drop_near_norm));

	GLfloat distance;
	bool istrue = glm::intersectRayPlane(drop_near_norm, direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), distance);
	if (istrue) {
		glm::vec3 intersection_point = drop_near_norm + distance*direction;
		if (glm::abs(intersection_point.x) <= 1.0 && glm::abs(intersection_point.y) <= 1.0) {
			dropX = intersection_point.x*0.5 + 0.5;
			dropY = intersection_point.y*0.5 + 0.5;
			std::cout << dropX << "  " << dropY << std::endl;
			return true;
		}

	}
	return false;



}

inline GLfloat denormalize(GLfloat var) {
	return (var*2.0 - 1.0);
}

inline void createWaterMesh(GLfloat *&mesh, GLint detail)
{
	mesh = new GLfloat[30 * detail*detail];
	//std::cout << (30*detail*detail) << std::endl;
	std::cout << mesh << std::endl;
	GLfloat detailsize = 1.0 / (GLfloat)detail;
	for (int y = 0; y < detail; y++) {
		for (int x = 0; x < detail; x++) {
			int index = (detail*y + x) * 30;
			//std::cout << "index " << index << std::endl;
			mesh[index] = denormalize(detailsize*(GLfloat)x);
			mesh[index + 1] = denormalize(detailsize*(GLfloat)y);
			mesh[index + 2] = 0.0;
			mesh[index + 3] = detailsize*(GLfloat)x;
			mesh[index + 4] = detailsize*(GLfloat)y;

			mesh[index + 5] = denormalize(detailsize*(GLfloat)(x + 1));
			mesh[index + 6] = denormalize(detailsize*(GLfloat)y);
			mesh[index + 7] = 0.0;
			mesh[index + 8] = detailsize*(GLfloat)(x + 1);
			mesh[index + 9] = detailsize*(GLfloat)y;

			mesh[index + 10] = denormalize(detailsize*(GLfloat)x);
			mesh[index + 11] = denormalize(detailsize*(GLfloat)(y + 1));
			mesh[index + 12] = 0.0;
			mesh[index + 13] = detailsize*(GLfloat)x;
			mesh[index + 14] = detailsize*(GLfloat)(y + 1);

			mesh[index + 15] = denormalize(detailsize*(GLfloat)x);
			mesh[index + 16] = denormalize(detailsize*(GLfloat)(y + 1));
			mesh[index + 17] = 0.0;
			mesh[index + 18] = detailsize*(GLfloat)x;
			mesh[index + 19] = detailsize*(GLfloat)(y + 1);

			mesh[index + 20] = denormalize(detailsize*(GLfloat)(x + 1));
			mesh[index + 21] = denormalize(detailsize*(GLfloat)(y + 1));
			mesh[index + 22] = 0.0;
			mesh[index + 23] = detailsize*(GLfloat)(x + 1);
			mesh[index + 24] = detailsize*(GLfloat)(y + 1);

			mesh[index + 25] = denormalize(detailsize*(GLfloat)(x + 1));
			mesh[index + 26] = denormalize(detailsize*(GLfloat)y);
			mesh[index + 27] = 0.0;
			mesh[index + 28] = detailsize*(GLfloat)(x + 1);
			mesh[index + 29] = detailsize*(GLfloat)y;
		}
	}
}


GLuint loadCubemap(const GLchar** faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < 6; i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

