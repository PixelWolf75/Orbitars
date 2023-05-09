#pragma once
#include <memory>
#include <string>
#include "GameState.h"
#include <functional>
#include <Windows.h>
#include "Universal_Structures.h"
#include "UDP_GameState.h"


namespace CODE {

	namespace SCREENS {

		namespace ENUMS {

			enum eScreen_Type {
				Splash = 0,
				Main,
				Options,
				game
			};
		}

		namespace Objects {

			class IScreen {
			public:
				virtual void Render(HWND hWnd, Window_Properties wpWinProp) = 0;
				virtual void Update(HWND hWnd, POINT ptCursor) = 0;
				virtual void Input(std::vector<eKeyboard>) = 0;
			};

			
			typedef std::unique_ptr<IScreen> screen_t;
			typedef std::function<screen_t(Client_ScreenINFO_t sGame)> screen_callback_t;


			class ScreenController
			{
			public:

				//gets the current screen
				static IScreen* get();

				//Renders the screen
				static void Render(HWND hWnd, Window_Properties wpWinProp);

				//Updates the screen a certain amount of times per frame
				static void Update(HWND hWnd, POINT ptCursor);

				static void Input(std::vector<eKeyboard> vecKeyPress);

				//Changes which screen we're on
				static void set_screen(const char* szName, Client_ScreenINFO_t sScreen);

				static void Record(const char* szName, screen_callback_t);
			};
		}

	}
}
