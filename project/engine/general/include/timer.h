#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point timePoint;

class timer {
private:
	timePoint startTime;
	timePoint currentTime;
	timePoint lastTime;
	float deltaTime;
	float time;

public:
	timer() {
		startTime = std::chrono::high_resolution_clock::now();
		lastTime = startTime;
	}

	float getTime() { return time; }
	float getDeltaTime() { return deltaTime; }

	void updateTime() {
		currentTime = std::chrono::high_resolution_clock::now();

		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	
		lastTime = currentTime;
	}

private:
};

#endif // TIMER_H_
