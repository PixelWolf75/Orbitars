#include "IScreen.h"
#include<random>
using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;
using namespace Gdiplus;

class Splash : public IScreen
{
public:
	Splash(Client_ScreenINFO_t&& pSomething_);
	~Splash();

	void Render(HWND hWnd, Window_Properties wpWinProp) override;
	void Update(HWND hWnd, POINT ptCursor) override;
	void Input(std::vector<eKeyboard> vecKey_Press) override;

	eKeyboard eLast_KeyPress;
	Client_ScreenINFO_t pSomething;
};

//Called first upon creation
//Creates the splash screen record
static int auto_registration() {
	ScreenController::Record("Splash", [](Client_ScreenINFO_t&& pSomething) -> screen_t {
		return std::make_unique<Splash>(move(pSomething));
		});
	return 0;
}

static int g_Register = auto_registration();
using namespace std;
Splash::Splash(Client_ScreenINFO_t&& pSomething_)
	: pSomething(move(pSomething_))
	, eLast_KeyPress(eKeyboard::None)
{
	/*class foobar_t {
		foobar_t(string abs) {}
	};
	map<string, int> mapEx1;
	map<string, unique_ptr<foobar_t>> mapThing;
	mapEx1["key"] = 123;

	mapEx1["key"] = int(123);
	mapEx1.insert(pair<string, int>("Key", 123));
	mapThing.insert(pair<string, unique_ptr<foobar_t>>("Key", make_unique<foobar_t>("blah")));*/


	//Load image files of ship
	//pSomething->mapImgFiles.insert(pair<string, unique_ptr<Image>>("Player", Image::FromFile(L"player5.png")));
	auto img_path = L"C:\\Users\\eric\\source\\repos\\Orbitars with Catch\\Orbitars with Catch\\player5.png";
	auto pImg = Image::FromFile(img_path);
	pSomething->mapImgFiles.insert(Client_ScreenINFO::map_o_images_t::value_type("Player", Image::FromFile(L"player5.png")));
	pSomething->mapImgFiles.insert(Client_ScreenINFO::map_o_images_t::value_type("Player_Thruster", Image::FromFile(L"player5 thruster.png")));
	pSomething->mapImgFiles.insert(Client_ScreenINFO::map_o_images_t::value_type("Bullet", Image::FromFile(L"Bullet.png")));

	//pSomething->mapImgFiles["Player_Thruster"] = std::make_unique<Image>(Image::FromFile(L"player5 thruster.png"));
	//pSomething->mapImgFiles["Bullet"] = std::make_unique<Image>(Image::FromFile(L"Bullet.png"));
}

Splash::~Splash()
{
}

void Splash::Render(HWND hWnd, Window_Properties wpWinProp)
{
	//Render: renders title screen

	SetTextColor(wpWinProp.hdcMem, RGB(255, 255, 255));
	SetBkMode(wpWinProp.hdcMem, TRANSPARENT);

	//Gets the screen area
	RECT rc;
	GetClientRect(hWnd, &rc);

	//Renders the title
	RECT rectTitle = { 30, 30, rc.right, rc.bottom };
	HFONT font = CreateFontA(80, 0, 0, 0, 80, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	HFONT hFontOld = (HFONT)SelectObject(wpWinProp.hdcMem, font);
	DrawTextA(wpWinProp.hdcMem, "Orbitars Version 2.0", -1, &rectTitle, DT_SINGLELINE | DT_NOCLIP);
	SelectObject(wpWinProp.hdcMem, hFontOld);
	DeleteObject(font);
}

void Splash::Update(HWND hWnd, POINT ptCursor)
{
}

void Splash::Input(vector<eKeyboard> vecKey_Press)
{
	for (auto eKey_press = vecKey_Press.begin(); eKey_press != vecKey_Press.end(); eKey_press++)
	{
		switch (*eKey_press)
			{
			case eKeyboard::Left_Click:
			case eKeyboard::Space:
				ScreenController::set_screen("Main", move(pSomething));
				break;
			default:
				break;
			}
	}
	
}