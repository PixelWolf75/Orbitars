#include "IScreen.h"
#include <string>
#include <vector>
#include <stdlib.h>
#include "sql/sqlite3.h"

#define TEXT_WIDTH_DISTANCE 200
#define TEXT_HEIGHT_DISTANCE 150

using namespace std;
using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;

static int Callback(void* pExtra, int argc, char** argv, char** azColName) {
	return 0;
}

class Main : public IScreen
{
public:
	Main(Client_ScreenINFO_t&& pStuff);
	~Main();

	void Render(HWND hWnd, Window_Properties wpWinProp) override;
	void Update(HWND hWnd, POINT ptCursor) override;
	void Input(std::vector<eKeyboard> vecKeyPress) override;

	bool bUpdate;
	Client_ScreenINFO_t pCommon;
	vector<string> vecOptions;

	vector<RECT> vecRects;
	//std::unique_ptr<Client_ScreenINFO_GameScreen> pGameInfo;

	sqlite3* pSQL;

	size_t pos;
	DWORD Tick;

	bool bQuit;

};

static int auto_registration() {
	ScreenController::Record("Main", [](Client_ScreenINFO_t&& pSomething) -> screen_t {
		return std::make_unique<Main>(move(pSomething));
		});
	return 0;
}

static int g_Register = auto_registration();


Main::Main(Client_ScreenINFO_t&& pStuff)
	: pCommon(move(pStuff))
	, vecOptions({ "Game", "Option2", "Quit" })
	, pos(0)
	, bUpdate(true)
	, bQuit(false)
	, Tick(0)
{

	int rc = sqlite3_open(pCommon->strFilePath.c_str(), &pSQL);
	if (rc)
	{
		throw(exception("Database failed to open"));
	}
	else {

	}

	string sql_Table = "CREATE TABLE LOGIN("\
		"ID INT PRIMARY KEY		NOT NULL,"\
		"USERNAME	VARCHAR(50)		NOT NULL,"\
		"PASSWORD	VARCHAR(50)		NOT NULL)";
	char* szErrMesg;
	rc = sqlite3_exec(pSQL, sql_Table.c_str(), Callback, 0, &szErrMesg);
	if (rc != SQLITE_OK)
	{

	}
	else
	{

	}

	sql_Table = "INSERT INTO LOGIN (ID,USERNAME,PASSWORD) "  \
		"VALUES (1, 'ERIC', 'LEON'); ";
	rc = sqlite3_exec(pSQL, sql_Table.c_str(), Callback, 0, &szErrMesg);

	sqlite3_close(pSQL);
}

Main::~Main()
{
}

void Main::Render(HWND hWnd, Window_Properties wpWinProp)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	//InvalidateRect(hWnd, &rc, false);

	SetTextColor(wpWinProp.hdcMem, RGB(255, 255, 255));
	SetBkMode(wpWinProp.hdcMem, TRANSPARENT);
	RECT rect = { LONG(0.005 * float(rc.right - rc.left)), LONG(0.05 * float(rc.bottom - rc.top)), LONG(0.25 * float(rc.right - rc.left)), LONG(0.17 * float(rc.bottom - rc.top)) };
	int rcH = int(0.15 * ((float)rc.bottom - rc.top));
	int rcW = int(0.03 * ((float)rc.right - rc.left));
	HFONT font = CreateFontA(rcH, rcW, 0, 0, 400, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	HFONT hFontOld = (HFONT)SelectObject(wpWinProp.hdcMem, font);
	DrawTextA(wpWinProp.hdcMem, "Menu:", -1, &rect, DT_SINGLELINE | DT_NOCLIP);

	int iLineHeight = int(0.25 * ((float)rc.bottom - rc.top));

	for (size_t i = 0; i < vecOptions.size(); i++)
	{
		rect.top = ((i + 1) * iLineHeight);
		rect.bottom = ((i + 2) * iLineHeight);
		if (i == pos)
		{
			rect.left = LONG(0.01 * (rc.right - rc.left));
			rect.right = LONG(0.35 * (rc.right - rc.left));
			DrawTextA(wpWinProp.hdcMem, string(">" + vecOptions[i]).c_str(), -1, &rect, DT_NOCLIP);
		}
		else {
			rect.left = LONG(0.02 * (rc.right - rc.left));
			DrawTextA(wpWinProp.hdcMem, string(vecOptions[i]).c_str(), -1, &rect, DT_NOCLIP);
		}
		vecRects.push_back(rect);
	}

	SelectObject(wpWinProp.hdcMem, hFontOld);
	DeleteObject(font);


}

void Main::Update(HWND hWnd, POINT ptCursor)
{
	//Check cursor position
	POINT ptScreenCursor = ptCursor;
	RECT rc;
	GetClientRect(hWnd, &rc);
	float fMin = float(min((rc.right - rc.left), (rc.bottom - rc.top)));
	RectFloat rcPlayer = { 0 / fMin,
		0 / fMin ,
		25 / fMin ,
		25 / fMin };
	RectFloat rcBullet = { 0 / fMin,
		0 / fMin ,
		4 / fMin ,
		4 / fMin };
	//pGameInfo = make_unique<Client_ScreenINFO_GameScreen>(std::move(pCommon), rcPlayer, rcBullet);
	pCommon->rcPlayerRatio = rcPlayer;
	pCommon->rcBulletRatio = rcBullet;

	if (ScreenToClient(hWnd, &ptScreenCursor))
	{
		for (size_t i = 0; i < vecRects.size(); i++)
		{
			if (ptScreenCursor.y >= vecRects[i].top && ptScreenCursor.y <= vecRects[i].bottom)
			{
				if (ptScreenCursor.x >= vecRects[i].left && ptScreenCursor.x <= vecRects[i].right)
				{
					pos = i;

					bUpdate = true;
				}
			}
		}
	}

	vecRects.clear();

	if (bQuit)
	{
		DestroyWindow(hWnd);
	}
}

void Main::Input(std::vector<eKeyboard> vecKey_press)
{
	for (auto eKey_press = vecKey_press.begin(); eKey_press != vecKey_press.end(); eKey_press++)
	{
		switch (*eKey_press)
		{
		case eKeyboard::Down:
			if (pos < (vecOptions.size() - 1))
			{
				pos++;
				//bUpdate = true;
			}
			break;
		case eKeyboard::Up:
			if (pos > 0)
			{
				pos--;
				//bUpdate = true;
			}
			break;
		case eKeyboard::Enter:
		case eKeyboard::Left_Click:
			if (vecOptions[pos] == "Game")
			{

				ScreenController::set_screen("Game", move(pCommon));
			}
			else if (vecOptions[pos] == "Option2")
			{
				//GameScrren_Options(pPrevous->pCommon, 12,123,12,312)
			}
			else if (vecOptions[pos] == "Quit")
			{
				bQuit = true;
				//exit(3);
			}
			break;
		default:
			break;
		}
	}
}