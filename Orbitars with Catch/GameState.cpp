#include "GameState.h"

Client_ScreenINFO::Client_ScreenINFO(std::string last_host_, std::string strFilePath_, int iPort_, int iType_, map_o_images_t pMapImgFiles_, RectFloat rcRatioPlayer, RectFloat rcRatioBullet)
	: iPort(iPort_)
	, last_host(last_host_)
	, type(iType_)
	, iKey_Type(0)
	//, mapImgFiles(std::move(pMapImgFiles_))
	, rcPlayerRatio(rcRatioPlayer)
	, rcBulletRatio(rcRatioBullet)
	, strFilePath(strFilePath_)
	, iNum_Connections(0)
{


}

Client_ScreenINFO_GameScreen::Client_ScreenINFO_GameScreen(Client_ScreenINFO_t&& pCommon_, RectFloat rcPlayerRatio_, RectFloat rcBulletRatio_)
	: pCommon(std::move(pCommon_))
	, rcPlayerRatio(rcPlayerRatio_)
	, rcBulletRatio(rcBulletRatio_)
{
	pCommon->iKey_Type = 1;
}

