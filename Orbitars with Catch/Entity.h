#pragma once


#include <unknwn.h>
#include <objidl.h>
#include <gdiplus.h>
#include <map>
#include <string>
#include <chrono>
#include <ctime>
#include "GameState.h"
#include "Universal_Structures.h"



enum class enEntity_Type {
	enPlayer = 0,
	enBullet,
	enAsteroid
};

class Entity
{
public:
	Entity(enEntity_Type enType_, point_t fPos_ = { 0.00, 0.00 }, point_t fVec_ = { 0.00, 0.00 }, float fRotation_ = 0.00, int ID_ = 0, RectFloat rcHitbox_ = { 0,0,0,0 }, float fRadius_ = 0.00f)
		: fPos(fPos_)
		, fVec(fVec_)
		, fRotation(fRotation_)
		, rcHitBox(rcHitbox_)
		, enType(enType_)
		, fRadius(fRadius_)
		, ID(ID_)
	{

	}

	point_t fPos; // Determines the position of the entity relative to the screen as a ratio (0 - 1)
	point_t fVec; // Determines the velocity of the entity which affects how much the position is changed
	float fRotation; // Determines the rotation of the entity
	enEntity_Type enType;//Determines which kind of entity the entity is
	std::vector<point_t> vecPoints_Of_Contact; //Determines where images can touch each other
	float fRadius; //Determines the area the entity exists as a circle radius
	RectFloat rcHitBox; // Determines the area that the entity exists
	int ID; //Distinguishes each Entity


	virtual void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp); //Renders the entity
	virtual void Update_Entity(HWND hWnd, POINT ptCursorPos); //Updates the entity
	virtual void Input_Entity(std::vector<eKeyboard> eKey_press);

	virtual bool General_Entity_Collision(Entity* pColliding_entity);
	virtual bool Advance_Entity_Collision(Entity* pColliding_entity);
	virtual void UpdateHitBox(float fDisMin); //Updates the hitbox if the size of the screen changes
protected:
	virtual bool doIntersect(const point_t& p1, const point_t& q1, const point_t& p2, const point_t& q2);
	virtual void Wrap_Entity(RECT rcClientArea);
	virtual void Wrap_Entity();
	//qt* pQt;
};

typedef std::shared_ptr<Entity> entity_t;