#include "IScreen.h"
#include <mutex>
#ifdef __MINGW32__
#include "mingw_headers/mingw.mutex.h"
#endif
#include <map>
#include <string>
#include <vector>
#include "IStars.h"
using namespace std;
using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;
using CODE::SCREENS::Objects::ScreenController;

static IStars* g_pStar = new IStars();

static map<string, CODE::SCREENS::Objects::screen_callback_t>* g_pCallbacks = nullptr;
static screen_t g_current_screen;
static vector<screen_t> g_deleted_screens;
static std::mutex g_current_screen_mutex;


IScreen* ScreenController::get()
{
	return g_current_screen.get();
}

void CODE::SCREENS::Objects::ScreenController::Render(HWND hWnd, Window_Properties wpWinProp)
{
	std::lock_guard<std::mutex>Guard(g_current_screen_mutex);
	g_pStar->Render(wpWinProp, hWnd);
	g_current_screen->Render(hWnd, wpWinProp);
}

void CODE::SCREENS::Objects::ScreenController::Update(HWND hWnd, POINT ptCursor)
{
	std::lock_guard<std::mutex>Block(g_current_screen_mutex);
	try {
		g_pStar->Update(hWnd);
		g_current_screen->Update(hWnd, ptCursor);
		g_deleted_screens.clear();
	}
	catch (exception& ex) {
		OutputDebugStringA("** UPDATE FUNCTION CRASHED " __FUNCTION__ "\n");
		OutputDebugStringA(ex.what());
	}

}

void CODE::SCREENS::Objects::ScreenController::Input(vector<eKeyboard> vecKeyPress)
{
	g_current_screen->Input(vecKeyPress);
}

void CODE::SCREENS::Objects::ScreenController::set_screen(const char* szName, Client_ScreenINFO_t sScreen)
{
	if (!g_pCallbacks)
	{
		throw new runtime_error("Failed to intiallise g_pCallbacks");
	}
	auto it = g_pCallbacks->find(szName);
	if (it == g_pCallbacks->end())
	{
		throw new runtime_error(string("Unknown Screen requested"));
	}

	std::lock_guard<std::mutex>Block(g_current_screen_mutex);
	g_deleted_screens.emplace_back(move(g_current_screen));

	g_current_screen = it->second(move(sScreen));
}

void CODE::SCREENS::Objects::ScreenController::Record(const char* szName, screen_callback_t callback)
{
	// Singleton pointer is nullptr at start
	if (!g_pCallbacks)
	{
		g_pCallbacks = new map<string, screen_callback_t>();
	}

	g_pCallbacks->insert(pair<string, screen_callback_t>(string(szName), callback));
}
