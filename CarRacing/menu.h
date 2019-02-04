#ifndef MENU_H
#define MENU_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <nanogui/nanogui.h>
#include <nanogui/button.h>
#include <nanogui/formhelper.h>

#include <iostream>

using namespace nanogui;

bool quitFlag = false;
int index;
float carSpeed;

int initGUI() {

	nanogui::init();

	Screen *screen;
	screen = new Screen(Vector2i(600, 600), "Car Racing Simulator");

	Window *window = new Window(screen, "Main Menu");
	window->setPosition(Vector2i(10, 10));
	window->setLayout(new GroupLayout());

	
	Button *b;
	
	new Label(window, "Choose Map", "sans-bold");
	b = new Button(window, "Arctic");
	b->setFlags(Button::ToggleButton);
	b->setCallback([] { index = 0; });
	b = new Button(window, "Northern Lights");
	b->setFlags(Button::ToggleButton);
	b->setCallback([] { index = 1; });
	b = new Button(window, "Grand Canyon");
	b->setFlags(Button::ToggleButton);
	b->setCallback([] { index = 2; });

	
	new Label(window, "Choose Difficulty", "sans-bold");
	b = new Button(window, "Easy");
	b->setFlags(Button::RadioButton);
	b->setCallback([] { carSpeed = 0.20f; });
	b = new Button(window, "Normal");
	b->setFlags(Button::RadioButton);
	b->setCallback([] { carSpeed = 0.17f; });
	b = new Button(window, "Hard");
	b->setFlags(Button::RadioButton);
	b->setCallback([] { carSpeed = 0.14f; });

	/*
	new Label(window, "Lightning", "sans-bold");
	b = new Button(window, "Day Light");
	b->setFlags(Button::RadioButton);
	b = new Button(window, "Night");
	b->setFlags(Button::RadioButton);
	*/

	new Label(window, "You can change the camera angle by using keys 1 and 2!", "sans-bold");

	b = new Button(window, "START GAME");
	b->setCallback([screen] { screen->setVisible(false); quitFlag = false; });
	b = new Button(window, "QUIT GAME");
	b->setCallback([screen] { screen->setVisible(false); quitFlag = true; });

	screen->setVisible(true);
	screen->performLayout();
	window->center();
	
	nanogui::mainloop();

	nanogui::shutdown();

	return 0;
}
#endif