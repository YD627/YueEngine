#pragma once

#include "Event/Event.h"
#include "Timestep.h"
#include <sstream>

namespace Yue {
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");

		virtual ~Layer() {}

		virtual void OnAttach() {};

		virtual void OnDetach() {};

		virtual void OnUpdate(Timestep ts) {};

		virtual void OnEvent(Event& event) {};

	protected:
		std::string m_DebugName;
	};
}