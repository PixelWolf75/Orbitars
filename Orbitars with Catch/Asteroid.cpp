#include "Asteroid.h"
#include <algorithm>
#include <gdiplus.h>
#include <math.h>

using namespace Gdiplus;
using namespace std;
//bool is_passing_line(Gdiplus::PointF ptBullet, Gdiplus::PointF ptFuture, Gdiplus::PointF p1, Gdiplus::PointF p2);

class Asteroid : public IAsteroid
{
public:
	Asteroid(point_t fPos, point_t fVec_, float fRotation_, int  iNum_of_sides_, float spin_speed, enEntity_Type enType_, int ID_, qt* pQt_, int iSize_ = 500);
	~Asteroid();

	int color;
	float spin_speed; // rotational momentum 

	int iNum_of_sides;
	typedef Gdiplus::PointF ast_point_t;
	struct shape_pos {
		float angle;
		float distance;
		ast_point_t calc(float delta) const;
	};
	std::vector<shape_pos> vecAngles;
	std::vector<Gdiplus::PointF> vecPos;

	int iSize;
	qt* pQt;
	int iNumOfResets;

	void Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp) override;
	void Update_Entity(HWND hWnd, POINT ptCursor) override;
	void Input_Entity(std::vector<eKeyboard>) override;

	void Update_Angles();
	shared_ptr<IAsteroid> Crumble(point_t fVecA, int iAsteroidCount) override;
	bool is_inside(point_t ptBullet);
	void Reset(HWND hWnd) override;

	void bhm_line(int x1, int y1, int x2, int y2, int c, Window_Properties wpWinProp);

	bool Advance_Entity_Collision(Entity* pColliding_entity) override;
	int Return_Number_of_side() override;
	bool Return_bHasExpired() override;
	void RandomLocation() override;

};

Asteroid::Asteroid(point_t fPos_, point_t fVec_, float fRotation_, int iNum_of_sides_, float spin_speed_,
                   enEntity_Type enType_, int ID_, qt* pQt_, int iSize_)
		: IAsteroid(fPos_, fVec_, fRotation_, iNum_of_sides_, spin_speed_, enType_, ID_, iSize_),
		 iNum_of_sides(iNum_of_sides_), spin_speed(spin_speed_),
		 iSize(iSize_), color(0) 
		, pQt(pQt_)
		, iNumOfResets(0)
{
	//Update_Angles();

	/*this->fPos.fx = 0.7f;
	this->fPos.fy = 0.5f;
	this->iNum_of_sides = 3;
	this->fRotation = 0.01f;
	this->spin_speed = 0.01f;*/
	Update_Angles();
	
}

Asteroid::~Asteroid()
{
	pQt->remove(this);
}

//Draws a line between 2 points, in a certain colour
void Asteroid::bhm_line(int x1, int y1, int x2, int y2, int c, Window_Properties wpWinProp)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; //displacement x
	dy = y2 - y1; //displacement y
	dx1 = int(fabs(dx)); //Absolute value of x
	dy1 = int(fabs(dy)); //Absolute value of y
	px = 2 * dy1 - dx1; //Point x
	py = 2 * dx1 - dy1; //Point y
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; 
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		//putpixel(x, y, c);
		//SetPixel(hdc, x, y, RGB(c, c, c));
		if (y >= 0 && y < wpWinProp.iHeight && x >= 0 && x < wpWinProp.iWidth)
		{
			((COLORREF*)wpWinProp.lpBitMapData)[x + (y * wpWinProp.iWidth)] = RGB(c, c, c);
		}
		
		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
			{
				px = px + 2 * dy1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}
			//putpixel(x, y, c);
			//SetPixel(hdc, x, y, RGB(c, c, c));
			if (y >= 0 && y < wpWinProp.iHeight && x >= 0 && x < wpWinProp.iWidth)
			{
				((COLORREF*)wpWinProp.lpBitMapData)[x + (y * wpWinProp.iWidth)] = RGB(c, c, c);
			}
		}
		int m = 0;
	}
	else
	{
		if (dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		//putpixel(x, y, c);
		//SetPixel(hdc, x, y, RGB(c, c, c));
		if (y >= 0 && y < wpWinProp.iHeight && x >= 0 && x < wpWinProp.iWidth)
		{
			((COLORREF*)wpWinProp.lpBitMapData)[x + (y * wpWinProp.iWidth)] = RGB(c, c, c);
		}
		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			//putpixel(x, y, c);
			//SetPixel(hdc, x, y, RGB(c, c, c));
			if (y >= 0 && y < wpWinProp.iHeight && x >= 0 && x < wpWinProp.iWidth)
			{
				((COLORREF*)wpWinProp.lpBitMapData)[x + (y * wpWinProp.iWidth)] = RGB(c, c, c);
			}
		}
	}
}

