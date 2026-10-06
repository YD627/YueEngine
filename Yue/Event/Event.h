#pragma once

namespace Yue {
	enum class EventTypes
	{
		None = 0,

		WindowClose,
		WindowResize,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
	};
	enum EventCategories
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

	#define EVENT_CLASS_TYPE(type) static EventTypes GetStaticType() { return EventTypes::type;}\
									virtual EventTypes GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }

	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual EventTypes GetEventType() const = 0;

		virtual int GetCategoryFlags() const = 0;

		virtual const char* GetName() const = 0;

		bool IsInCategory(EventCategories category)
		{
			return GetCategoryFlags() & category;
		}

		bool Handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e):m_Event(e){}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};
}