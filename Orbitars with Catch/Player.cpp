#include "..\Test\qt.h"
#include "Player.h"
#include <algorithm>
#include <math.h>

using namespace Gdiplus;
using namespace std;

class Player : public IPlayer
{
public:
	Player(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_, qt* pQt_);
	~Player();

	bool bIsThrusters;
	int iHitPoints;
	//std::vector<IBullet> vecBullets;
	std::vector<std::shared_ptr<IBullet>> vecSharedBullets;
	int iTotalBullets;

	qt* pQt;

	int iScore;
	std::map<std::string, Gdiplus::Image*> mapPlayerImgs;
	void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) override;
	void Update_Entity(HWND hWnd, POINT ptPosCursor) override;
	void Input_Entity(vector<eKeyboard> eKey_press) override;
	virtual void Create_Bullet(qt* pQt, int ID) override;
	virtual bool Player_taken_damage() override;
	virtual bool Return_State() override;
	bool bIsAlive; //State that the Player is in
	std::vector<std::shared_ptr<IBullet>> VecSharedBullet_Access();
	virtual void Player_Score_Increment() override;
	virtual std::vector<std::shared_ptr<IBullet>> Return_Shared_Bullets() override;
	RectFloat rcBulletRatio;

protected:
	virtual void UpdateHitBox(float fDisMin) override;

};


Player::Player(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_, qt* pQt_)
	: IPlayer(fPos_, fVec_, fRotation_, rcHitbox_, ID_, bIsThrusters, enType_, rcBulletRatio_)
	, bIsThrusters(bIsThrusters_)
	, bIsAlive(true)
	, iHitPoints(100)
	, iScore(0)
	, rcBulletRatio(rcBulletRatio_)
	, iTotalBullets(0)
	, pQt(pQt_)
{
	float half_x = rcHitBox.right / 2;
	float half_y = rcHitBox.bottom / 2;

	//Point determined by pixels
	vecPoints_Of_Contact.push_back({ 0.5f * rcHitBox.right, 00.00f });
	vecPoints_Of_Contact.push_back({ rcHitBox.right / 9.00f, 2 * rcHitBox.bottom / 9.00f });
	vecPoints_Of_Contact.push_back({ 23.00f * rcHitBox.right / 27.00f, 2 * rcHitBox.bottom / 9.00f });
	vecPoints_Of_Contact.push_back({ rcHitBox.right / 9.00f, 22.00f * rcHitBox.bottom / 27.00f });
	vecPoints_Of_Contact.push_back({ 23.00f * rcHitBox.right / 27.00f, 22.00f * rcHitBox.bottom / 27.00f });
	for (auto& p: vecPoints_Of_Contact) {
		p.fx -= half_x;
		p.fy -= half_y;
	}
	fRadius = (float)sqrt(pow((rcHitBox.right), 2) + pow(rcHitBox.bottom, 2));
}

Player::~Player()
{
	pQt->remove(this);
}