void Asteroid::Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t * pMapImgs, Window_Properties wpWinProp) {
	RECT rc;
	GetClientRect(hWnd, &rc);
	//pbRange rcAsteroidArea;

	float fDisMin = (float)min(rc.right - rc.left, rc.bottom - rc.top);
	float fDisMax = (float)max(rc.right - rc.left, rc.bottom - rc.top);
	float fDifference = fDisMax - fDisMin;
	point_t ptPixel;
	if (fDisMax == rc.right - rc.left)
	{
		ptPixel = {(fPos.fx * fDisMin) + (fDifference / 2), (fPos.fy * fDisMin)};
	}
	else
	{
		ptPixel = {(fPos.fx * fDisMin), (fPos.fy * fDisMin) + (fDifference / 2)};
	}

	/*	rcAsteroidArea = { LONG(fPos.fx) - rcHitBox.left
		, LONG(fPos.fy) - rcHitBox.top
		, LONG(fPos.fx) + rcHitBox.right
		, LONG(fPos.fy) + rcHitBox.bottom };
		*/
	Gdiplus::Graphics g(wpWinProp.hdcMem);
	g.ResetTransform();
	//RectF sImgRectAsteroid(-(rcHitBox.right / 2), -(rcHitBox.bottom / 2), rcHitBox.right, rcHitBox.bottom);
	Pen pen(Color(255, 255, 255, 255));
	if (vecPos.size())
	{


		std::vector<PointF> vecPixelPos;

		for (auto it = vecAngles.begin(); it != vecAngles.end(); it++)
		{
			ast_point_t astPosAngle = it->calc(fRotation);
			if (fDisMax == rc.right - rc.left)
			{
				vecPixelPos.push_back(PointF(((astPosAngle.X + fPos.fx) * fDisMin) + (fDifference / 2), ((astPosAngle.Y + fPos.fy) * fDisMin)));
			}
			else
			{
				vecPixelPos.push_back(PointF(((astPosAngle.X + fPos.fx) * fDisMin), ((astPosAngle.Y + fPos.fy) * fDisMin) + (fDifference / 2)));
			}
			//vecPixelPos.push_back(PointF((astPosAngle.X + fPos.fx) * fDisMin, (astPosAngle.Y + fPos.fy) * fDisMin));
		}

		/*
		for (auto it = vecPos.begin(); it != vecPos.end(); it++)
		{
			vecPixelPos.push_back(PointF((it->X + fPos.fx) * fDisMin, (it->Y + fPos.fy) * fDisMin));
		}
		*/
		//Contigious so it references the first element then reads the next
		PointF *point = &vecPixelPos.at(0);
		std::vector<PointF> vecHitBox;

		vecHitBox.push_back({REAL(rcHitBox.left), REAL(rcHitBox.top)});
		vecHitBox.push_back({REAL(rcHitBox.right), REAL(rcHitBox.top)});
		vecHitBox.push_back({REAL(rcHitBox.right), REAL(rcHitBox.bottom)});
		vecHitBox.push_back({REAL(rcHitBox.left), REAL(rcHitBox.bottom)});
		vecHitBox.push_back({REAL(rcHitBox.left), REAL(rcHitBox.top)});


		POINT ptScreenPos;
		GetCursorPos(&ptScreenPos);

		ScreenToClient(hWnd, &ptScreenPos);
		Gdiplus::PointF ptCursorFpos = {float(ptScreenPos.x), float(ptScreenPos.y)};
		Gdiplus::PointF ptFuture = {ptCursorFpos.X, ptCursorFpos.Y + float(10000)}; // The bullet at a random position
		point_t cursor_ast = {ptCursorFpos.X - this->fPos.fx, ptCursorFpos.Y - this->fPos.fy};
		point_t cursor_ast2 = {ptFuture.X - this->fPos.fx, ptFuture.Y - this->fPos.fy};

		Pen greeny(Color(255, 0, 255, 0));

		//g.DrawLine(&greeny, ptCursorFpos, ptFuture);

		PointF *Point_t = &vecHitBox.at(0);
		//Translates the asteroid to a positon at ptPixel
		//g.TranslateTransform(ptPixel.fx, ptPixel.fy, Gdiplus::MatrixOrderAppend);
		//Rotates the asteroid
		//g.RotateTransform(REAL(fRotation / PI * 180));
		int w = int((fRadius) * fDisMin);
		int h = int((fRadius) * fDisMin);

		if (false)
		{
			g.DrawLines(&pen, Point_t, (int) vecHitBox.size());
		}
		else
		{
			//g.DrawEllipse(&pen, -w / 2 * 2, -h / 2 * 2, w * 2, h * 2);
		}

		auto it = vecPixelPos.begin();
		for (auto it2 = vecPixelPos.begin() + 1; it != vecPixelPos.end(); it2++, it++)
		{
			if (it2 == vecPixelPos.end())
			{
				it2 = vecPixelPos.begin();
			}
			bhm_line(int(it->X), int(it->Y), int(it2->X), int(it2->Y), 255, wpWinProp);
		}

		//g.DrawLines(&pen, Point_t, vecHitBox.size());
		//g.DrawPolygon(&pen, point, iNum_of_sides); 

		if (color)
		{
			Pen redy(Color(255, 255, 0, 0));
			Pen blue(Color(255, 0, 0, 255));
			//g.DrawPolygon(&redy, point, iNum_of_sides);
			for (size_t i = 0; i < vecAngles.size(); i++) //For every pair of points
			{
				//Calculates the position of the points to draw
				PointF p1 = vecAngles.at(i).calc(this->fRotation);
				PointF p2 = vecAngles.at((i + 1) % vecPos.size()).calc(this->fRotation);
				PointF p1_draw = vecAngles.at(i).calc(0);
				PointF p2_draw = vecAngles.at((i + 1) % vecPos.size()).calc(0);

				point_t p1Altered = point_t(p1.X, p1.Y);
				point_t p2Altered = point_t(p2.X, p2.Y);

				Pen *pPen = &blue;
				if (doIntersect(cursor_ast, cursor_ast2, p1Altered, p2Altered))
				{
					pPen = &redy;
				}
				//Draw the alter asteroid
				g.DrawLine(pPen, p1_draw, p2_draw);
			}
		}
		else
		{
			//Draw the asteroid based on the points
			//g.DrawPolygon(&pen, point, iNum_of_sides);
		}


		//g.DrawLines(&pen, point, iNum_of_sides + 1);
		//fRotation = 1.5;

		//Tanslates the entire asteroid
		//g.TranslateTransform(ptPixel.fx, ptPixel.fy, Gdiplus::MatrixOrderAppend);

	}

}

