#pragma once

#include <iostream>
#include <chrono>

/// @brief timer class to act as a stopwatch
class Time
{
public:
	Time() = default;
	~Time() = default;

	/// @brief Start timer
	void start(void)
	{
		m_beg = hrc::now();
	}

	/// @brief Stop timer and save time between start and stop
	void stop(void)
	{
		m_elapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(hrc::now() - m_beg).count();
	}

	/// @brief Reset elapsed time
	void clear(void) { m_elapsed = 0ll; }


	/// @brief Display saved elapsed
	/// @param verbose : Display a longer message
	void display(bool verbose = false)
	{
		// Display time in nanoseconds only
		if (!verbose)
			std::cout << "Elapsed time: " << m_elapsed << " nanoseconds.\n";

		// Longer display
		else
		{
			// Divide into microseconds, milliseconds, etc
			long long		seconds = static_cast<long long>(m_elapsed / 1000000000ll);
			long long		milliseconds = static_cast<long long>((m_elapsed - (seconds * 1000000000ll)) / 1000000ll);
			long long		microseconds = static_cast<long long>((m_elapsed - (seconds * 1000000000ll) - (milliseconds * 1000000ll)) / 1000ll);
			long long		nanoseconds = static_cast<long long>(m_elapsed - (seconds * 1000000000ll) - (milliseconds * 1000000ll) - (microseconds * 1000ll));

			std::cout << "Elapsed time: " << seconds << " s, " << milliseconds << " ms, " << microseconds << " us, " << nanoseconds << " ns." << std::endl;
		}
	}

	/// @brief Get elapsed time in nanoseconds (must be previously saved with start() and stop() or getRuntime())
	/// @return : Nanoseconds between start and stop (long long)
	long long getClock(void) { return m_elapsed; }


	/// @brief Record a function's execution time
	/// @tparam T : Function's return type
	/// @tparam ...Args : Argument types to pass to function to test
	/// @param func : Function pointer to test
	/// @param ...args : Arguments to pass to func
	/// @return Recorded time in nanoseconds
	template<typename T, typename... Args>
	long long getRuntime(T(*func)(Args...), Args... args)
	{
		// Get timer ready and start
		clear();
		start();

		// Run function to test
		func(args...);

		// Stop timer
		stop();
		return m_elapsed;
	}



private:
	using hrc = std::chrono::high_resolution_clock;

	// When timer was started
	hrc::time_point		m_beg = hrc::now();

	// Stop point - start point in nanoseconds (timer must be stopped to save elapsed time)
	long long			m_elapsed = 0ll;

};