void Player::Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t* pMapImgs, Window_Properties wpWinProp)
{
	RECT rcClientArea;
	GetClientRect(hWnd, &rcClientArea);
	RectFloat rcShipArea;

	float fDisMin = float(min(rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top));
	
	//UpdateHitBox(fDisMin); no longer needed as the area is rendered based on a constant resolution

	float fDisMax = float(max(rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top));
	float fDifference = fDisMax - fDisMin;

	auto point_to_pixel = [&fDisMax, &fDisMin, &fDifference, &rcClientArea](point_t new_pos) {
		point_t ptPixel;

		if (fDisMax == rcClientArea.right - rcClientArea.left)
		{
			ptPixel = { ((new_pos.fx) * fDisMin) + (fDifference / 2) , (new_pos.fy * fDisMin) };
		}
		else
		{
			ptPixel = { (new_pos.fx * fDisMin), (new_pos.fy * fDisMin) + (fDifference / 2) };
		}
		return ptPixel;
	};

	point_t ptPixel = point_to_pixel(fPos);
	
	rcShipArea = { (ptPixel.fx) + rcHitBox.left * (fDisMin)
		, (ptPixel.fy) + rcHitBox.top * (fDisMin)
		, (ptPixel.fx) + rcHitBox.right * (fDisMin)
		, (ptPixel.fy) + rcHitBox.bottom * (fDisMin) };


	/*
	point_t ptPixel = { fPos.fx * (rcClientArea.right - rcClientArea.left) ,
						fPos.fy * (rcClientArea.bottom - rcClientArea.top) };
	rcShipArea = {(ptPixel.fx) + rcHitBox.left * (rcClientArea.right - rcClientArea.left)
		, (ptPixel.fy) + rcHitBox.top * (rcClientArea.bottom - rcClientArea.top)
		, (ptPixel.fx) + rcHitBox.right * (rcClientArea.right - rcClientArea.left)
		, (ptPixel.fy) + rcHitBox.bottom * (rcClientArea.bottom - rcClientArea.top) };
	*/

	Gdiplus::Graphics g(wpWinProp.hdcMem);
	g.ResetTransform();
	Gdiplus::Rect sImgRectShip(int(-(rcHitBox.right * (fDisMin) / 2)),
		int(-(rcHitBox.bottom * (fDisMin) / 2)),
		int(rcHitBox.right * (fDisMin)),
		int(rcHitBox.bottom * (fDisMin)));

	g.RotateTransform(fRotation * (180 / PI));
	g.TranslateTransform(ptPixel.fx, ptPixel.fy, Gdiplus::MatrixOrderAppend);
	//pClient_ScreenINFO->mapImgFiles;
	//Image* p = pMapImgs->at("Player_Thrust").get();

	if (bIsAlive)
	{
		g.DrawImage(bIsThrusters ? pMapImgs->at("Player_Thruster").get() : pMapImgs->at("Player").get(), sImgRectShip);
		Gdiplus::Color c = (iHitPoints > 66) ? Color(0, 255, 0) :
			(iHitPoints > 33) ? Color(255, 255, 0) :
			Color(255, 0, 0);
		RectF rcShipHealth(-(rcHitBox.right * (fDisMin) / 2),
			(rcHitBox.bottom * (fDisMin) / 2),
			rcHitBox.right * (fDisMin)*iHitPoints / PLAYER_MAX_HEALTH,
			3);

		Gdiplus::SolidBrush b(c);
		g.FillRectangle(&b, rcShipHealth);
	}


	RectF sImgBullet(-(rcBulletRatio.right * (fDisMin) / 2),
		-(rcBulletRatio.bottom * (fDisMin) / 2),
		rcBulletRatio.right * (fDisMin),
		rcBulletRatio.bottom * (fDisMin));
	for (auto poc : this->vecPoints_Of_Contact) {
		g.ResetTransform();
		point_t p2 = { poc.fx - 0.5f, poc.fy };
		//point_t offset = point_to_pixel(p2);
		point_t offset;

		if (fDisMax == rcClientArea.right - rcClientArea.left)
		{
			offset = { ((poc.fx) * fDisMin) , (poc.fy * fDisMin) };
		}
		else
		{
			offset = { (poc.fx * fDisMin), (poc.fy * fDisMin) };
		}

		//offset.fx += sImgRectShip.GetLeft();
		//offset.fy += sImgRectShip.GetTop();
		g.TranslateTransform(offset.fx, offset.fy, Gdiplus::MatrixOrderAppend);

		g.RotateTransform(fRotation * (180 / PI), Gdiplus::MatrixOrderAppend);
		
		g.TranslateTransform(ptPixel.fx, ptPixel.fy, Gdiplus::MatrixOrderAppend);
		g.DrawImage(pMapImgs->at("Bullet").get(), sImgBullet);
	}

	//For every bullet the player fired
	for (auto &vecSharedBullet : vecSharedBullets)
	{
		vecSharedBullet.get()->Render_Entity(hWnd, pMapImgs, wpWinProp);
	}
}

void Player::Update_Entity(HWND hWnd, POINT ptCursor)
{
	RECT rcClientArea;
	GetClientRect(hWnd, &rcClientArea);

	if (false)
	{
		//Get rotation from mouse position
		//Check if position is positive or negative
		point_t Dx_Dy = { fPos.fx - ptCursor.x, fPos.fy - ptCursor.y };
		fRotation = atan(Dx_Dy.fy / Dx_Dy.fx);
	}

	//update velocity based on rotation, if thrusters are active
	if (bIsThrusters)
	{
		//Thrust = relative maximum distance/ Time taken / FPS
		float fThrustBurn = 1.0f / VELOCITY_PERIOD / FRAME_RATE;
		fVec.fy += sin(fRotation - PI / 2) * fThrustBurn;
		fVec.fx += cos(fRotation - PI / 2) * fThrustBurn;
	}

	fPos += fVec;
	//Wrap_Entity(rcClientArea);
	Wrap_Entity(); //Constant resolution

	if (vecSharedBullets.size()) {
		int y = 123;
	}
	vector<int> vecExpiredBullet;
	int index = 0;

	//Checking to see if any bullets hit a player
	for (auto it = vecSharedBullets.begin(); it != vecSharedBullets.end(); it++)
	{
		it->get()->Update_Entity(hWnd, ptCursor);
		if (it->get()->Has_Bullet_Expired())
		{
			vecExpiredBullet.push_back((*it)->ID);
		}
	}

	vector<bullet_t> out;
	
	out.reserve(vecSharedBullets.size() - vecExpiredBullet.size());
	vecExpiredBullet.push_back(-1);  // ID can never be so acts as a no match
	vector<int>::iterator itExpired = vecExpiredBullet.begin();
	std::copy_if(vecSharedBullets.cbegin(), vecSharedBullets.cend(), back_inserter(out), 
		[&itExpired](const bullet_t& b) {
			bool bMatch = b->ID == *itExpired;
			if (bMatch) {
				itExpired++;
			}
			return !bMatch; 
		});
	vecSharedBullets = out;

	this->pQt->update(this);

}