void Asteroid::Update_Entity(HWND hWnd, POINT ptCursor) {
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	InvalidateRect(hWnd, &rcClient, false);


	fRotation += spin_speed;
	// fRotation = 1.5;
	//fPos.fx += fVec.fx;
	//fPos.fy += fVec.fy;
	// Change position
	{
		fPos += fVec;
		//Wrap_Entity(rcClient);
		Wrap_Entity(); //Constant resolution
		this->pQt->update(this);  // always after changing position, update the QT
	}
	/*if (fRotation == 100.00)
	{
		iNum_of_sides--;
		//iSize = 90;
		Update_Angles();
		fRotation = 0;
		spin_speed = 0.05f;
		fVec.fx = 0;
		fVec.fy = 0;
	}*/
	POINT ptScreenPos = ptCursor;
	ScreenToClient(hWnd, &ptScreenPos);
	point_t ptCursorFpos = {float(ptScreenPos.x), float(ptScreenPos.y)};
	bool bIsHitCheck = is_inside(ptCursorFpos);
	if (bIsHitCheck)
	{
		this->color = 1;
		//bIsHitCheck = is_inside(ptCursorFpos);
		//fVec.fx = -1;
		//fVec.fy = -1;
		//spin_speed = 0.05;
	}
	else
	{
		this->color = 0;
	}

}

