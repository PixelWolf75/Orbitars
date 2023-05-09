#include "IScreen.h"
#include <vector>
#include <map>
#include <gdiplus.h>
#include "Player.h"
#include "Asteroid.h"
#include <random>
#include <algorithm>
#include <iterator>
#include <variant>
#include "IQuadTree.h"
#include "qt.h"

using namespace CODE;
using namespace CODE::SCREENS;
using namespace CODE::SCREENS::Objects;
using namespace std;
using namespace Gdiplus;

//typedef std::shared_ptr<Entity> entity_t;
int g_iAsteroid_count;

class Game : public IScreen
{
public:
	Game(Client_ScreenINFO_t&& pSomething_);
	~Game();

	virtual void Render(HWND hWnd, Window_Properties wpWinProp) override; // Renders the game
	virtual void Update(HWND hWnd, POINT ptCursor) override; // Updates the game based on frame rate
	virtual void Input(vector<eKeyboard> vecKey_press) override;

	vector<entity_t> vecEntities;
	/*
	void Bullet_Check(map<size_t, Player*> vecPlayers, Player* it); //Checks to see if the bullets hit anything
	void Player_Check();
	void Asteroid_Check(Entity* pEntity);
	*/

	vector<shared_ptr<IAsteroid>> Collision_Check(Entity* pEntity, HWND hWnd);

	vector<shared_ptr<IPlayer>> vecPlayers; //Players organised by IDs
	vector<shared_ptr<IAsteroid>> vecAsteroids; //Asteroids organised by IDs

	vector<int> vecExpiredAsteroid;

	std::unique_ptr<IQuadTree> pQTree;
	std::unique_ptr<qt> pQT;

	const sockaddr* addr_Host;

	int ID;
	int iPlayer_Count;
	int iAsteroid_Count;
	Client_ScreenINFO_t pGameInfo;
};

//Called first upon creation
//Creates the Game screen record
static int auto_registration() {
	ScreenController::Record("Game", [](Client_ScreenINFO_t&& pSomething) -> screen_t {
		return std::make_unique<Game>(move(pSomething));
		});
	return 0;
}

static int g_Register = auto_registration();

void on_recv(void* pData, size_t len, const void* pWinsockAddr, int len_of_addr, void* pContext)
{
	Game* pGame = (Game*)pContext;

	const sockaddr* pAddr = (const sockaddr*)pWinsockAddr;

	pGame->addr_Host = pAddr;

	const Packet* pPacket = (const Packet*)pData;
	const UpdateResponsePacket* _pUpdateResponse;
	const Update_Death_Packet* _pDeath;
	const Asteroid_LocationPacketResponse* pLocation;
	const Asteroid_UpdatePacketResponse* pAsteroidUpdate;

	if (len_of_addr < sizeof(Packet))
	{
		// escape
	}



	switch (pPacket->id)
	{
	case int(Packet_ID::Update) :
		break;
	case int(Packet_ID::Update_Response) :
		break;
	case int(Packet_ID::End_Server) :
		break;
	case int(Packet_ID::Update_Death) :
		break;
	case int(Packet_ID::Update_Asteroid_Response) :
		//When Asteroid changes
		pAsteroidUpdate = (Asteroid_UpdatePacketResponse*)pPacket;
		//pGame->vecAsteroids
		break;
	case int(Packet_ID::Update_Asteroid_Location_Response) :
		//When Asteroids are first created
		pLocation = (Asteroid_LocationPacketResponse*)pPacket;

		break;
	default:
		break;
	}

	int y = 0;
}


Game::Game(Client_ScreenINFO_t&& pSomething_)
	: ID(0)
	, iPlayer_Count(0)
	, pGameInfo(std::move(pSomething_))
{

	//pQTree = QuadTree_Factory::Create(4);
	pQT = factory::create_qt();

	player_t default_Player = Player_Factory::Create(pQT.get(), point_t{ 0.5f, 0.5f }, point_t{ 0.0f, 0.0f }, 0.00f, pGameInfo->rcPlayerRatio, 0, false, enEntity_Type::enPlayer, pGameInfo->rcBulletRatio);
	//pGameInfo->pServerConn->set_callback(on_recv, this);

	//Player  default_Player = (Player({ 0.5f, 0.5f }, { 0.0f, 0.0f }, 0, pGameInfo->rcPlayerRatio, 0, false, enEntity_Type::enPlayer, pGameInfo->rcBulletRatio));
	vecPlayers.push_back(default_Player);
	//pQT->add(vecPlayers.at(0));

	for (size_t i = 0; i < MAX_NUM_OF_ASTEROIDS; i++)
	{
		float fPercentage = std::rand() / float(RAND_MAX);
		int iZone = std::rand() % 4;
		float fZonePercentage = (1.5f * fPercentage) - 0.25f;
		point_t fPosZone = { 0, 0 };

		switch (iZone)
		{
		case 0:
			fPosZone = { fZonePercentage, -0.25 };
			break;
		case 1:
			fPosZone = { 1.25, fZonePercentage };
			break;
		case 2:
			fPosZone = { fZonePercentage, 1.25 };
			break;
		case 3:
			fPosZone = { -0.25, fZonePercentage };
			break;
		default:
			break;
		}
		int iDirectionX = std::rand() % 2;
		int iDirectionY = std::rand() % 2;
		float fVecX = iDirectionX == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) : fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);
		float fVecY = iDirectionY == 1 ? fmod(std::rand() / float(RAND_MAX), float(0.005)) : fmod(std::rand() / float(RAND_MAX), float(0.005)) * float(-1.0);


		asteroid_t Default = Asteroid_Factory::Create(pQT.get(), fPosZone, point_t({ fVecX, fVecY }), 0.00f, std::rand() % 8 + 3, 0.00f, enEntity_Type::enAsteroid, i);
		vecAsteroids.push_back(Default);
		//pQT->add(vecAsteroids.at(i));

	}
	asteroid_t Default = Asteroid_Factory::Create(pQT.get(), { 0.5f,0.6f }, { 0,0 }, 0, 5, 0, enEntity_Type::enAsteroid, 5);
	vecAsteroids.push_back(Default);
	vector<Entity *> vecTest = pQT->get({ 0,0 }, 1);
	//vecAsteroids.push_back(make_shared<Asteroid>(Asteroid({ 0.49, 0.3 }, { 0,0 }, .00f, 5, 0, enAsteroid, 0)));
	iPlayer_Count = vecPlayers.size();
	g_iAsteroid_count = vecAsteroids.size();
}

