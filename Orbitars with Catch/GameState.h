#pragma once
#include <memory>
#include <string>
#include <functional>
#include <Unknwn.h>    
#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#include <string>
#include <map>
#include "UDP_Interface.h"
//#include "UDP_GameState.h"

struct RectFloat {
	float left = 0.00f;
	float top = 0.00f;
	float right = 0.00f;
	float bottom = 0.00f;
};

class qt;

//Infomation on the client side used by the screen
struct Client_ScreenINFO {
	int iPort;
	int type; // DOCUMENTATION MF!!!!  WTF IS THIS.  DOCUMENT!!!!!
	std::string last_host;
	typedef std::map<std::string, std::unique_ptr<Gdiplus::Image>> map_o_images_t;
	std::string strFilePath;
	map_o_images_t mapImgFiles;
	int iKey_Type;
	RectFloat rcPlayerRatio;
	RectFloat rcBulletRatio;
	int iNum_Connections;
	int ID;

	std::shared_ptr<API::Objects::UDPInterface> pServerConn;
	
	//The contents are shared constantly between server and client
	// Server_to_ClientGameState pSTC;
	qt* pQuadTree;


	Client_ScreenINFO(std::string last_host_, std::string strFilePath_, int iPort_, int iType_, map_o_images_t pMapImgFiles_ = map_o_images_t(), RectFloat rcRatioPlayer = RectFloat{}, RectFloat rcRatioBullet = RectFloat{});
};



typedef std::unique_ptr<Client_ScreenINFO> Client_ScreenINFO_t;

struct Client_ScreenINFO_GameScreen {
	RectFloat rcPlayerRatio;
	RectFloat rcBulletRatio;
	Client_ScreenINFO_t pCommon;

	Client_ScreenINFO_GameScreen(Client_ScreenINFO_t&& pCommon_,
		RectFloat rcPlayerRatio_,
		RectFloat rcBulletRatio_);
};


