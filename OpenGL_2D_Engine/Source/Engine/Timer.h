
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
private:
	std::chrono::steady_clock::time_point start, end;

public:
	void StartTimer() { start = std::chrono::steady_clock::now(); }
	void StopTimer() { end = std::chrono::steady_clock::now(); }
	float ElapsedTimeMS() { return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); }
	float ElapsedTimeSeconds() { return std::chrono::duration_cast<std::chrono::seconds>(end - start).count(); }
};

#endif
