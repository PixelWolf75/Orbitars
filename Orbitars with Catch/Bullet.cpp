#include "Bullet.h"

using namespace Gdiplus;
using namespace std;

class Bullet : public IBullet
{
public:
	Bullet(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_, qt* pQt_);
	~Bullet();

	int ID_Player; //ID of the player that fired it
	//bool bIsInQT;

	bool bHasCollided; //If the bulet has collided remove
	std::chrono::system_clock::time_point chStart;

	virtual void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) override;
	virtual void Update_Entity(HWND hWnd, POINT ptPosCursor) override;
	virtual int Return_Player_ID() override;
	//virtual bool Return_bIsInQT() override;
	//virtual void Set_bIsInQT(bool bIsInQt) override;
	virtual bool Return_bHasCollided() override;
	virtual void Set_bHasCollided(bool bHasCollided) override;
	virtual bool Has_Bullet_Expired() override;
	qt* pQt;
	//void Input_Entity(eKeyboard eKey_press) override;

protected:
	virtual void UpdateHitBox(float fDisMin) override;

};


Bullet::Bullet(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_, qt* pQt_)
	: IBullet(fPos_, fVec_, fRotation_, rcHitBox_, enType_,  ID_Player_, ID_)
	, ID_Player(ID_Player_)
	//, bIsInQT(false)
	, pQt(pQt_)
	, bHasCollided(false)
{
	double fVecVector = sqrt(pow(fVec.fx, 2) + pow(fVec.fy, 2));
	fVec.fx = (float)((fVecVector + VELOCITY_BULLET) * cosf(fRotation - PI / 2));
	fVec.fy = (float)((fVecVector + VELOCITY_BULLET) * sinf(fRotation - PI / 2));

	fRadius = sqrtf((float)(pow(rcHitBox.right, 2) + pow(rcHitBox.bottom, 2)));

	chStart = std::chrono::system_clock::now();
}

Bullet::~Bullet()
{
	pQt->remove(this);
}

void Bullet::Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t* pMapImgs, Window_Properties wpWinProp)
{
	RECT rcClientArea;
	GetClientRect(hWnd, &rcClientArea);
	RectFloat rcBulletArea;

	float fDisMin = (float)fmin(rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top);

	UpdateHitBox(fDisMin);

	float fDisMax = (float)fmax(rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top);
	float fDifference = fDisMax - fDisMin;

	point_t ptPixel;
	if (fDisMax == rcClientArea.right - rcClientArea.left)
	{
		ptPixel = { (fPos.fx * fDisMin) + (fDifference / 2) , (fPos.fy * fDisMin) };
	}
	else
	{
		ptPixel = { (fPos.fx * fDisMin), (fPos.fy * fDisMin) + (fDifference / 2) };
	}
	rcBulletArea = { (ptPixel.fx) + rcHitBox.left * (fDisMin)
		, (ptPixel.fy) + rcHitBox.top * (fDisMin)
		, (ptPixel.fx) + rcHitBox.right * (fDisMin)
		, (ptPixel.fy) + rcHitBox.bottom * (fDisMin) };

	Gdiplus::Graphics g(wpWinProp.hdcMem);
	g.ResetTransform();
	g.RotateTransform(fRotation);
	RectF sImgBullet(-(rcHitBox.right * (fDisMin) / 2), -(rcHitBox.bottom * (fDisMin) / 2), rcHitBox.right * (fDisMin), rcHitBox.bottom * (fDisMin));
	g.TranslateTransform(ptPixel.fx, ptPixel.fy, Gdiplus::MatrixOrderAppend);
	g.DrawImage(pMapImgs->at("Bullet").get(), sImgBullet);

	vecPoints_Of_Contact.push_back({ 0.00f, 0.00f });
}

void Bullet::Update_Entity(HWND hWnd, POINT ptCursor)
{
	fPos += fVec;

	// RECT rc;
	// GetClientRect(hWnd, &rc);
	// std::chrono::seconds::count();

	Wrap_Entity();
	this->pQt->update(this);
}

int Bullet::Return_Player_ID()
{
	return ID_Player;
}

//bool Bullet::Return_bIsInQT()
//{
//	return bIsInQT;
//}
//
//void Bullet::Set_bIsInQT(bool bIsInQt_)
//{
//	bIsInQT = bIsInQt_;
//}

bool Bullet::Return_bHasCollided()
{
	return bHasCollided;
}

void Bullet::Set_bHasCollided(bool bHasCollided_)
{
	bHasCollided = bHasCollided_;
}

bool Bullet::Has_Bullet_Expired()
{
	std::chrono::system_clock::time_point chEnd = std::chrono::system_clock::now();
	std::chrono::duration<float> elasped_time = chEnd - chStart;
	if (ENTITY_EXPIRY < elasped_time.count() || bHasCollided)
	{
		int y = 0;
		return true;
	}
	return false;
}

void Bullet::UpdateHitBox(float fDisMin)
{
	rcHitBox = { 0 / fDisMin
				,0 / fDisMin
				,4 / fDisMin
				,4 / fDisMin };

	fRadius = sqrtf((float)(pow(rcHitBox.right, 2) + pow(rcHitBox.bottom, 2)));
}

IBullet::IBullet(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_)
	: Entity(enType_, fPos_, fVec_, fRotation_, ID_, rcHitBox_)
{
}

std::shared_ptr<IBullet> Bullet_Factory::Create(qt* pQt, point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitBox_, enEntity_Type enType_, int ID_Player_, int ID_)
{
	std::shared_ptr<IBullet> pBullet = shared_ptr<IBullet>(new Bullet(fPos_, fVec_, fRotation_, rcHitBox_, enType_, ID_Player_, ID_, pQt));
	pQt->add(pBullet.get());
	return pBullet;
}
