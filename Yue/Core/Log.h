#pragma once
#include<iostream>

namespace Yue {
	class Log
	{
	public:
		static void Init();

		static void CoreInfo(const std::string& message);
		static void Info(const std::string& message);

		static void CoreError(const std::string& message);
		static void Error(const std::string& message);
	};
}

#define YUE_CORE_INFO(message) ::Yue::Log::CoreInfo(message)
#define YUE_INFO(message) ::Yue::Log::Info(message)

#define YUE_CORE_ERROR(message) ::Yue::Log::CoreError(message)
#define YUE_ERROR(message) ::Yue::Log::Error(message)

