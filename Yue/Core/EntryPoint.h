#pragma once
#include "Application.h"

extern Yue::Application* Yue::CreateApplication();

int main()
{
	auto app = Yue::CreateApplication();

	app->Run();

	delete app;
}