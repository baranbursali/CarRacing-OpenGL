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
#include "skybox.h"
#include "menu.h"

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

// Model matrices
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 modelBot = glm::mat4(1.0f);
glm::mat4 secondTreeModels[12];
glm::mat4 grassModelMat[6];


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || quitFlag == true)
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

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Car Racing Simulator", NULL, NULL);
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

bool checkCollision(glm::mat4 matrix) {
	if (matrix[3][2] > -12 && matrix[3][2] < 12 &&
		matrix[3][0] > -3.3    && matrix[3][0] < 3.3) {
		if (matrix[3][2] < 7 && matrix[3][2] > -7 &&
			matrix[3][0] > -1.3    && matrix[3][0] < 1.3) {
			return false;
		}
		return true;
	}
	return false;
}

int main()
{	
	FreeConsole();

	int startFlag = -1;
	bool backFlag = false;

	// Main menu
	initGUI();

	if (quitFlag == false) {
		createWindow();
		callbacks();
		glEnable(GL_DEPTH_TEST);
		//
		Shader shader("camera_vertex.vs", "camera_fragment.fs");
		Shader shader1("camera_vertex.vs", "camera_fragment.fs");
		Shader shader2("camera_vertex.vs", "camera_fragment.fs");
		Shader skyboxShader("skybox.vs", "skybox.fs");
		Shader cubeMapShader("cubemaps.vs", "cubemaps.fs");

		Model carModel("from/10604_slot_car_red_SG_v1_iterations-2.obj");
		Model botModel("from/10603_slot_car_blue_SG_v1_iterations-2.obj");
		Model trackModel("from/10605_Slot_Car_Race_Track_v1_L3.obj");
		Model treeModel("from/10445_Oak_Tree_v1_max2010_iteration-1.obj");
		Model grassModel("from/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");

		// Player's car
		Car carObj;
		carObj.initialize();
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 modelCar = model;
		modelCar = glm::scale(model, glm::vec3(1, 0.5, 1));
		glm::vec3 displacementCar = glm::vec3(17.5, -20, 1);
		glm::vec3 dC = glm::vec3(-1.75f, -0.1f, -2.0f);
		modelCar = glm::translate(modelCar, displacementCar);
		glm::vec3 displacement = glm::vec3(-1.75f, -0.4f, -3.0f);
		homeMadeCam = glm::translate(homeMadeCam, displacement);
		glm::mat4 temp = glm::mat4(1.0f);

		// CPU's car
		Car carObjBot;
		carObjBot.initialize();
		modelBot = glm::mat4(1.0f);
		modelBot = glm::scale(modelBot, glm::vec3(0.1f, 0.1f, 0.2f));
		modelBot = glm::rotate(modelBot, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		modelBot = glm::translate(modelBot, glm::vec3(4.0f, 0.0f, 0.0f));
		modelBot = glm::scale(modelBot, glm::vec3(1, 0.5, 1));
		glm::vec3 displacementBot = glm::vec3(17.5, -20, 1);
		glm::vec3 dCBot = glm::vec3(-1.75f, -0.1f, -2.0f);
		modelBot = glm::translate(modelBot, displacementBot);
		
		// Player's car specs
		float x = 0;
		float speed = carSpeed;
		float rotSpeed = speed * 3.0f;

		// CPU's car specs
		float xBot = 0;
		float botSpeed = 0.14f;
		float botRotSpeed = botSpeed * 3.0f;
		

		// Creating model matrices for tree models
		int t = 0;
		for (float j = -5.0f; j < 5.1f; j += 2.0f) {
			secondTreeModels[t] = glm::mat4(1.0f);
			secondTreeModels[t] = glm::translate(secondTreeModels[t], glm::vec3(0.5f, 0.0f, j));
			secondTreeModels[t] = glm::rotate(secondTreeModels[t], -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
			secondTreeModels[t] = glm::scale(secondTreeModels[t], glm::vec3(0.004f, 0.004f, 0.005f));

			secondTreeModels[t + 1] = glm::mat4(1.0f);
			secondTreeModels[t + 1] = glm::translate(secondTreeModels[t + 1], glm::vec3(-0.5f, 0.0f, j - 1.0f));
			secondTreeModels[t + 1] = glm::rotate(secondTreeModels[t + 1], -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
			secondTreeModels[t + 1] = glm::scale(secondTreeModels[t + 1], glm::vec3(0.004f, 0.004f, 0.005f));
			t += 2;
		}

		// Creating model matrices for grass models
		t = 0;
		for (float g = -6.0f; g < 6.6f; g += 2.5f) {
			grassModelMat[t] = glm::mat4(1.0f);
			grassModelMat[t] = glm::translate(grassModelMat[t], glm::vec3(0.0f, 0.0f, g));
			grassModelMat[t] = glm::rotate(grassModelMat[t], -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
			grassModelMat[t] = glm::scale(grassModelMat[t], glm::vec3(0.01f, 0.01f, 0.004f));
			t++;
		}

		int treeIndices[6] = { 1,2,5,6,9,10 };

		// Skybox
		skybox skybox;
		skybox.initialize();

		// Cubemap Textures
		vector<std::string> facesNevada
		{
			"from/Nevada/nevada_rt.tga",
			"from/Nevada/nevada_lf.tga",
			"from/Nevada/nevada_up.tga",
			"from/Nevada/nevada_dn.tga",
			"from/Nevada/nevada_ft.tga",
			"from/Nevada/nevada_bk.tga"
		};
		vector<std::string> facesMidnight
		{
			"from/Midnight/midnight-silence_rt.tga",
			"from/Midnight/midnight-silence_lf.tga",
			"from/Midnight/midnight-silence_up.tga",
			"from/Midnight/midnight-silence_dn.tga",
			"from/Midnight/midnight-silence_ft.tga",
			"from/Midnight/midnight-silence_bk.tga"
		};
		vector<std::string> facesArctic
		{
			"from/Arctic/arctic-ice_rt.tga",
			"from/Arctic/arctic-ice_lf.tga",
			"from/Arctic/arctic-ice_up.tga",
			"from/Arctic/arctic-ice_dn.tga",
			"from/Arctic/arctic-ice_ft.tga",
			"from/Arctic/arctic-ice_bk.tga"
		};

		vector<std::string> maps[3] = {facesArctic, facesMidnight, facesNevada};
		unsigned int cubemapTexture = loadCubemap(maps[index]);

		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);


		//Game loop
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
			/*
			printf("%f %f %f %f \n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n \n XXXX \n", modelCar[0][0]
				, modelCar[0][1], modelCar[0][2],  modelCar[0][3],  modelCar[1][0], modelCar[1][1], modelCar[1][2], modelCar[1][3]
				, modelCar[2][0], modelCar[2][1], modelCar[2][2], modelCar[2][3], modelCar[3][0], modelCar[3][1], modelCar[3][2]
				, modelCar[3][3]);
			*/
			if (camAngle == 2 && set == true) {
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(0, 0, -0.5f));
				homeMadeCam = glm::translate(homeMadeCam, glm::vec3(0, -0.5f, -1.5f));
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
					temp = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, -dC);
						homeMadeCam = glm::rotate(homeMadeCam, -glm::radians(rotSpeed), glm::vec3(0, 1, 0));
						homeMadeCam = glm::translate(homeMadeCam, dC);


						modelCar = glm::rotate(modelCar, glm::radians(rotSpeed), glm::vec3(0, 0, 1));
						x += glm::radians(rotSpeed);
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
						dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					}
				}
				else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
					temp = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, -dC);
						homeMadeCam = glm::rotate(homeMadeCam, glm::radians(rotSpeed), glm::vec3(0, 1, 0));
						homeMadeCam = glm::translate(homeMadeCam, dC);

						modelCar = glm::rotate(modelCar, -glm::radians(rotSpeed), glm::vec3(0, 0, 1));
						x += -glm::radians(rotSpeed);
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
						dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					}
				}
				else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
					temp = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, -dC);
						homeMadeCam = glm::rotate(homeMadeCam, glm::radians(rotSpeed), glm::vec3(0, 1, 0));
						homeMadeCam = glm::translate(homeMadeCam, dC);

						modelCar = glm::rotate(modelCar, -glm::radians(rotSpeed), glm::vec3(0, 0, 1));
						x += -glm::radians(rotSpeed);
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
						dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					}
				}
				else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
					temp = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, -dC);
						homeMadeCam = glm::rotate(homeMadeCam, -glm::radians(rotSpeed), glm::vec3(0, 1, 0));
						homeMadeCam = glm::translate(homeMadeCam, dC);

						modelCar = glm::rotate(modelCar, glm::radians(rotSpeed), glm::vec3(0, 0, 1));
						x += glm::radians(rotSpeed);
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
						dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					}
				}
				else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
					if (startFlag == -1){
						startFlag = 1;
					}
					temp = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f));
						dC += glm::vec3(sin(x)*speed / 10.0f, 0.0, cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, speed, 0.0f));
					}

				}
				else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
					temp = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					if (checkCollision(temp)) {
						homeMadeCam = glm::translate(homeMadeCam, glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f));
						dC += glm::vec3(-sin(x)*speed / 10.0f, 0.0, -cos(x)*speed / 10.0f);
						modelCar = glm::translate(modelCar, glm::vec3(0.0f, -speed, 0.0f));
					}
				}
			}

			// Start race
			if (startFlag == 1){
				
				//Bot forward
				dCBot += glm::vec3(sin(x)*botSpeed / 10.0f, 0.0, cos(x)*botSpeed / 10.0f);
				modelBot = glm::translate(modelBot, glm::vec3(0.0f, botSpeed, 0.0f));
				
				//Bot forward + left
				if (modelBot[3][2] <= -5.15) {
					modelBot = glm::rotate(modelBot, glm::radians(botRotSpeed * 1.85f), glm::vec3(0, 0, 1));
					xBot += glm::radians(botRotSpeed);
					dCBot += glm::vec3(sin(xBot)*botSpeed / 10.0f, 0.0, cos(xBot)*botSpeed / 10.0f);
					modelBot = glm::translate(modelBot, glm::vec3(0.0f, botSpeed, 0.0f));
				}

				//Bot forward
				else if (modelBot[3][2] >= 6.0f) {
					backFlag = true;
					startFlag = 0;
					xBot = 0;
					dCBot = glm::vec3(-1.75f, -0.1f, -2.0f);
				}
			}
			//Bot forward + left
			else if (backFlag == true && startFlag == 0) {
				
				if (modelBot[3][2] >= 7.0f) {
					modelBot = glm::rotate(modelBot, glm::radians(botRotSpeed * 0.9f), glm::vec3(0, 0, 1));
					xBot += glm::radians(botRotSpeed);
					dCBot += glm::vec3(sin(xBot)*botSpeed / 10.0f, 0.0, cos(xBot)*botSpeed / 10.0f);
					modelBot = glm::translate(modelBot, glm::vec3(0.0f, botSpeed, 0.0f));
				}
				else if (modelBot[3][2] <= 2.5f){
					dCBot += glm::vec3(sin(x)*botSpeed / 10.0f, 0.0, cos(x)*botSpeed / 10.0f);
					modelBot = glm::translate(modelBot, glm::vec3(0.0f, botSpeed, 0.0f));
				}
				else {	
					startFlag = 1;
					backFlag = false;
				}
			}
			
			shader.setMat4("view", homeMadeCam);
			trackModel.Draw(shader);

			// Drawing trees
			for (int k = 0; k < 6; k++) {
				shader2.setMat4("model", secondTreeModels[treeIndices[k]]);
				treeModel.Draw(shader2);
			}

			// Drawing grass
			for (int l = 0; l < 6; l++) {
				shader2.setMat4("model", grassModelMat[l]);
				grassModel.Draw(shader2);
			}

			shader1.use();
			shader1.setMat4("projection", projection);
			shader1.setMat4("model", modelCar);
			shader1.setMat4("view", homeMadeCam);
			carModel.Draw(shader1);

			shader1.setMat4("model", modelBot);
			botModel.Draw(shader1);

			// Drawing skybox as last
			glDepthFunc(GL_LEQUAL);  
			skyboxShader.use();
			glm::mat4 skyView = camera.GetViewMatrix();
			skyView = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
			skyboxShader.setMat4("view", skyView);
			skyboxShader.setMat4("projection", projection);

			// Skybox cube
			glBindVertexArray(skybox.VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); 

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	}
	
	glfwTerminate();
	return 0;
}
