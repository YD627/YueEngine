#pragma once
#include <sstream>
#include <functional>
#include "Event/Event.h"
#include "Base.h"

namespace Yue {
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Yue Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height) {
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void Update() = 0;

		virtual bool ShouldClose() const = 0;

		virtual unsigned int GetWidth() const = 0;

		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}
