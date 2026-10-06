#include "Time.h"
#include <GLFW/glfw3.h>

// 初始化时间相关的静态成员变量
float Time::s_DeltaTime = 0.0f;
float Time::s_LastFrame = 0.0f;
float Time::s_TotalTime = 0.0f;

int Time::s_FPS = 0;
float Time::s_FPSTimer = 0.0f;
int Time::s_FrameCount = 0;


void Time::Update()
{
	// 获取当前时间
	float currentFrame = static_cast<float>(glfwGetTime());
	// 计算上一帧与当前帧的时间差
	s_DeltaTime = currentFrame - s_LastFrame;
	// 更新上一帧的时间为当前时间
	s_LastFrame = currentFrame;
	// 总时间
	s_TotalTime = currentFrame;

	s_FrameCount++;
	s_FPSTimer += s_DeltaTime;
	if (s_FPSTimer >= 1.0f)
	{
		s_FPS = s_FrameCount;
		s_FrameCount = 0;
		s_FPSTimer = 0.0f;
	}
}

float Time::DeltaTime()
{
	return s_DeltaTime;
}

float Time::TotalTime()
{
	return s_TotalTime;
}

int Time::FPS()
{
	return s_FPS;
}