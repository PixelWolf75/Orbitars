#pragma once
#include <windows.h>
#include "Entity.h"
#include "qt.h"


class Bullet_Factory;
class IBullet : public Entity
{
public:
	virtual ~IBullet() = default;


	virtual	void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) = 0;
	virtual void Update_Entity(HWND hWnd, POINT ptCursor) = 0;
	virtual int Return_Player_ID() = 0;
	//virtual bool Return_bIsInQT() = 0;
	//virtual void Set_bIsInQT(bool bIsInQt) = 0;
	virtual bool Return_bHasCollided() = 0;
	virtual void Set_bHasCollided(bool bHasCollided) = 0;
	virtual bool Has_Bullet_Expired() = 0;

protected:
	IBullet(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_);
	friend class Bullet_Factory;
	virtual void UpdateHitBox(float fDisMin)=0;
};

typedef std::shared_ptr<IBullet> bullet_t;

class Bullet_Factory
{
public:
	static std::shared_ptr<IBullet> Create(qt * pQt, point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_);
};

