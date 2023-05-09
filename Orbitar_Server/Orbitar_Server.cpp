// Orbitar_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>
#include <winsock.h>
#include <string>
#include <mutex>
#include <random>
#include <stdlib.h>
#include "../Orbitars with Catch/qt.h"
#include <time.h>
#include <condition_variable>


#include "../Orbitars with Catch/Entity.h"
#include "../Orbitars with Catch/qt.h"
#include "../Orbitars with Catch/Asteroid.h"
#include "../Orbitars with Catch/Player.h"
#include "../Orbitars with Catch/Bullet.h"
#include "../Orbitars with Catch/Packet.h"
#include "../Orbitars with Catch/UDP_GameState.h"

using namespace std;
using namespace API::Objects;
using namespace API::Factories;

struct fPos {
	float fX;
	float fY;
};

/*
TODO List
---------------
Create Update for server
Make True Entities, that client entities mirror
Copy game code here
Create IDs for each entity here, and sent them to the clients
Update the Truth for each client
*/


vector<shared_ptr<IAsteroid>> Game::Collision_Check(Entity* pEntity, HWND hWnd)
{
	//Casted variables for pEntity, who the entity colliding with
	IPlayer* pBasePlayer = nullptr;
	IBullet* pBaseBullet = nullptr;
	IAsteroid* pBaseAsteroid = nullptr;

	//A circular area around the player to search
	search_radius_t pbRange_Radius = search_radius_t({ pEntity->fPos.fx, pEntity->fPos.fy, pEntity->fRadius });
	search_area_t pbRange_Boundary = search_area_t({ pEntity->fPos.fx, pEntity->fPos.fy, pEntity->rcHitBox.right, pEntity->rcHitBox.bottom });
	//The entities around the player
	vector<node> vecFoundNodes;

	//List of new asteroids to add on
	vector<asteroid_t> new_asteroids;
	vector<int> deleted_asteroid_indexes;

	/*
	Upon Collison:
	Player takes damage
	Player's score increases

	Asteroid Crumbles into two
	If destroyed/rest relays information to player

	Bullet relays information if it has killed opposing player

	Player pointer -> Takes damage/has score increase
	Bullet pointer -> Relays if the player has taken damage
	Asteroid pointer -> Crumbles/relays if it has been destoryed to bullet and its player
	*/


	//If a player collides with an entity
	auto p_Player_Collision = [&](IPlayer* pColliding_entity) -> int {
		if (!pColliding_entity->Player_taken_damage())
		{
			iPlayer_Count--;
			return pColliding_entity->ID;
		}
		return -1;
	};


	//The response to a player colliding with an entity
	auto p_Player_Collision_response = [&](IPlayer* pBase_entity) {
		pBase_entity->Player_Score_Increment();
	};

	//If a Bullet 
	auto p_Bullet_Collision = [&](IBullet* pColliding_entity)-> int {
		return pColliding_entity->Return_Player_ID();
	};

	auto p_Asteroid_Collision = [&](IAsteroid* pColliding_entity, Entity* pBase_entity) -> bool {
		if (pColliding_entity->Return_Number_of_side() > 3)
		{
			g_iAsteroid_count++;
			new_asteroids.push_back(pColliding_entity->Crumble(pBase_entity->fVec, g_iAsteroid_count));
			return true;
		}
		pColliding_entity->Reset(hWnd);
		if (pColliding_entity->Return_bHasExpired())
		{
			vecExpiredAsteroid.push_back(pColliding_entity->ID);
		}
		return false;
	};



	auto p_Bullet_Collision_response = [&](IBullet* pBase_entity) {
		pBase_entity->Set_bHasCollided(true);
	};

	auto p_Asteroid_Collison_response = [&](IAsteroid* pBase_entity) {

	};

	//Gets the
	std::vector<Entity*> vecEntities = pQT->get(pEntity->fPos, pEntity->fRadius);


	switch (pEntity->enType)
	{
	case enEntity_Type::enPlayer:
		pBasePlayer = (IPlayer*)pEntity;
		//vecFoundNodes = pQTree->search(pbRange_Boundary);

		break;
	case enEntity_Type::enBullet:
		pBaseBullet = (IBullet*)pEntity;
		//vecFoundNodes = pQTree->search(pbRange_Boundary);
		break;
	case enEntity_Type::enAsteroid:
		pBaseAsteroid = (IAsteroid*)pEntity;
		//vecFoundNodes = pQTree->search(pbRange_Radius);
		break;
	default:
		break;
	}


	if (vecFoundNodes.size() == 1)
	{
		vecFoundNodes.at(0).bHasUpdated = false;
	}


	auto p_Collision_processing = [&](Entity* pEntityNode) {
		IPlayer* pPlayer = nullptr;
		IBullet* pBullet = nullptr;
		IAsteroid* pAsteroid = nullptr;
		int iCheck = 0;
		int iIDCheck = 0;

		switch (pEntityNode->enType)
		{
		case enEntity_Type::enPlayer:
		{
			pPlayer = (IPlayer*)(pEntityNode);
			int IDBCheck = -1;

			if (pEntity->enType == enEntity_Type::enBullet)
			{
				IDBCheck = p_Bullet_Collision(pBaseBullet);
			}



			if (IDBCheck != pPlayer->ID)
			{
				iCheck = p_Player_Collision(pPlayer);
				if (IDBCheck >= 0)
				{
					p_Bullet_Collision_response(pBaseBullet);
				}
			}



			if (pEntity->enType == enEntity_Type::enAsteroid)
			{
				if (p_Asteroid_Collision(pBaseAsteroid, pPlayer))
				{
					iCheck = -1;
				}
			}

			if (iCheck >= 0)
			{
				p_Player_Collision_response((vecPlayers.at(iCheck).get()));
			}

			break;
		}
		case enEntity_Type::enBullet:
			pBullet = (IBullet*)(pEntityNode);
			iIDCheck = p_Bullet_Collision(pBullet);
			if (pEntity->enType == enEntity_Type::enPlayer && iIDCheck != pEntity->ID)
			{
				iCheck = p_Player_Collision(pBasePlayer);
				p_Bullet_Collision_response(pBullet);
			}
			else if (pEntity->enType == enEntity_Type::enAsteroid)
			{
				if (p_Asteroid_Collision(pBaseAsteroid, pBullet))
				{
					iCheck = -1;
				}
				p_Bullet_Collision_response(pBullet);
			}

			if (iCheck != -1)
			{
				p_Player_Collision_response(vecPlayers.at(iIDCheck).get());
			}

			break;
		case enEntity_Type::enAsteroid:
			// dont currently care about rocks banging in to each other.
			if (pEntity->enType == enEntity_Type::enAsteroid)
			{
				break;
			}

			if (!pEntityNode->Advance_Entity_Collision(pEntity))
			{
				break;
			}

			pAsteroid = (IAsteroid*)(pEntityNode);



			if (!p_Asteroid_Collision(pAsteroid, pEntity))
			{
				if (pEntity->enType == enEntity_Type::enPlayer)
				{
					p_Player_Collision_response(pBasePlayer);
				}
				if (pEntity->enType == enEntity_Type::enBullet)
				{
					iIDCheck = p_Bullet_Collision(pBaseBullet);
					p_Bullet_Collision_response(pBaseBullet);
					p_Player_Collision_response(vecPlayers.at(iIDCheck).get());
				}
			}

			break;
		default:
			break;
		}
	};


	for (auto it = vecEntities.begin(); it != vecEntities.end(); it++)
	{

		if (*it == pEntity)
		{
			continue;
		}

		if (!pEntity->General_Entity_Collision(*it))
		{
			continue;
		}

		if (pEntity->enType == enEntity_Type::enAsteroid)
		{
			if (!pEntity->Advance_Entity_Collision(*it))
			{
				continue;
			}
		}

		p_Collision_processing(*it);
	}

	/*
	for (auto it = vecFoundNodes.begin(); it != vecFoundNodes.end(); it++)
	{
		Entity* pEntityNode = (Entity*)(it->pData);
		if (pEntity == pEntityNode)
			continue; // dont touch yourself

		if (!pEntity->General_Entity_Collision(pEntityNode))
		{
			continue;
		}


		p_Collision_processing(pEntityNode);

	}
	*/

	int index = 0;

	vector<shared_ptr<IAsteroid>> vecNewList;

	if (!deleted_asteroid_indexes.empty() || !new_asteroids.empty()) {
		// changes have been marked

		vecNewList.reserve(vecAsteroids.size() + new_asteroids.size() - deleted_asteroid_indexes.size());
		int iNextSkipIndex = -1;  // this is the next index to skip
		int deleted_vector_index = 0;
		if (!deleted_asteroid_indexes.empty()) {
			iNextSkipIndex = deleted_asteroid_indexes[deleted_vector_index];
		}
		for (int i = 0; i < index; i++) {
			if (iNextSkipIndex == i) {
				// skip the add.
				if (*deleted_asteroid_indexes.rbegin() != iNextSkipIndex) {
					// The last index in the naughty list isn't the current number 
					// so safe to assume more.
					deleted_vector_index++;
					iNextSkipIndex = deleted_asteroid_indexes.at(deleted_vector_index);
				}
				continue;
			}
			vecNewList.push_back(vecAsteroids.at(i));
		}

		std::copy(new_asteroids.begin(), new_asteroids.end(), std::back_inserter(vecNewList));
	}

	//std::copy(vecNewList.begin(), vecNewList.end(), std::back_inserter(vecAsteroids));
	return vecNewList;
}



