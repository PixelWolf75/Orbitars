#pragma once
#include "Entity.h"
#include <vector>
#include <random>
#include "qt.h"

class Asteroid_Factory;

class IAsteroid : public Entity 
{
public:
	virtual ~IAsteroid() = default;

	virtual	void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) = 0;
	virtual void Update_Entity(HWND hWnd, POINT ptCursor) = 0;
	virtual void Input_Entity(std::vector<eKeyboard>) = 0;
	virtual int Return_Number_of_side() = 0;

	virtual void Reset(HWND hWnd) = 0;
	virtual std::shared_ptr<IAsteroid> Crumble(point_t fVecA, int iAsteroidCount) = 0;
	virtual bool Advance_Entity_Collision(Entity* pColliding_entity)=0;
	virtual bool Return_bHasExpired() = 0;
	virtual void RandomLocation() = 0;

protected:
	
	IAsteroid(point_t fPos, point_t fVec_, float fRotation_, int  iNum_of_sides_, 
		float spin_speed, enEntity_Type enType_, int ID_, int iSize_ = 500);
	friend class Asteroid_Factory;
};

typedef std::shared_ptr<IAsteroid> asteroid_t;

class Asteroid_Factory
{
public:
	static std::shared_ptr<IAsteroid> Create(qt* pQt, point_t fPos, point_t fVec_, float fRotation_, int  iNum_of_sides_,
		float spin_speed, enEntity_Type enType_, int ID_, int iSize_ = 500);
};