void Asteroid::Input_Entity(vector<eKeyboard> eKeyPress) {
	
	for (auto it = eKeyPress.begin(); it != eKeyPress.end(); it++)
	{
		switch (*it)
		{
		case eKeyboard::Left_Click:
			iNum_of_sides--;
			if (iNum_of_sides < 3)
			{
				iNum_of_sides = 3;
			}
			iSize -= 30;
			spin_speed += spin_speed;
			Update_Angles();
			break;
		default:
			break;
		}
	}

}



//Checks if the bullet would pass a plane, returns true if it does
/*bool is_passing_line(Gdiplus::PointF ptBullet, Gdiplus::PointF ptFuture, Gdiplus::PointF p1, Gdiplus::PointF p2)
{
	//If the position is outside the range it isn't passing
	if (ptBullet.X < p1.X && ptBullet.X < p2.X)
		return false;
	if (ptBullet.X > p1.X && ptBullet.X > p2.X)
		return false;

	float a1 = atan2f(ptBullet.Y - p1.Y, ptBullet.X - p1.X); // angle between bullet and point 1
	float a2 = atan2f(ptFuture.Y - p1.Y, ptFuture.X - p1.Y); // angle between bullet and point 2
	float a_ref = atan2f(p2.Y - p1.Y, p2.X - p1.X); // the angle between the 2 points taken as a reference between -PI and PI

	float lesser, higher;
	if (a_ref > 0) {
		// check for below
		lesser = a_ref - PI;
		higher = a_ref;
	}
	else {
		//reverse check
		lesser = a_ref;
		higher = a_ref + PI;
	}
	int is_odd_counter = 0;
	if (a1 >= lesser && a1 < higher) {
		// we know a1 is below, passed the plane
		is_odd_counter++;
	}
	else {
		// not below
	}
	if (a2 >= lesser && a2 < higher) {
		// we know a2 is below, passed the plane
		is_odd_counter++;
	}
	else {
		// not below
	}

	if (is_odd_counter & 1) { //If is_odd_counter has an odd value, it is inside
		return true;
	}
	return false;
}*/



// documentation here
//Checks if ptBullet is inside the polygon donoted by a container of points
//Does this byMethod: 
//- gets angles of points between: (the point, bullet), (same point, bullet at infinity), (the point, and a subsequent point)
//- Adjust the angle between the points (taken as a reference) if it is above or below Pi as a range
//
bool Asteroid::is_inside(point_t ptBullet) {
	//Alters Bullet position to be relative to the asteroid position
	ptBullet.fx -= this->fPos.fx;
	ptBullet.fy -= this->fPos.fy;
	point_t ptFuture = {ptBullet.fx - float(1.123),
	                            ptBullet.fy + float(10000)}; // The bullet at a random position
	int counter = 0;  // count of crossing

	for (size_t i = 0; i < vecAngles.size(); i++) //For every pair of points
	{

		//Calculate angle from nth to n + 1
		PointF p1 = vecAngles.at(i).calc(this->fRotation);
		point_t p1Altered = point_t{ p1.X, p1.Y };
		//PointF p2 = vecPos.at(i + 1 >= vecPos.size() ? 0 : i + 1);
		PointF p2 = vecAngles.at((i + 1) % vecPos.size()).calc(this->fRotation);
		point_t p2Altered = point_t{ p2.X, p2.Y };
		//Checks if the bullet would pass the plane/line
		if (doIntersect(ptBullet, ptFuture, p1Altered, p2Altered))
		{
			counter++;
		}

	}
	// 00000  & 1 = 00000
	// 00001  & 1 = 00001
	// 00010  & 1 = 00000
	// 00011  & 1 = 00001
	// 00100  & 1 = 00000
	// 00101  & 1 = 00001
	if (counter & 1)
	{ //If counter is odd, which means number of planes bullet passed is odd
		// inside
		return true;
	}
	return false;

}

