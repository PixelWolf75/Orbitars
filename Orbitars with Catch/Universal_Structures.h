#pragma once
#include <windows.h>
#include <cmath>
#define DEFAULT_NUM_OF_STARS 100
#define MINIMUM_BRIGHTNESS 127
#define FRAME_RATE 60
#define PERIOD 4
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 500
#define VELOCITY_PERIOD 250
#define VELOCITY_BULLET 0.005f
#define MINIMUM_ANGLE 0.1f
#define PLAYER_MAX_HEALTH 100
#define MAX_NUM_OF_ASTEROIDS 0
#define MIN_NUM_OF_ASTEROIDS 1
#define PI 3.14159265f
#define ENTITY_EXPIRY 30.00f
#define RESET_LIMIT 10

enum class eKeyboard {
	Left = 0,
	Right,
	Up,
	Down,
	Space,
	Enter,
	Left_Click,
	Right_Click,
	None
};

struct point_t {
	float fx;
	float fy;

	point_t& operator+=(const point_t& A) {
		this->fx += A.fx;
		this->fy += A.fy;
		return *this;
	}

	point_t operator/(const point_t& A) {
		return { this->fx / A.fx, this->fy / A.fy };
	}
	point_t operator/(float A) {
		return { this->fx / A, this->fy / A };
	}
	point_t operator*(const point_t& A) {
		return { this->fx * A.fx, this->fy * A.fy };
	}
	point_t operator*(float A) {
		return { this->fx * A, this->fy * A };
	}

	point_t operator+(const point_t& A) {
		return { this->fx + A.fx, this->fy + A.fy };
	}

	point_t operator%(const float& A) {
		return { fmod(this->fx + A, A), fmod(this->fy + A, A)};
	}

};

struct Window_Properties
{
	HDC hdcMem;
	void* lpBitMapData;
	int iWidth;
	int iHeight;
};

