#include "IStars.h"
#include <algorithm>
#include "Universal_Structures.h"

using namespace std;

//Prototyped functions
Star default_star();
FPOINT Change_Velocity(FPOINT pPos);


IStars::IStars()
{
	vecStars.reserve(DEFAULT_NUM_OF_STARS);
	//reserves the room of the default number of stars in vecStars
	for (size_t i = 0; i < DEFAULT_NUM_OF_STARS; i++)
	{
		vecStars.push_back(default_star());
		//for each star make and push back a new star
	}
}

IStars::~IStars()
{
}

void IStars::Render(Window_Properties wpWinProp, HWND hWnd)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	//Gets the size of the screen and puts it in the rcClient

	/*HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
	//Creates an off screen device context to do all the drawing to*/

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH hOld = (HBRUSH)SelectObject(wpWinProp.hdcMem, hBrush);
	//Creates a solid black brush and selects it

	// FillRect(hdc, &rcClient, hBrush);
	//Fills the whole screen black
	SelectObject(wpWinProp.hdcMem, hOld);
	DeleteObject(hBrush);
	//Pushs the brush back onto the stack and then deletes the brush

	Draw_Stars(wpWinProp, rcClient);
	//Draws all the stars

	/*BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);
	//Transfers all the drawing we've done onto the main device context
	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	//Deletes the offscreen device context*/
}

static const float g_fMax_Distance = (float)sqrt(double(int(SCREEN_WIDTH) * int(SCREEN_WIDTH) + int(SCREEN_HEIGHT) * int(SCREEN_HEIGHT))) / 2;
void IStars::Update(HWND hWnd)
{
	const float fMax_Distance = g_fMax_Distance;
	RECT rc;
	GetClientRect(hWnd, &rc);
	
	//fMax_Distance = the maximum distance the star can move from the center
	for (auto it = vecStars.begin(); it != vecStars.end(); it++)
	{
		//Checks to see if Intial brightness is greater than the Current brightness if it is then make the curent brightness the intial brightness
		it->iInitialBrightness = min(it->iCurrentBrightness, it->iInitialBrightness + 1);

		//Change velocity
		it->pVelocity = Change_Velocity(it->pPos);

		//Update position base on velocity
		it->pPos.x += it->pVelocity.x;
		it->pPos.y += it->pVelocity.y;

		//Check to see if star if off screen
		FPOINT fAbsoluteDis = { ((rc.right - rc.left) / 2) - it->pPos.x, ((rc.right - rc.left) / 2) - it->pPos.y };
		float fAbsoluteDisHypotenuse = sqrtf(fAbsoluteDis.x * fAbsoluteDis.x + fAbsoluteDis.y * fAbsoluteDis.y);

		//If it is remake the star
		if (fAbsoluteDisHypotenuse > fMax_Distance)
		{
			*it = default_star();
		}
	}

	//Invailidates the whole screen so the screen knows to render again
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	InvalidateRect(hWnd, &rcClient, FALSE);
}

//Returns a star with:
//random position
//random velocity
//intial brightness
//and a current brightness(same as the intial)
//random colour scheme
Star default_star()
{
	FPOINT fPos = { (float)(rand() % SCREEN_WIDTH) ,(float)(rand() % SCREEN_HEIGHT) };
	//fpos = random position within the screen boundaries
	int iBrightness = rand() % (255 - MINIMUM_BRIGHTNESS) + MINIMUM_BRIGHTNESS;
	//iBrightness = random brightness with a minimum brightness
	int iBrightness_Starting = iBrightness;
	int iColour = rand() % 10;
	//iColor = random colour scheme

	return Star(fPos, Change_Velocity(fPos), iBrightness, iBrightness_Starting, iColour);
}

//Returns a velocity float point
//based on the position
FPOINT Change_Velocity(FPOINT pPos)
{
	FPOINT fAbsoluteDis = { (float)fabs(double((SCREEN_WIDTH / 2) - pPos.x)), (float)fabs(double((SCREEN_HEIGHT / 2) - pPos.y)) };
	//fAbsoluteDis = the absolute distance from the center, never more than half the screen height/width
	FPOINT fVelDirection = { pPos.x > (SCREEN_WIDTH / 2) ? 1.0f : -1.0f, pPos.y > (SCREEN_HEIGHT / 2) ? 1.0f : -1.0f };
	//fVelDirection = Checks to see if the position on either side
	FPOINT pVelocity = FPOINT({ fAbsoluteDis.x / FRAME_RATE / PERIOD * fVelDirection.x, fAbsoluteDis.y / FRAME_RATE / PERIOD * fVelDirection.y });
	//fVelocity = speed for the star to move to the edge based on framerate at a set period
	return pVelocity;
}

