#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
class Timer
{
public:

	// Create trimer
	Timer(void)
		: m_lastFrameTime(0.00)
	{}

	// Destroy timer
	~Timer() = default;


	void StartTimer()
	{
		m_timerStart = glfwGetTime();
	}

	double StopTimer() const
	{
		double res = glfwGetTime() - m_timerStart;
		return res;
	}

	double ResumeFrame()
	{
		m_lastFrameTime = glfwGetTime() - GetDeltaTime();

		m_newFrame = true;
	}

	// Get elapsed time between two frames
	double GetDeltaTime(void)
	{
		// Record current time and subtract last recorded time
		static double deltaTime = 0.f;


		if (m_newFrame)
		{
			double currentTime = glfwGetTime();

			deltaTime = currentTime - m_lastFrameTime;

			// Set new value for next call
			m_lastFrameTime = currentTime;
		}

		m_newFrame = false;

		//if (deltaTime > 0.01f)
		//{
		//	return 0.01f;
		//}

		return deltaTime;
	}

	void SetNewFrame(bool status)
	{
		m_newFrame = status;
	}

private:

	// Last recorded time
	double	m_lastFrameTime;
	double	m_timerStart = 0.0;
	bool	m_newFrame = true;
};