void Asteroid::Reset(HWND hWnd) {

	iNumOfResets++;
	iNum_of_sides = 10;
	Update_Angles();
	//float fPosN = fmodf((float(std::rand()) / RAND_MAX), 0.01);
	fRotation = (float(std::rand()) / RAND_MAX * PI);
	/*float fVecX = fPosN * cosf((fRotation - (PI/2)) * (180/PI));
	float fVecX = fPosN * sinf((fRotation - (PI / 2)) * (180 / PI));
	float fPosX = 0.5f;
	float fPosY = 0.0f;*/
	RECT rcClientArea;
	GetClientRect(hWnd, &rcClientArea);

	// Length of 1 side of the square is the smallest dimention
	int iLengthOfWidth = rcClientArea.right - rcClientArea.left;
	int iLengthOfHeight = rcClientArea.bottom - rcClientArea.top;
	float fLenghtOfSquare = (float)min(iLengthOfWidth, iLengthOfHeight);

	float fDistanceToLeft = (iLengthOfWidth) / (fLenghtOfSquare) * 0.5f;
	float fDistanceToTop = (iLengthOfHeight) / (fLenghtOfSquare) * 0.5f;

	// Absolute coordinates of the edges

	float fPercentage = std::rand() / float(RAND_MAX);
	int iZone = std::rand() % 4;
	float fZonePercentageL = (float)(2* fDistanceToLeft * fPercentage) - (fDistanceToLeft - 0.5f);
	float fZonePercentageT = (float)(2 * fDistanceToTop * fPercentage) - (fDistanceToTop - 0.5f);
	point_t fPosZone = {0, 0};

	switch (iZone)
	{
		case 0:
			fPos = {fZonePercentageL, 0.00f - fRadius};
			break;
		case 1:
			fPos = {0.5f + fDistanceToLeft + fRadius, fZonePercentageT};
			break;
		case 2:
			fPos = {fZonePercentageL, 1.00f + fRadius};
			break;
		case 3:
			fPos = {0.5f - fDistanceToLeft - fRadius, fZonePercentageT};
			break;
		default:
			break;
	}

	int iDirectionX = std::rand() % 2;
	int iDirectionY = std::rand() % 2;
	float fVecX = iDirectionX == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) :
	              fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);
	float fVecY = iDirectionY == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) :
	              fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);


	//Asteroid Default = Asteroid(fPosZone, point_t({ fVecX, fVecY }), 0.00f, std::rand() % 8 + 3, 0.00f, enAsteroid, ID);

	iNum_of_sides = std::rand() % 8 + 3;

	fRotation = 0;

	//fPos = { fPosX, fPosY };
	fVec = {fVecX, fVecY};
	spin_speed = 0.1f;
}

bool Asteroid::Advance_Entity_Collision(Entity* pColliding_entity)
{
	for (auto it = pColliding_entity->vecPoints_Of_Contact.begin(); it != pColliding_entity->vecPoints_Of_Contact.end(); it++)
	{
		point_t abs_point = ((*it) + pColliding_entity->fPos);
		//if (abs_point.fy >= 0.6)
		//{
		//	int y = 0;
		//}
		if (is_inside(abs_point))
		{
			return true;
		}
	}

	return false;
}

