#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_m.h"
#include "car.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "model.h"



using namespace std;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
GLFWwindow* window;
glm::vec3 camPos = glm::vec3(0,0,0);
glm::vec3 carPos = glm::vec3(17.5f, 3.0f, 4.0f);
Camera camera(camPos);
float lastX = 1280 / 2.0f;
float lastY = 720 / 2.0f;
bool firstMouse = true;
glm::vec3 front = glm::vec3(0.0, 0.0, -1.0);
glm::mat4 homeMadeCam = glm::lookAt(camPos, camPos + front, glm::vec3(0.0, 1.0, 0.0));
int camAngle = 1;
bool set = false;
int state1 = GLFW_RELEASE;
int state2 = GLFW_RELEASE;
float teka = 0, tekb = 0, tekc = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::mat4 model = glm::mat4(1.0f);
//

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && teka == 0) {
		camAngle = 1;
		state1 = GLFW_PRESS;
	}
	else if (state1 == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		set = true;
		state1 = GLFW_RELEASE;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && tekb == 0) {
		camAngle = 2;
		state2 = GLFW_PRESS;
	}
	else if (state2 == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		set = true;
		state2 = GLFW_RELEASE;
	}
		
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


int createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Car Racing", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void callbacks() {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void callInputs() {
	processInput(window);
}

int main()
{	
	createWindow();
	callbacks();
	glEnable(GL_DEPTH_TEST);
	//
	Shader shader("camera_vertex.vs", "camera_fragment.fs");
	Shader shader1("camera_vertex.vs", "camera_fragment.fs");
	Model carModel("from/10604_slot_car_red_SG_v1_iterations-2.obj");
	Model trackModel("from/10605_Slot_Car_Race_Track_v1_L3.obj");
	
	Car carObj;
	carObj.initialize();
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.2f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 modelCar = model;
	modelCar = glm::scale(model, glm::vec3(1, 0.5, 1));
	glm::vec3 displacementCar = glm::vec3(17.5, -20, 1);
	glm::vec3 dC = glm::vec3(-1.75f,-0.1f,-2.0f);
	modelCar = glm::translate(modelCar, displacementCar);
	glm::vec3 displacement = glm::vec3(-1.75f, -0.4f, -3.0f);
	homeMadeCam = glm::translate(homeMadeCam, displacement);
	float rotSpeed = 0.05;
	float x = 0;
	float speed = 0.01;
	
	
	//Rendering
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		callInputs();
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Draw
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.use();
		shader.setMat4("projection", projection);		
		shader.setMat4("model", model);

		
		if (camAngle == 2 && set == true) {
			homeMadeCam = glm::translate(homeMadeCam, glm::vec3(0, 0, -0.5f));
			homeMadeCam = glm::translate(homeMadeCam, glm::vec3(0,-0.5f, -1.5f));
			set = false;
			tekb = 1;
		}
		else if (camAngle == 1 && set == true) {
			homeMadeCam = glm::translate(homeMadeCam, glm::vec3(0, 0, 0.5f));
			set = false;
			teka = 1;
		}
		else {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				homeMadeCam = glm::translate(homeMadeCam, -dC);
				homeMadeCam = glm::rotate(homeMadeCam, -glm::radians(rotSpeed), glm::vec3(0, 1, 0));
				homeMadeCam = glm::translate(homeMadeCam, dC);


				modelCar = glm::rotate(modelCar, glm::radians(rotSpeed), glm::vec3(0, 0, 1));
				x += glm::radians(rotSpeed);
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
				dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				homeMadeCam = glm::translate(homeMadeCam, -dC);
				homeMadeCam = glm::rotate(homeMadeCam, glm::radians(rotSpeed), glm::vec3(0, 1, 0));
				homeMadeCam = glm::translate(homeMadeCam, dC);

				modelCar = glm::rotate(modelCar, -glm::radians(rotSpeed), glm::vec3(0, 0, 1));
				x += -glm::radians(rotSpeed);
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
				dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				homeMadeCam = glm::translate(homeMadeCam, -dC);
				homeMadeCam = glm::rotate(homeMadeCam, glm::radians(rotSpeed), glm::vec3(0, 1, 0));
				homeMadeCam = glm::translate(homeMadeCam, dC);

				modelCar = glm::rotate(modelCar, -glm::radians(rotSpeed), glm::vec3(0, 0, 1));
				x += -glm::radians(rotSpeed);
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
				dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				homeMadeCam = glm::translate(homeMadeCam, -dC);
				homeMadeCam = glm::rotate(homeMadeCam, -glm::radians(rotSpeed), glm::vec3(0, 1, 0));
				homeMadeCam = glm::translate(homeMadeCam, dC);

				modelCar = glm::rotate(modelCar, glm::radians(rotSpeed), glm::vec3(0, 0, 1));
				x += glm::radians(rotSpeed);
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
				dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
				dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
				dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
				modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
			}
		}
		
		
		
		shader.setMat4("view", homeMadeCam);
		trackModel.Draw(shader);
		
		shader1.use();
		shader1.setMat4("projection", projection);
		shader1.setMat4("model", modelCar);
		shader1.setMat4("view", homeMadeCam);
		carModel.Draw(shader1);		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}