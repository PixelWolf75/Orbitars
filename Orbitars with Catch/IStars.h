#pragma once
#include <Windows.h>
#include <vector>
#include "Universal_Structures.h"


struct FPOINT {
	float x;
	float y;
};

struct Star {
	FPOINT pPos; //position relative to the screen
	FPOINT pVelocity; //how much the position is altered per second
	int iCurrentBrightness; //current brightness of the star
	int iInitialBrightness; //intial brightness of the star
	int iColor; //what sort of colour the star is

	Star(FPOINT pPos_, FPOINT pVelocity_, int iCurrentBrightness_, int iIntialBrightness_, int iColour_)
		: pPos(pPos_)
		, pVelocity(pVelocity_)
		, iCurrentBrightness(iCurrentBrightness_)
		, iInitialBrightness(iIntialBrightness_)
		, iColor(iColour_)
	{

	}
};

class IStars
{
public:
	IStars();
	~IStars();

	void Render(Window_Properties wpWinProp, HWND hWnd);
	void Update(HWND hWnd);

	std::vector<Star> vecStars;
private:
	void Sub_pixel(FPOINT fPos, float fBrightness, Window_Properties wpWinProp);
	void Draw_Stars(Window_Properties wpWinProp, RECT rcClient);
};
