#include "IScreen.h"
#include <string>
#include <memory>
#include <vector>
#include <stdlib.h>
#include "UDP_Interface.h"

using namespace std;
using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;
using namespace Gdiplus;

class Join : public IScreen
{
public:
	Join(Client_ScreenINFO_t&& pSomething_);
	~Join();

	void Render(HWND hWnd, Window_Properties wpWinProp) override;
	void Update(HWND hWnd, POINT ptCursor) override;
	void Input(std::vector<eKeyboard> vecKey_Press) override;

	Client_ScreenINFO_t params;

	size_t szNum_of_Connections;
	size_t szClient_ID;

	//HostAddr:Socket Address of the server
	const sockaddr* HostAddr;

	bool bIsStart;

	//pServerConn: The connection to the server
	unique_ptr<API::Objects::UDPInterface> pServerConn;

	//Host_Info_ID: The ID of the master client used in the server
	size_t Host_Info_ID;

};

//First called upon creation
static int auto_registration() {
	ScreenController::Record("Join", [](Client_ScreenINFO_t&& pSomething) -> screen_t {
		return std::make_unique<Join>(move(pSomething));
		});
	return 0;
}

static int g_Register = auto_registration(); 

void on_rec(void* pData, size_t len, const void* pWinsockAddr, int len_of_addr, void* pContext)
{
	//pJoin: allows communication between screen and server/client
	Join* pJoin = (Join*)pContext;

	//pAddr: socket address of the one that sent to this callback almost always the server
	const sockaddr* pAddr = (const sockaddr*)pWinsockAddr;

	//Checks to see if this is a packet
	if (len_of_addr < sizeof(Packet))
	{
		//LoggerFactory::get()->Log("Foreign data being sent");

	}
	//Initiallise all variables
	const Packet* pPacket = (const Packet*)pData;
	const ClientConnected* pClient_Connect = nullptr;
	const ServerStartPacket* pStart = nullptr;
	const ClientConnectedResponse* pClient_Response = nullptr;




	switch (pPacket->id)
	{
	case int(Packet_ID::Client_Connect):
		break;
	case int(Packet_ID::Start_Server): // wtf is 1?  should be enum
	{
		/*string strName = "foobar";
		pStart = (const ServerStartPacket*)pPacket;
		pJoin->g_bIsStart = true;
		//packet_pointer_t pResponse = COMMON_STUFF::PACKETS::ClientConnected::create(strName);
		//pJoin->pServerConn->Send(pResponse.get(), sizeof(COMMON_STUFF::PACKETS::ClientConnected) + strName.length(), pAddr);
		packet_pointer_t pStartUp = COMMON_STUFF::PACKETS::ClientStartPacket::create();
		pJoin->pServerConn->Send(pStartUp.get(), sizeof(COMMON_STUFF::PACKETS::ClientStartPacket), pAddr);*/
		break;
	}
	case int(Packet_ID::End_Server):
		break;
	case int(Packet_ID::Connection_Response_Client):
	{
		pClient_Response = (const ClientConnectedResponse*)pPacket;
		pJoin->params->ID = pClient_Response->iPlayerID;
		pJoin->bIsStart = true;
		break;
	}
	case int(Packet_ID::Game_State):
	{
		break;
	}
	case int(Packet_ID::Start_Client):
	{
		pJoin->params->iNum_Connections++;

		break;
	}
	case int(Packet_ID::End_Client): // client end
	{
		//Switch screen end
		ScreenController::set_screen("splash", move(pJoin->params));
		break;
	}
	case int(Packet_ID::Game_Start):
	{
		//pClient_Response = (const ClientConnectedResponse*)pPacket;
		const GameStartResponse* pGame_Start;
		pGame_Start = (const GameStartResponse*)pPacket;
		vector<PlayerStartingInfo> lGameInfo;
		lGameInfo.reserve(pGame_Start->count);
		for (size_t i = 0; i < pGame_Start->count; i++)
		{
			lGameInfo.push_back(pGame_Start->arrPlayerInfo[i]);
		}

		//auto* pGI = &pJoin->params->game_info;
		//pGI->Current_Index = pJoin->Client_Info_PlayerID;
		//pGI->PlayerIndexs = lGameInfo;
		//pGI->pServer_Con = move(pJoin->pServerConn);
		//ScreenController::set("game", std::move(pJoin->params));
		pJoin->params->iNum_Connections = pJoin->szNum_of_Connections;
		ScreenController::set_screen("game", std::move(pJoin->params));

		break;
	}
	default:
		break;
	}

}


Join::Join(Client_ScreenINFO_t&& pSomething_)
	: params(move(pSomething_))
	, szNum_of_Connections(0)
{
	int port = 10000;
	if (params->iPort)
	{
		port = params->iPort;
	}

	//Create a Client
	//Also sends a packet to the server saying a client is trying to connect
	params->pServerConn = API::Factories::UDPFactory::create_client(params->last_host, port, on_rec, this, "B");
}

Join::~Join()
{
}

void Join::Render(HWND hWnd, Window_Properties wpWinProp)
{
	//Gets the screen area
	RECT rc;
	GetClientRect(hWnd, &rc);
	string str = bIsStart ? "Connected" : "Connecting";
	//Renders the title
	RECT rectTitle = { 30, 30, rc.right, rc.bottom };
	RECT rectSubTitle = { 30, 120, rc.right, rc.bottom };
	HFONT font = CreateFontA(80, 0, 0, 0, 80, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	HFONT hFontOld = (HFONT)SelectObject(wpWinProp.hdcMem, font);
	DrawTextA(wpWinProp.hdcMem, "Joining Game", -1, &rectTitle, DT_SINGLELINE | DT_NOCLIP);
	DrawTextA(wpWinProp.hdcMem, str.c_str(), -1, &rectSubTitle, DT_SINGLELINE | DT_NOCLIP);

	SelectObject(wpWinProp.hdcMem, hFontOld);
	DeleteObject(font);
}

void Join::Update(HWND hWnd, POINT ptCursor)
{
}

void Join::Input(std::vector<eKeyboard> vecKey_Press)
{
}