void Player::Input_Entity(vector<eKeyboard> vecKey_press)
{
	//What to do on key press
	for (auto eKey_press = vecKey_press.begin(); eKey_press != vecKey_press.end(); eKey_press++)
	{
		switch (*eKey_press)
		{
		case eKeyboard::Up:
			bIsThrusters = true;
			break;
		case eKeyboard::Down:
			bIsThrusters = false;
			fVec = { 0, 0 };
			break;
		case eKeyboard::Left:
		{
			if (fRotation <= 0)
			{
				fRotation = 2 * (3.14159265f);
			}
			fRotation = fRotation - 0.1f;
			break;
		}
		case eKeyboard::Right:
		{
			if (fRotation >= 2 * (3.14159265f))
			{
				fRotation = 0;
			}
			fRotation = fRotation + 0.1f;
			break;
		}
		case eKeyboard::Space:
			//vecBullets.push_back(Bullet(fPos, fVec, fRotation, rcBulletRatio, enBullet, ID, iTotalBullets));
			//bullet_t pBullet = Bullet_Factory::Create(pQ)
			//vecSharedBullets.push_back(std::make_shared<Bullet>(fPos, fVec, fRotation, rcBulletRatio, enEntity_Type::enBullet, ID, iTotalBullets));
			iTotalBullets++;
			break;
		case eKeyboard::Left_Click:
			bIsThrusters = bIsThrusters ? false : true;
			break;
		case eKeyboard::Right_Click:
			//vecBullets.push_back(Bullet(fPos, fVec, fRotation, rcBulletRatio, enBullet, ID, iTotalBullets));
			//vecSharedBullets.push_back(std::make_shared<IBullet>(fPos, fVec, fRotation, rcBulletRatio, enEntity_Type::enBullet, ID, iTotalBullets));
			iTotalBullets++;
			break;
		default:
			break;
		}

	}

	
}

void Player::Create_Bullet(qt* pQt, int ID_Bullet)
{
	bullet_t pBullet = Bullet_Factory::Create(pQt, fPos, fVec, fRotation, rcBulletRatio, enEntity_Type::enBullet, ID, ID_Bullet);
	vecSharedBullets.push_back(pBullet);
}

bool Player::Player_taken_damage()
{
	iHitPoints--;
	if (iHitPoints <= 0)
	{
		bIsAlive = false;
	}

	return bIsAlive;
}

bool Player::Return_State()
{
	return bIsAlive;
}

std::vector<std::shared_ptr<IBullet>> Player::VecSharedBullet_Access()
{
	return vecSharedBullets;
}

void Player::Player_Score_Increment()
{
	iScore++;
}

std::vector<std::shared_ptr<IBullet>> Player::Return_Shared_Bullets()
{
	return vecSharedBullets;
}

void Player::UpdateHitBox(float fDisMin)
{
	vecPoints_Of_Contact.clear();

	rcHitBox = { 0 / fDisMin
				,0 / fDisMin
				,25 / fDisMin
				,25 / fDisMin };

	float half_x = rcHitBox.right / 2;
	float half_y = rcHitBox.bottom / 2;

	vecPoints_Of_Contact.push_back({ 0.5f * rcHitBox.right, 00.00f });
	vecPoints_Of_Contact.push_back({ rcHitBox.right / 9.00f, 2 * rcHitBox.bottom / 9.00f });
	vecPoints_Of_Contact.push_back({ 23.00f * rcHitBox.right / 27.00f, 2 * rcHitBox.bottom / 9.00f });
	vecPoints_Of_Contact.push_back({ rcHitBox.right / 9.00f, 22.00f * rcHitBox.bottom / 27.00f });
	vecPoints_Of_Contact.push_back({ 23.00f * rcHitBox.right / 27.00f, 22.00f * rcHitBox.bottom / 27.00f });
	for (auto& p : vecPoints_Of_Contact) {
		p.fx -= half_x;
		p.fy -= half_y;
	}
	fRadius = (float)sqrt(pow((rcHitBox.right), 2) + pow(rcHitBox.bottom, 2));
}

IPlayer::IPlayer(point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_)
	: Entity(enType_, fPos_, fVec_, fRotation_, ID_, rcHitbox_)
{
}

std::shared_ptr<IPlayer> Player_Factory::Create(qt* pQt, point_t fPos_, point_t fVec_, float fRotation_, RectFloat rcHitbox_, int ID_, bool bIsThrusters_, enEntity_Type enType_, RectFloat rcBulletRatio_)
{
	shared_ptr<IPlayer> pPlayer = shared_ptr<IPlayer>(new Player( fPos_,  fVec_,  fRotation_,  rcHitbox_,  ID_,  bIsThrusters_,  enType_,  rcBulletRatio_, pQt));
	pQt->add(pPlayer.get());
	return pPlayer;
}
