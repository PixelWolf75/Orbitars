#pragma once
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <Windows.h>
#include <winsock.h>
#include <string>
#include <memory>
#include "UDP_Interface.h"
#include "Entity.h"
#include "Player.h"
#include "qt.h"
#include "Packet.h"


/**
* Client to Server message.
* Contains information of an event
*  e.g.: Bullet fired
*        PLayer action
*		 astoriod hit
*/
struct ClientMessageEvent {
	//int iPort;
	//char last_host[100];
};


// Messages from Server to Client
// e.g.   Player conencted
//        Other player did action.
struct ServerMessageEvent {
	std::vector<IPlayer> vecPlayers;
	std::unique_ptr<API::Objects::UDPInterface> pServer;  // UDP Server
	int iPlayerCount; // number of players
};

//typedef std::unique_ptr<Game_State> game_state_t;

struct Player
{
	const sockaddr pAddr;
	int iClientId;
	Player(const sockaddr pAddr_, int iPlayerCount)
		: pAddr(pAddr_)
		, iClientId(iPlayerCount)
	{

	}
};

//Contents are shared constantly between 
struct Server_to_ClientGameState
{
	//Qt used for the game
	qt* pQt;

	//Entities in the game
	//::vector<entity_t> vecEntities;

};

struct ServerState
{
	std::vector<Player> players;		// List of all players connected  TODO: add timeout
	std::unique_ptr<API::Objects::UDPInterface> pServer;  // UDP Server
	std::function<void()> func_trigger_server_quit;	     // Call this to trigger the main to stop waiting and quit the server
	int iPlayerCount; // number of players

	bool bIsUpdateNeeded;
	typedef std::vector<POINT> snake_nodes_t;
	typedef std::vector<snake_nodes_t> snake_collection_t;

	

	snake_collection_t lstPlane;
	snake_collection_t lstPrePlane;

	std::map<const Player*, snake_nodes_t> mapSnake;
	std::map<const Player*, snake_nodes_t> mapPreSnake;


	std::vector<int> lstDirections;

	network_point ptFruitPos;

	//std::map<const Player*, int> mapDirections; // directions of a sender

	//std::map<const Player*, bool> mapPlayerUpdateCheck; // checks if the player has already sent their update

	//Server_to_ClientGameState pSTC;
	std::shared_ptr<qt> pQt;
	std::vector<entity_t> vecTrueEntities;
	int iTotalEntities; //Total number of entities and IDs used

	ServerState(std::function<void()> func_trigger_server_quit_)
		: func_trigger_server_quit(func_trigger_server_quit_)
		, iPlayerCount(0)
		, ptFruitPos(network_point(2, 0))
		, bIsUpdateNeeded(true)
		, iTotalEntities(0)
	{
		
	}
};


struct ClientState
{
	std::vector<Player> players;							 // List of all players connected  TODO: add timeout
	std::unique_ptr<API::Objects::UDPInterface> pServer;  // UDP Server
	std::function<void()> func_trigger_server_quit;	     // Call this to trigger the main to stop waiting and quit the server
	std::function<void()> func_trigger_on_connection_to_server;
	ClientState(std::function<void()> func_trigger_server_quit_,
		std::function<void()> func_trigger_on_connection_to_server_)
		: func_trigger_server_quit(func_trigger_server_quit_)
		, func_trigger_on_connection_to_server(func_trigger_on_connection_to_server_)

	{

	}
};