void on_data_received(void* pData, size_t len, const void* pWinsockAddr, int len_of_addr, void* pContext)
{
	//Host* pHost_Server = (Host*)pContext;

	//pAddr: Socket address of the socket that sent to this receive event
	const sockaddr* pAddr = (const sockaddr*)pWinsockAddr;
	//pState: Context that records and passes around data in the server
	ServerState* pState = (ServerState*)pContext;



	//Checks to see if the data sent is a packet
	if (len_of_addr < sizeof(Packet))
	{
		//LoggerFactory::get()->Log("Foreign data being sent");
	}

	//intiallise variables
	const Packet* pPacket = (const Packet*)pData;
	const ClientConnected* pConnected = nullptr;
	const ServerStartPacket* pStart = nullptr;
	const GameStartResponse* pPlayerInfo = nullptr;
	const ServerEndPacket* pServerEnd = nullptr;
	const UpdatePacket* pUpdate = nullptr;
	const Update_Death_Packet* pDeath = nullptr;
	const Asteroid_UpdatePacket* pAsteroidUpdate = nullptr;
	const Asteroid_LocationPacket* pAsteroidLocation = nullptr;

	//search for packet based on ID
	switch (pPacket->id)
	{
	case int(Packet_ID::Client_Connect): // client connected
	{
		//Client is connected
		cout << "Client Connected" << endl;
		pConnected = (const ClientConnected*)pPacket;
		//Player object is made containning self-made ID and winsocket address
		Player* p = new Player(*pAddr, pState->iPlayerCount);

		//sent when a new connection is formed to show that the server is connected to the client
		packet_pointer_t pResponse = ClientConnectedResponse::create(p->iClientId);

		//Sent to show how many connections there are to the master Client
		packet_pointer_t pClientStart = ClientStartPacket::create();

		if (!pResponse)
		{
			int y = 0;
		}
		if (!pState->pServer)
		{
			int y = 0;
		}

		//Pushes back all the player's socket info onto player container
		pState->players.push_back(*p);
		pState->iPlayerCount++;

		//Sends a response to the respective client that sent the packet
		pState->pServer->Send(pResponse.get(), pResponse->size, &p->pAddr);
		//Sends a packet to the master client, each time a client is connected, including itself
		pState->pServer->Send(pClientStart.get(), pClientStart->size, &pState->players[0].pAddr);
		break;
	}
	case int(Packet_ID::Start_Server): // server start
	{
		//Server recieves packet that the game is about to start from master client
		//Send to all player's socket address the client start packet
		pStart = (const ServerStartPacket*)pPacket;

		//Creates the True entities and their IDs

		const float fTheta = (PI * 2) / pState->iTotalEntities; //The angle that each player are seperated from

		//Create true players and their IDs
		for (auto player = pState->players.begin(); player != pState->players.end(); player++)
		{

			//To update screen dimension of true are fixed to 1000 by 1000 (temporarely)
			std::shared_ptr<IPlayer> pPlayer = Player_Factory::Create(pState->pQt.get(),
				{ 0.50f + (0.15f * sinf(fTheta * (player->iClientId + 1))), 0.50f + (0.15f * cosf(fTheta * (player->iClientId + 1))) }, // position of each playr seperated evenly
				{ 0.00, 0.00 }, // velocity temp
				0.00f, //rotation
				{ 0,0, 25.00f / 1000, 25.00f / 1000 }, //hitbox screen dimensions are 1000 by 1000
				player->iClientId, // ID starting from 0
				false, enEntity_Type::enPlayer, // Entity type
				{ 0,0, 4.00f / 1000, 4.0f / 1000 }); //hitbox of the bullet

			pState->vecTrueEntities.push_back(std::move(pPlayer)); //Push back onto vector of entities

			pState->iTotalEntities++; //Total entities increased
		}

		//Create true asteroids and their IDs
		for (size_t i = 0; i < MAX_NUM_OF_ASTEROIDS; i++)
		{
			asteroid_t pAst = Asteroid_Factory::Create(pState->pQt.get(),
				{0.00, 0.00f}, // The position 0
				{0.00, 0.00f}, // the velocity 0
				0.00f, // the rotation
				std::rand() % 8 + 3, // The number of sides from 3 to 10
				0.00f, enEntity_Type::enAsteroid, // The type of entity
				pState->iTotalEntities); // The ID
			pAst->RandomLocation();
			pState->vecTrueEntities.push_back(std::move(pAst));

			pState->iTotalEntities++; //Increment total entities
		}


		//Container of the starting info of all players
		vector<PlayerStartingInfo> lstPlayers;

		//Goes through every Player and gives them an info of their ID, starting position, usually diagonally, and a 'name'
		for (size_t i = 0; i < size_t(pState->iPlayerCount); i++)
		{
			PlayerStartingInfo player = { pState->players[i].iClientId, {int(rand() % 41), int(rand() % 41)}, "Guess" };
			lstPlayers.push_back(player);
		}

		//Sent with Info to start the game
		packet_pointer_t pGameINFO = GameStartResponse::create(lstPlayers);

		//Sent to all players
		for (size_t i = 0; i < size_t(pState->iPlayerCount); i++)
		{
			pState->pServer->Send(pGameINFO.get(), /*sizeof(GameStartResponse) + (sizeof(PlayerStartingInfo)*lstPlayers.size())*/ pGameINFO->size, &pState->players[i].pAddr);
			//pState->pServer->Send(pGameStart.get(), sizeof(ClientStartPacket), pState->players[i].pAddr);
		}
		break;
	}
	case int(Packet_ID::End_Server): // server end
	{
		//pServerEnd = (const ServerEndPacket*)pPacket;

		//send end packet to all clients
		packet_pointer_t pClient_End = ClientEndPacket::create();
		for (auto it = pState->players.begin(); it != pState->players.end(); it++)
		{

			pState->pServer->Send(pClient_End.get(), pClient_End->size, &it->pAddr);
		}
		pState->func_trigger_server_quit;
		break;
	}
	case int(Packet_ID::Connection_Response_Client): // Client connected response
		break;
	case int(Packet_ID::Game_State): // Game state
		break;
		case int(Packet_ID::Start_Client): // Client start
		break;
	case int(Packet_ID::End_Client): // Client end
		break;
	case int(Packet_ID::Game_Start): // game start response
		//pPlayerInfo = (const GameStartResponse*)pPacket;

		break;
		case int(Packet_ID::Update): // update Packet
	{
		//Gives the data a context
		pUpdate = (const UpdatePacket*)pPacket; //Hold the keypress and the userinput
		pState->vecTrueEntities.at(pUpdate->iUser_ID)->Input_Entity(pUpdate->vecKey_Press);
		for (auto it = pUpdate->vecKey_Press.begin(); it != pUpdate->vecKey_Press.end(); it++)
		{
			if (*it == eKeyboard::Space)
			{
				player_t pPlayer = pState->vecTrueEntities.at(pUpdate->iUser_ID);
				pPlayer->Create_Bullet(pState->pQt.get(), pState->iTotalEntities);
				pState->iTotalEntities++;
			}
		}

		/*
			//Sends the plane directions and latest updated fruit coordinates
			packet_pointer_t pUpdateResponse = UpdateResponsePacket::create(pState->ptFruitPos, lstDataPlane, lstDirect, lstDataPrePlane);
			for (auto it = pState->players.begin(); it != pState->players.end(); it++)
			{
				pState->pServer->Send(pUpdateResponse.get(), pUpdateResponse->size, &it->pAddr);
			}

		}
		*/
		break;
	}
	case int(Packet_ID::Update_Response): // update packet response
		break;
		case int(Packet_ID::Update_Death):
		pDeath = (const Update_Death_Packet*)pPacket;
		for (auto it = pState->players.begin(); it != pState->players.end(); it++)
		{
			pState->pServer->Send(pDeath, pDeath->size, &it->pAddr);
		}
		break;
	case int(Packet_ID::Update_Asteroid) :
	{
		pAsteroidUpdate = (const Asteroid_UpdatePacket*)pPacket;
		
		//Intialise
		vector<point_t> vecAngles;
		vector<point_t> vecPos;
		
		float fTotal_Angle = 2 * PI; //Total angle
		float fAvg = fTotal_Angle / float(pAsteroidUpdate->iNum_of_sides); //Average angle
		float fAdjustment = fAvg / 2; // The adjustmet of the average
		float f_Current = 0; // The current angle
		const float frand_to_num = 1.0f / RAND_MAX; //fraction of random
		float fDis_two_min = 0.2f / log10(float(pAsteroidUpdate->iSize));

		float fDis_Min = sqrtf((float)((pAsteroidUpdate->iSize * pAsteroidUpdate->iSize) + 
			(pAsteroidUpdate->iSize * pAsteroidUpdate->iSize))) /
			(pAsteroidUpdate->iSize / (pAsteroidUpdate->iNum_of_sides * pAsteroidUpdate->iNum_of_sides)); // to be altered
		float wiggle_room_of_length = fDis_two_min / 4; //How much the distance can be altered


		float r_left = 0, r_top = 0, r_right = 0, r_bottom = 0, r_Distance = 0; //the hit box coords
		for (size_t i = 0; i < size_t(pAsteroidUpdate->iNum_of_sides); i++)
		{
			//             0..1  convert to  wiggle angle + avg angle
			// angle in radians
			f_Current += (i != size_t(pAsteroidUpdate->iNum_of_sides - 1)) ? ((rand() * frand_to_num) * fAdjustment) - (0.5f * fAdjustment) +
				fAvg : (fTotal_Angle - f_Current);

			//Calculates based on adjustment and wiggle room minus half the adjustment + the average, or if it the last side use the remaining angle
			float fDis = rand() * frand_to_num * wiggle_room_of_length + fDis_two_min;
			point_t pos = { f_Current, fDis };
			vecAngles.push_back(pos);

			//the distance from the centre.
			point_t PosAngle = { fDis * cosf(f_Current), fDis * sinf(f_Current) };

			//the position given based on the angle and the distance

			r_left = fmin(PosAngle.fx, r_left); //r_left = posAngle.X if it is smaller than itself
			r_top = fmin(PosAngle.fy, r_top); //r_top = posAngle.Y if it is smaller than itself
			r_right = fmax(PosAngle.fx, r_right); //r_right = posAngle.X if it is bigger than itself
			r_bottom = fmax(PosAngle.fy, r_bottom); //r_bottom = posAngle.Y if it is bigger than itself
			r_Distance = fmax(r_Distance, fDis); //r_Distance = fDis if it is bigger than itself

			vecPos.push_back(PosAngle); //Pushes back the position
			//vecPoints_Of_Contact.push_back(point_t{ PosAngle.X, PosAngle.Y });

		}
		//RectF rcHit(r_left, r_top, r_right - r_left, r_bottom - r_top);
		float fRadius = r_Distance; //Calculate the radius

		packet_pointer_t pResponse = Asteroid_UpdatePacketResponse::create(vecPos, vecAngles, fRadius);

		for (auto player = pState->players.begin(); player != pState->players.end(); player++)
		{
			pState->pServer->Send(pResponse.get(), pResponse->size, &player->pAddr);
		}
		

		//Send the points of contact, angles and radius
		//	X * float*2				Y * float	float
		break;
	}
	case int(Packet_ID::Update_Asteroid_Location) :
	{
		pAsteroidLocation = (Asteroid_LocationPacket*)pPacket;

		
		float fPercentage = std::rand() / float(RAND_MAX);
		int iZone = std::rand() % 4;
		//float fZonePercentageL = (float)(2 * pAsteroidLocation->fDistanceToLeft * fPercentage) - (pAsteroidLocation->fDistanceToLeft - 0.5f);
		//float fZonePercentageT = (float)(2 * pAsteroidLocation->fDistanceToTop * fPercentage) - (pAsteroidLocation->fDistanceToTop - 0.5f);
		point_t fPosZone = { 0, 0 };

		int iDirectionX = std::rand() % 2;
		int iDirectionY = std::rand() % 2;
		float fVecX = iDirectionX == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) :
			fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);
		float fVecY = iDirectionY == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) :
			fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);

		point_t fVec = {fVecX, fVecY};

		packet_pointer_t pResponse = Asteroid_LocationPacketResponse::create(fVec, fPercentage, iZone);
		for (auto player = pState->players.begin(); player != pState->players.end(); player++)
		{
			pState->pServer->Send(pResponse.get(), sizeof(Asteroid_LocationPacketResponse), &player->pAddr);
		}

		break;
	}
	default:
		break;
	}


}