void IStars::Draw_Stars(Window_Properties wpWinProp, RECT rcClient)
{
	//For every star
	for (auto it = vecStars.begin(); it != vecStars.end(); it++)
	{
		//Set as default brightness
		int iR = it->iCurrentBrightness;
		int iG = it->iCurrentBrightness;
		int iB = it->iCurrentBrightness;

		//Alter star colour based on random colour scheme
		if (it->iColor > 8)
		{
			iB = iB / 2;
			iR = min(iR + 40, 255);
			iG = min(iG + 40, 255);
		}
		else if (it->iColor > 9)
		{
			iR = iG = 128;
			iB = 255;
		}
		else if (it->iColor > 10)
		{
			iG = iB = 0;
		}

		//Sub pixel render at the position
		Sub_pixel(it->pPos, (float)it->iCurrentBrightness, wpWinProp);
	}
}

//Render the Pixel, using the different colour channels to recreate a new pixel
//Makes the rendering smoother and reduces aliasing
//Params: fPos(original float position), fBrightness(The brightness of the pixel), hdc(device context we are writing to)
void IStars::Sub_pixel(FPOINT fPos, float fBrightness, Window_Properties wpWinProp)
{
	POINT iPos = { (LONG)fPos.x, (LONG)fPos.y };
	//iPos = integer position
	POINT fPos_and_half = { (LONG)(fPos.x + 0.5f), (LONG)(fPos.y + 0.5f)};
	//fPos_and_half = altered intger position if the float value can be rounded

	POINT fOther_pos = { (fPos_and_half.x > iPos.x) ? 1 : -1,(fPos_and_half.y > iPos.y) ? 1 : -1 };
	//Create a direction for the pixel to render based on the pixel position

	POINT arrPos[4] = {
	iPos, //Integer position
	{iPos.x + fOther_pos.x, iPos.y}, //Altered x position
	{iPos.x, iPos.y + fOther_pos.y}, //Altered y position
	{iPos.x + fOther_pos.x, iPos.y + fOther_pos.y} // Altered x and y position
	};

	struct RECTF {
		float left, top, right, bottom; //Float version of RECT
	};

	//Draw_sub: Draws the sub pixel
	//parameters:
	//fOriginalPos = the orignal float position,
	//iIntegerPos = the integerPosition and altered value we are subpixeling by
	//fBrightness = the brightness of the pixel we are rendering
	auto Draw_Sub = [=](FPOINT fOriginalPos, POINT iIntegerPos, float fBrightness) -> void {
		RECTF RectR = { 0, 0 ,0.333f, 1.0f }; // the left third of a pixel. The red channel
		RECTF RectG = { 0.333f, 0, 0.666f, 1.0f }; // the middle third of a pixel. The Green channel
		RECTF RectB = { 0.666f, 0, 1.0f, 1.0f };// the right third of a pixel. The blue channel

		//Creates the altered rect to check for overlap
		RECTF RectStar = { fOriginalPos.x - iIntegerPos.x - 0.5f,
			fOriginalPos.y - iIntegerPos.y - 0.5f,
			fOriginalPos.x - iIntegerPos.x - 0.5f + 1,
			fOriginalPos.y - iIntegerPos.y - 0.5f + 1 };

		//GetOverlapArea: gets the overlap area to base the brightness on
		//parameters: Rect1 = the area of the star we are checking for overlap
		//              Rect2 = the area of the colour channel we are overlapping
		auto GetOverlapArea = [](RECTF& Rect1, RECTF& Rect2) -> float {
			//If any overlap would be negative there is no overlap
			float fx_Overlap = float(fmax(0, min(Rect1.right, Rect2.right) - fmax(Rect1.left, Rect2.left)));
			float fy_Overlap = float(fmax(0, min(Rect1.bottom, Rect2.bottom) - fmax(Rect1.top, Rect2.top)));
			return fx_Overlap * fy_Overlap;
		};

		//Creates the rgb values based omn the overlap with the colour channels
		float r = GetOverlapArea(RectStar, RectR) * 3 * fBrightness;
		float g = GetOverlapArea(RectStar, RectG) * 3 * fBrightness;
		float b = GetOverlapArea(RectStar, RectB) * 3 * fBrightness;

		//Sets the pixel at the interger position
		 //SetPixel(hdc, iIntegerPos.x, iIntegerPos.y, RGB(r, g, b));
		if (iIntegerPos.y >= 0 && iIntegerPos.y < wpWinProp.iHeight && iIntegerPos.x >= 0 && iIntegerPos.x < wpWinProp.iWidth)
		{
			((COLORREF*)wpWinProp.lpBitMapData)[iIntegerPos.x + (iIntegerPos.y * wpWinProp.iWidth)] = RGB(r, g, b);
		}
		 
	};

	for (size_t i = 0; i < 4; i++)
	{
		//Draws a sub pixel for each altered value
		Draw_Sub(fPos, arrPos[i], fBrightness);
	}
}