Game::~Game() {

}

bool operator==(point_t A, point_t B)
{
	return (A.fx == B.fy) && (A.fy == B.fy);
}

void Game::Render(HWND hWnd, Window_Properties wpWinProp)
{
	// renders the screen
	//tells players to render
	//tells bullets to render

	int y = 0;

	for (auto it = vecPlayers.begin(); it != vecPlayers.end(); it++)
	{
		(it)->get()->Render_Entity(hWnd,  &pGameInfo->mapImgFiles, wpWinProp);
	}

	for (auto it = vecAsteroids.begin(); it != vecAsteroids.end(); it++)
	{

		(it)->get()->Render_Entity(hWnd, &pGameInfo->mapImgFiles, wpWinProp);
	}

}

/*
typedef map<size_t, Player> Players;
typedef map<size_t, Bullet> Bullets;
typedef vector<Asteroid> Asteroids;
*/
//Checks to see if the bullets hit anything
//vecPlayers: the map to the player to alter the player connected to the bullet
//it: the pointer to the player that fired the bullets

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


void Game::Update(HWND hWnd, POINT ptCursor)
{
	//Updates the position of the player based on the input
	//Updates the player position based on gravitational constant
	RECT rc;
	GetClientRect(hWnd, &rc);
	search_area_t pbArea = search_area_t{ 0, 0, 1.0, 1.0 };

	int y = 0;

	//pQTree->Reset_QuadTree(pbArea);

	for (auto it = vecPlayers.begin(); it != vecPlayers.end(); it++)
	{
		(it)->get()->Update_Entity(hWnd, ptCursor);
		//node nPlayer = node{ &(*it), (it)->get()->fPos };
		//pQTree->insert_node(nPlayer);
	}
	int x = 0;
	for (auto it = vecAsteroids.begin(); it != vecAsteroids.end(); it++)
	{
		(it)->get()->Update_Entity(hWnd, ptCursor);
	}

	/*
	Bullet* pBullet_Test = &Bullet({ 0.49, 0.3 }, { 0,0 }, 0, pGameInfo->rcBulletRatio, enBullet, 0);
	node nBullet = node{ pBullet_Test, pBullet_Test->fPos };
	pQTree->insert_node(nBullet);
	Collision_Check(pBullet_Test);
	*/

	for (auto it = vecPlayers.begin(); it != vecPlayers.end(); it++)
	{
		auto vecSB = it->get()->Return_Shared_Bullets();

		for (auto itBullet = vecSB.begin(); itBullet != vecSB.end(); itBullet++)
		{
			Collision_Check(itBullet->get(), hWnd);

		}

		if (!it->get()->Return_State())
		{
			continue;
		}

		//search_area_t pbRange = { it->fPos.fx, it->fPos.fy, float(it->rcHitBox.right), float(it->rcHitBox.bottom)};
		Collision_Check(it->get(), hWnd);
	}

	vector<asteroid_t> vecNew_Asteroids;
	vector<asteroid_t> out;
	

	for (auto it = vecAsteroids.begin(); it != vecAsteroids.end(); it++)
	{
		vector<shared_ptr<IAsteroid>> vecNewAsteroids = Collision_Check(it->get(), hWnd);
		copy(vecNewAsteroids.begin(), vecNewAsteroids.end(), back_inserter(vecNew_Asteroids));
	}

	out.reserve(vecAsteroids.size() - vecExpiredAsteroid.size());
	vecExpiredAsteroid.push_back(-1);
	vector<int>::iterator itExpired = vecExpiredAsteroid.begin();

	std::copy_if(vecAsteroids.cbegin(), vecAsteroids.cend(), std::back_inserter(out),
		[&itExpired](const asteroid_t& a) {
			bool bMatch = a->ID == *itExpired;
			if (bMatch) {
				itExpired++;
			}
			return !bMatch;
		});
	vecAsteroids = out;
	vecExpiredAsteroid.clear();

	copy(vecNew_Asteroids.begin(), vecNew_Asteroids.end(), back_inserter(vecAsteroids));

}



void Game::Input(vector<eKeyboard> vecKey_press)
{
	//Input command based onplayer based on ID
	//When server is implemented make it so that this sends a packet with input
	for (auto eKey_press = vecKey_press.begin(); eKey_press != vecKey_press.end(); eKey_press++)
	{
		if (*eKey_press == eKeyboard::Space)
		{
			vecPlayers.at(ID)->Create_Bullet(pQT.get(), 0);
		}
	}
	vecPlayers.at(ID)->Input_Entity(vecKey_press);
}