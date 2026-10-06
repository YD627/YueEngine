#pragma once
#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <sstream>

namespace Yue {
	class GLFWWindow : public Window 
	{
	public:
		GLFWWindow(const WindowProps& props);
		virtual ~GLFWWindow();

		void Update() override;

		bool ShouldClose() const override;

		unsigned int GetWidth() const override;
		unsigned int GetHeight() const override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

	private:
		void Init(const WindowProps& props);

	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}