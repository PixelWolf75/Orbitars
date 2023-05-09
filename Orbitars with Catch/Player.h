#pragma once
#include <Windows.h>
#include "Entity.h"
#include "Bullet.h"
#include <vector>
#include "qt.h"

class Player_Factory;

class IPlayer : public Entity
{
public:
	virtual ~IPlayer() = default;

	virtual	void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) = 0;
	virtual void Update_Entity(HWND hWnd, POINT ptCursor) = 0;
	virtual void Input_Entity(std::vector<eKeyboard>) = 0;
	virtual void Create_Bullet(qt* pQt, int ID) = 0;
	virtual bool Player_taken_damage() = 0;
	virtual bool Return_State() = 0;
	virtual void Player_Score_Increment() = 0;
	virtual std::vector<std::shared_ptr<IBullet>> Return_Shared_Bullets() = 0;


protected:
	IPlayer(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_);
	friend class Player_Factory;
	virtual void UpdateHitBox(float fDisMin)=0;
};

typedef std::shared_ptr<IPlayer> player_t;

class Player_Factory 
{
public:
	static std::shared_ptr<IPlayer> Create(qt* pQt, point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_);
};

