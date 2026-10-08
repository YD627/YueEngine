#include "Application.h"
#include "Input.h"
#include "Log.h"

#include "Renderer/Renderer.h"

#include <iostream>
#include <chrono>

namespace Yue {
	Application::Application() {
		Log::Init();
		YUE_CORE_INFO("Application Created");

		m_Window = Window::Create();

		m_Window->SetEventCallback([this](Event& e) {OnEvent(e);});

		Renderer::Init();
	}

	Application::~Application() {
	}

	void Application::Run() {

		while (m_Running) {
			// Update application logic here

			// Rendering code here

			float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

			float timeStep = std::chrono::duration<float, std::chrono::seconds::period>(time - m_LastFrameTime).count();

			m_LastFrameTime = time;

			Timestep ts(timeStep);

			m_Window->Update();

			for (auto& layer : m_LayerStack) {
				layer->OnUpdate(ts);
			}

			if (m_Window->ShouldClose()) {
				m_Running = false;
			}
		}
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) {
			return OnWindowClose(event);
		});

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
			return OnWindowResize(event);
		});

		for (auto it = m_LayerStack.end();it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled) break;
		}
	}

	void Application::PushLayer(Scope<Layer> layer) {
		m_LayerStack.PushLayer(std::move(layer));
	}

	void Application::PushOverlay(Scope<Layer> overlay) {
		m_LayerStack.PushOverlay(std::move(overlay));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		std::cout << "Resize: " << e.GetWidth() << " " << e.GetHeight() << std::endl;
		return true;
	}
}