int Asteroid::Return_Number_of_side()
{
	return iNum_of_sides;
}

bool Asteroid::Return_bHasExpired()
{
	return iNumOfResets > RESET_LIMIT;
}

//Creates the random location of the asteroid outside of the screen
void Asteroid::RandomLocation()
{
	/*
	// Length of 1 side of the square is the smallest dimention
	int iLengthOfWidth = rcClientArea.right - rcClientArea.left;
	int iLengthOfHeight = rcClientArea.bottom - rcClientArea.top;
	float fLenghtOfSquare = (float)min(iLengthOfWidth, iLengthOfHeight);
	
	float fDistanceToLeft = (iLengthOfWidth) / (fLenghtOfSquare) * 0.5f;
	float fDistanceToTop = (iLengthOfHeight) / (fLenghtOfSquare) * 0.5f;
	*/

	float fPercentage = std::rand() / float(RAND_MAX);
	int iZone = std::rand() % 4;
	float fZonePercentage = (1.5f * fPercentage) - 0.25f;
	point_t fPosZone = { 0, 0 };

	switch (iZone)
	{
	case 0:
		fPos = (fRadius > 0.00f) ? point_t{fZonePercentage, 0.00f - fRadius} : point_t{ fZonePercentage, -0.25 };
		break;
	case 1:
		fPos = (fRadius > 0.00f) ? point_t{1.00f + fRadius, fZonePercentage} : point_t{ 1.25, fZonePercentage };
		break;
	case 2:
		fPos = (fRadius > 0.00f) ? point_t{fZonePercentage, 1.00f + fRadius} : point_t{ fZonePercentage, 1.25 };
		break;
	case 3:
		fPos = (fRadius > 0.00f) ? point_t{ 1.00f - fRadius, fZonePercentage } : point_t{ -0.25, fZonePercentage };
		break;
	default:
		break;
	}
	int iDirectionX = std::rand() % 2;
	int iDirectionY = std::rand() % 2;
	float fVecX = iDirectionX == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) : fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);
	float fVecY = iDirectionY == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) : fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);
	fVec = { fVecX, fVecY };

}


void Asteroid::Update_Angles() {
	vecAngles.clear();
	vecPos.clear();
	vecPoints_Of_Contact.clear();

	float fTotal_Angle = 2 * PI; //Total angle
	float fAvg = fTotal_Angle / float(iNum_of_sides); //Average angle
	float fAdjustment = fAvg / 2; // The adjustmet of the average
	float f_Current = 0; // The current angle
	const float frand_to_num = 1.0f / RAND_MAX; //fraction of random
	float fDis_two_min = 0.2f / log10(float(iSize));

	float fDis_Min = sqrtf((float) ((iSize * iSize) + (iSize * iSize))) /
	                 (iSize / (iNum_of_sides * iNum_of_sides)); // to be altered
	float wiggle_room_of_length = fDis_two_min / 4; //How much the distance can be altered


	float r_left = 0, r_top = 0, r_right = 0, r_bottom = 0, r_Distance = 0; //the hit box coords
	for (size_t i = 0; i < size_t(iNum_of_sides); i++)
	{
		//             0..1  convert to  wiggle angle + avg angle
		// angle in radians
		f_Current += (i != size_t(iNum_of_sides - 1)) ? ((rand() * frand_to_num) * fAdjustment) - (0.5f * fAdjustment) +
		                                                fAvg : (fTotal_Angle - f_Current);

		//Calculates based on adjustment and wiggle room minus half the adjustment + the average, or if it the last side use the remaining angle
		float fDis = rand() * frand_to_num * wiggle_room_of_length + fDis_two_min;
		Asteroid::shape_pos pos = {f_Current, fDis};
		vecAngles.push_back(pos);

		//the distance from the centre.
		PointF PosAngle = pos.calc(0);

		//the position given based on the angle and the distance

		r_left = fmin(PosAngle.X, r_left); //r_left = posAngle.X if it is smaller than itself
		r_top = fmin(PosAngle.Y, r_top); //r_top = posAngle.Y if it is smaller than itself
		r_right = fmax(PosAngle.X, r_right); //r_right = posAngle.X if it is bigger than itself
		r_bottom = fmax(PosAngle.Y, r_bottom); //r_bottom = posAngle.Y if it is bigger than itself
		r_Distance = fmax(r_Distance, fDis); //r_Distance = fDis if it is bigger than itself

		vecPos.push_back(PosAngle); //Pushes back the position
		vecPoints_Of_Contact.push_back(point_t{ PosAngle.X, PosAngle.Y });

	}
	//RectF rcHit(r_left, r_top, r_right - r_left, r_bottom - r_top);
	fRadius = r_Distance; //Calculate the radius
	
}

