#include "Yue/Core/EntryPoint.h"
#include "SandboxLayer.h"

class SandboxApp : public Yue::Application
{
public:
	SandboxApp()
	{
		PushLayer(new SandboxLayer());
	}
};

Yue::Application* Yue::CreateApplication()
{
	return new SandboxApp();
}