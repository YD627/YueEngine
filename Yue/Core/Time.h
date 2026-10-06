#pragma once
#ifndef TIME_H
#define TIME_H

class Time
{
public:
	static void Update();

	static float DeltaTime();

	static float TotalTime();

	static int FPS();

private:
	static float s_DeltaTime;	// 上一帧与当前帧的时间差
	static float s_LastFrame;	// 上一帧的时间
	static float s_TotalTime;	// 从应用程序启动到当前的总时间

	static int s_FPS;			// 当前帧率
	static float s_FPSTimer;	// 用于计算帧率的计时器
	static int s_FrameCount;	// 计数器，用于计算帧率
};

#endif // !TIME_H