shared_ptr<IAsteroid>  Asteroid::Crumble(point_t fVecA, int iAsteroidCount) {
	iNum_of_sides--;
	//Makes sure the asteroid satys at a minumum size and num of sides
	/*if (iNum_of_sides < 3)
	{
		Reset();
		return make_shared<Asteroid>(point_t{0, 0}, point_t{0, 0}, 0.00f, 0, 0, enEntity_Type::enAsteroid, -1);
	}*/

	if (iSize - 30 > 0)
	{
		iSize -= 30;
	}

	spin_speed += spin_speed; //Increase angular velocity due to kinetic energy from seperation
	Update_Angles(); //Update angles to be random with reduced side
	float LostV = float(iSize / 500); //Lost kinetic energy due to crumble of asteroid
	fVec = fVec / 2;

	//Decrease absolute value of the velocity
	if (fVec.fx >= 0)
	{
		fVec.fx -= LostV;
	}
	else
	{
		fVec.fx += LostV;

	}

	if (fVec.fy >= 0)
	{
		fVec.fy -= LostV;
	}
	else
	{
		fVec.fy += LostV;
	}

	point_t fVecAlternate = {fVecA.fy / 2,
	                         fVecA.fx / 2}; //Perpendicular velocity in one direction is the x and y components swapped
	point_t fVec_Other = fVec + fVecAlternate / -2; //Perpendicular velocity in the other direction  for other asteroid
	fVec += fVecAlternate; //Adjusted velocity for impact with bullet

	//Creates a split off asteroid, at the same position, with the other velocity, at the same rotation rotation in the opposite direction
	// make_shared<Asteroid>(fPos, fVec_Other, fRotation, iNum_of_sides, -(spin_speed), enEntity_Type::enAsteroid, iSize, pQt,  ID + 1);
	 return Asteroid_Factory::Create(this->pQt, fPos, fVec_Other, fRotation, iNum_of_sides, -(spin_speed), enEntity_Type::enAsteroid, iAsteroidCount);
}

Asteroid::ast_point_t Asteroid::shape_pos::calc(float delta_rotation = 0.0f) const {
	PointF PosAngle = {distance * cosf(angle + delta_rotation), distance * sinf(angle + delta_rotation)};
	return ast_point_t(PosAngle);
}

IAsteroid::IAsteroid(point_t fPos_, point_t fVec_, float fRotation_, int  iNum_of_sides_,
				float spin_speed, enEntity_Type enType_, int ID_, int iSize_)
	: Entity(enType_, fPos_, fVec_, fRotation_, ID_)
{
}

std::shared_ptr<IAsteroid> Asteroid_Factory::Create(qt* pQt, point_t fPos, point_t fVec_, float fRotation_, int iNum_of_sides_, float spin_speed, enEntity_Type enType_, int ID_, int iSize_)
{
	
	shared_ptr<IAsteroid> pAsteroid = shared_ptr<IAsteroid>(new Asteroid(fPos, fVec_, fRotation_, iNum_of_sides_, spin_speed, enType_, ID_, pQt, iSize_));
	pQt->add(pAsteroid.get());
	return pAsteroid;
	
}
