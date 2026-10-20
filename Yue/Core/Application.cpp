#include "Application.h"
#include "Input.h"
#include "Log.h"

#include "Renderer/Renderer.h"

#include <iostream>
#include <chrono>
#include <algorithm>

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

		auto lastFrameTime = std::chrono::steady_clock::now();

		while (m_Running) {
			// Update application logic here

			// Rendering code here

			auto currentFrameTime = std::chrono::steady_clock::now();
			float timeStep = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
			lastFrameTime = currentFrameTime;

			// Do not simulate an enormous step after a pause or a long stall.
			timeStep = std::min(timeStep, 0.1f);

			Timestep ts(timeStep);

			for (auto& layer : m_LayerStack) {
				layer->OnUpdate(ts);
			}

			m_Window->Update();

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
