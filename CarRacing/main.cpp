#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.h"
#include "callbacks.h"
#include "inputs.h"
#include "car.h"

using namespace std;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
GLFWwindow* window;

int createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

void callCallbacks() {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void callInputs() {
	processInput(window);
}

void drawCar(Car carObj) {

	Shader shader("camera_vertex.vs", "camera_fragment.fs");

	shader.use();
	glBindVertexArray(carObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);


}

int main()
{	
	createWindow();
	callCallbacks();

	//
	Car carObj;
	carObj.initialize();

	//Rendering
	while (!glfwWindowShouldClose(window))
	{
		callInputs();
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		drawCar(carObj);

		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}