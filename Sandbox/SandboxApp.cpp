#include "Yue/Core/EntryPoint.h"
#include "SandboxLayer.h"

class SandboxApp : public Yue::Application
{
public:
	SandboxApp()
	{
		PushLayer(Yue::CreateScope<SandboxLayer>());
	}
};

Yue::Application* Yue::CreateApplication()
{
	return new SandboxApp();
}