unique_ptr<API::Objects::UDPInterface> host_server(UDPState& state) 
{
	state.is_quiting = false;

	auto func_trigger_for_quiting = [&]() {
		// triggers when someone wants the server to die.
		state.is_quiting = true;
		state.cv.notify_one();

	};

	

	state.pState = new ServerState(func_trigger_for_quiting);
	state.pSharedState = std::make_shared<ServerState>(func_trigger_for_quiting);
	ServerState* pState = (ServerState*)state.pSharedState.get();
	pState->pQt = factory::create_shared_qt();

	auto pUDP = API::Factories::UDPFactory::create_server(8080, on_data_received, state.pState);

	//ServerState* pState = (ServerState*&)(state.pState);
	pState->pServer = move(pUDP);

	return pUDP;
}

//Where the server updates the entities
//pState: the server's gamestate where the entities are stored
//Starts running upon server creation
void Update(ServerState * pState)
{
	for (auto it = pState->vecTrueEntities.begin(); it != pState->vecTrueEntities.end(); it++)
	{
		//(it)->get()->Update_Entity();
	}

}


int main()
{
    std::cout << "Hello World!\n";

	int iNum = GetTickCount();
	std::srand(iNum);

	bool is_quiting = false;
	UDPState state;
	auto pUDP = host_server(state);
	Update((ServerState*)state.pSharedState.get());

	state.wait();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
