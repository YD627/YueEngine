#pragma once
#include "Window.h"
#include "Event/Event.h"
#include "Event/ApplicationEvent.h"
#include "LayerStack.h"
#include "Base.h"
#include "Timestep.h"
#include <iostream>

namespace Yue {
	class Application
	{
	public:
		Application();

		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Scope<Layer> layer);
		void PushOverlay(Scope<Layer> overlay);

		bool OnWindowClose(WindowCloseEvent& e);

		bool OnWindowResize(WindowResizeEvent& e);
	private:
		bool m_Running = true;
		Scope<Window> m_Window;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	};

	// 在客户端定义这个函数，返回一个Application的实例
	Application* CreateApplication();
}