#include "IScreen.h"
#include <string>
#include <vector>
#include <stdlib.h>
#include <winsock.h>
#include <memory>
#include "Packet.h"
#include "UDP_Interface.h"
#include "UDP_GameState.h"

using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;
using namespace Gdiplus;
using namespace std;

class Host : public IScreen
{
public:
	Host(Client_ScreenINFO_t&& pSomething_);
	~Host();

	void Render(HWND hWnd, Window_Properties wpWinProp) override;
	void Update(HWND hWnd, POINT ptCursor) override;
	void Input(std::vector<eKeyboard> vecKey_Press) override;

	//Num_of_connections: Number of connections connected to the server
	size_t Num_of_connections;
	//enLastPress: Last key press done by the user
	eKeyboard enLastPress;
	//bUpdate_requried:Checks to see if an Update is to happen so it can update the screen
	bool bUpdate_required;
	//params:params containning basic parameters, including the function for it to quit, and info required for the host screen
	Client_ScreenINFO_t params;
	//HostAddr:Socket Address of the server
	const sockaddr* HostAddr;

	//pServerConn: The connection to the server
	unique_ptr<API::Objects::UDPInterface> pServerConn;

	//Host_Info_ID: The ID of the master client used in the server
	size_t Host_Info_ID;

};

static int auto_registration() {
	ScreenController::Record("Host", [](Client_ScreenINFO_t&& pSomething) -> screen_t {
		return std::make_unique<Host>(move(pSomething));
		});
	return 0;
}

static int g_Register = auto_registration();

/*----------------------------------------------------------------------
 Purpose:  Handler to receive data from the server.
 Params:	pData			- Pointer to the packet sent.
			len				- Number of bytes for pData
			pWinsockAddr	- i think pointer to winsock addr sturcture (isn't this just the host?)
			len_of_addr     - length in bytes of pWinsock (still dont think this is nessary)
			pContext		- Pointer to the Host class.
Returns:    None
----------------------------------------------------------------------*/

void on_rec_client(void* pData, size_t len, const void* pWinsockAddr, int len_of_addr, void* pContext)
{
	//pHost_Client: Communicates between the server and the screen
	Host* pHost_Client = (Host*)pContext;

	const sockaddr* pAddr = (const sockaddr*)pWinsockAddr;

	pHost_Client->HostAddr = pAddr;

	//Assign variables
	const Packet* pPacket = (const Packet*)pData;
	const ClientConnectedResponse* pResponse = nullptr;
	const GameStartResponse* pGameStart = nullptr;
	// const ClientStartPacket* pClientStart;
	const ClientEndPacket* pClientEnd = nullptr;



	if (len_of_addr < sizeof(Packet))
	{
		//LoggerFactory::get()->Log("Foreign data being sent");

	}

	switch (pPacket->id)
	{
	case int(Packet_ID::Client_Connect): // client connected
		break;
	case int(Packet_ID::Start_Server): // server start
		break;
	case int(Packet_ID::End_Server): // server end
		break;
	case int(Packet_ID::Connection_Response_Client): //client connected response
		pResponse = (const ClientConnectedResponse*)pPacket;
		pHost_Client->params->ID = pResponse->iPlayerID;
		break;
	case int(Packet_ID::Game_State): // Client_ScreenINFO
		break;
	case int(Packet_ID::Start_Client): // client start
		pHost_Client->params->iNum_Connections++;
		pHost_Client->bUpdate_required = true;
		break;
	case int(Packet_ID::End_Client): // client end
		pClientEnd = (const ClientEndPacket*)pPacket;
		//pHost_Client->params->common.trigger_quit();

		break;
	case int(Packet_ID::Game_Start): // game start response
	{
		pGameStart = (const GameStartResponse*)pPacket;

		//Creates player info
		vector<PlayerStartingInfo> lstPlayerInfo;
		//memcpy(&*lstPlayerInfo.begin(), pGameStart->arrPlayerInfo, pGameStart->size);
		lstPlayerInfo.reserve(pGameStart->count);
		for (size_t i = 0; i < pGameStart->count; i++)
		{
			lstPlayerInfo.push_back(pGameStart->arrPlayerInfo[i]);
		}

		//const CommonStuff Common(pHost_Client->params.trigger_quit, pHost_Client->params.last_host, pHost_Client->params.port, pHost_Client->params->type);

		/*auto* pGI = &pHost_Client->params->game_info;
		pGI->Current_Index = pHost_Client->Host_Info_ID;
		pGI->PlayerIndexs = lstPlayerInfo;
		pGI->pServer_Con = move(pHost_Client->pServerConn);
		*/
		//ScreenController::set("game", std::move(pHost_Client->params));

		break;
	}
	case int(Packet_ID::Update_Death):

		break;
	default:
		break;
	}
}

Host::Host(Client_ScreenINFO_t&& pSomething_)
	: Num_of_connections(0)
	, enLastPress(eKeyboard::None)
	, params(move(pSomething_))
{
	int port = 10000;
	if (params->iPort)
	{
		port = params->iPort;
	}

	//Create a Client
	//Also sends a packet to the server saying a client is trying to connect
	params->pServerConn = API::Factories::UDPFactory::create_client(params->last_host, port, on_rec_client, this, "B");
}

Host::~Host()
{
}

void Host::Render(HWND hWnd, Window_Properties wpWinProp)
{
	//Gets the screen area
	RECT rc;
	GetClientRect(hWnd, &rc);

	//Renders the title
	RECT rectTitle = { 30, 30, rc.right, rc.bottom };
	RECT rectSubTitle = { 30, 120, rc.right, rc.bottom };
	HFONT font = CreateFontA(80, 0, 0, 0, 80, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	HFONT hFontOld = (HFONT)SelectObject(wpWinProp.hdcMem, font);
	DrawTextA(wpWinProp.hdcMem, "Hosting Game", -1, &rectTitle, DT_SINGLELINE | DT_NOCLIP);
	DrawTextA(wpWinProp.hdcMem, ("Players Connected:" + params->iNum_Connections), -1, &rectSubTitle, DT_SINGLELINE | DT_NOCLIP);

	SelectObject(wpWinProp.hdcMem, hFontOld);
	DeleteObject(font);
}

void Host::Update(HWND hWnd, POINT ptCursor)
{
}

void Host::Input(std::vector<eKeyboard> vecKey_Press)
{
	for (auto Key_Press = vecKey_Press.begin(); Key_Press != vecKey_Press.end(); Key_Press++)
	{
		if (*Key_Press == eKeyboard::Enter && Num_of_connections > 0)
		{
			packet_pointer_t pGameStart = ServerStartPacket::create();
			params->pServerConn->Send(pGameStart.get(), pGameStart->size, nullptr);
		}
	}
}
