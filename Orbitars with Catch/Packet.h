#pragma once
#include <memory>
#include <string>
#include "UDP_Interface.h"
#include "Universal_Structures.h"

enum class Packet_ID {
	Client_Connect = 0,
	Start_Server,
	End_Server,
	Connection_Response_Client,
	Game_State,
	Start_Client,
	End_Client,
	Game_Start,
	Update,
	Update_Response,
	Update_Death,
	Connection_Info,
	Update_Asteroid,
	Update_Asteroid_Response,
	Update_Asteroid_Location,
	Update_Asteroid_Location_Response
};

struct Packet {
	int id;
	size_t size;
	Packet(int id_)
		: id(id_)
	{

	}
};
struct custom_delete {
	void operator()(Packet* body) const {
		delete[](uint8_t*)body;
	}
};
typedef std::unique_ptr<Packet, custom_delete> packet_pointer_t;


// This packet is sent when a client attempts to connect to a host.
struct ClientConnected : public Packet {
public:
	// sockaddr    Im a idiot   - Eric 2020
	// sockaddr* pAddr;
	// std::string strName;
	const char szName[1];

private:
	ClientConnected(std::string strName_)
		: Packet(int(Packet_ID::Client_Connect))
		, szName()
	{
		//Copys the name, to szName
		strcpy_s((char*)szName, strName_.length() + 1, strName_.c_str());
	}
public:

	static packet_pointer_t create(std::string strName);

};

// This packet is sent from server to client on game start
struct ServerStartPacket : public Packet {
public:

	static packet_pointer_t create();

private:
	ServerStartPacket() : Packet(int(Packet_ID::Start_Server))
	{

	}
};

// sent from client to server on game start
struct ClientStartPacket : public Packet {
public:

	static packet_pointer_t create();

private:
	ClientStartPacket() : Packet(int(Packet_ID::Start_Client))
	{

	}
};

// This packet is sent from server to client on game end
struct ServerEndPacket : public Packet {
public:

	static packet_pointer_t create();

private:
	ServerEndPacket() : Packet(int(Packet_ID::End_Server))
	{

	}
};

struct ClientEndPacket : public Packet {
public:

	static packet_pointer_t create();

private:

	ClientEndPacket() : Packet(int(Packet_ID::End_Client))
	{

	}
};

struct PlayerStartingInfo
{
	int id;
	struct pos {
		int x, y;
	} starting;
	char name[20];

};

struct GameStartResponse : public Packet {
public:
	size_t count;
	PlayerStartingInfo arrPlayerInfo[1];
	size_t size_count;
	static packet_pointer_t create(std::vector<PlayerStartingInfo> vecPlayerAddresses);

private:
	GameStartResponse(std::vector<PlayerStartingInfo> vecAddressess)
		: Packet(int(Packet_ID::Game_Start))
		, count(vecAddressess.size())
	{
		size_count = 0;
		for (auto& it : vecAddressess)
		{
			//Generates the size of the players
			size_count += sizeof(PlayerStartingInfo) + (2 * sizeof(int)) + (sizeof(char) * std::strlen(it.name));
		}

		int y = 0;

		memcpy(arrPlayerInfo, &*vecAddressess.begin(), /*(count * (sizeof(PlayerStartingInfo)))*/ size_count);
	}
};

// This packet is sent from server to client on client connection
struct ClientConnectedResponse : public Packet {
public:

	size_t iPlayerID;

	static packet_pointer_t create(size_t iPlayerID_);

private:
	ClientConnectedResponse(size_t iPlayerID_)
		: Packet(int(Packet_ID::Connection_Response_Client))
		, iPlayerID(iPlayerID_)
	{

	}
};


struct network_point {
	uint32_t x;
	uint32_t y;

	network_point()
		: x(0)
		, y(0)
	{

	}

	network_point(uint32_t x_, uint32_t y_)
		: x(x_)
		, y(y_)
	{

	}

	static network_point from_point(POINT pt) {
		return network_point((uint32_t)pt.x, (uint32_t)pt.y);
	}

	network_point(POINT pt)
		: x(LONG(pt.x))
		, y(LONG(pt.y))
	{
	}

	POINT to_point() {
		return { LONG(x), LONG(y) };
	}
};

/*
struct UpdatePacket : public Packet {
public:
	size_t count;
	int iDirection;
	bool bIsFruitTaken;
	network_point arrPos[1];
	network_point arrPrePos[1];

	static packet_pointer_t create(std::vector<network_point> vecPos, int iDirection, bool bIsFruitTaken, std::vector<network_point> vecPrePos);

private:

	UpdatePacket(std::vector<network_point> vecPos_, int iDirection_, bool bIsFruitTaken_, std::vector<network_point> vecPrePos_)
		: Packet(int(Packet_ID::Update))
		, count(vecPos_.size())
		, iDirection(iDirection_)
		, bIsFruitTaken(bIsFruitTaken_)
	{
		memcpy(arrPos, &*vecPos_.begin(), count * sizeof(network_point));
		memcpy(arrPrePos, &*vecPrePos_.begin(), count * sizeof(network_point));
	}
};

struct UpdateResponsePacket : public Packet {
public:
	// This is the information of each snake.
	struct SNAKE_INFO_COMMON
	{
		int is_alive;			// is the snake alive?  0 - false 1 - true
		int head_direction;		// direction of head
		int length;				// Number of nodes in the array

	};
	struct SNAKE_INFO : SNAKE_INFO_COMMON
	{
		network_point nodes[1];			// At least 1 node for the head
		network_point Prenodes[1];
	};
	struct SNAKE_INFO_Dyn : SNAKE_INFO_COMMON
	{
		std::vector<network_point> nodes;	// At least 1 node for the head
		std::vector<network_point> Prenodes;
	};

	// WTF is this.
	struct ans {
		network_point pt;
		size_t num_of_snakes;  // not needed here?
		std::vector<SNAKE_INFO_Dyn> vec; // WTF is this.

	};

	ans unpack() const;

	static packet_pointer_t create(network_point ptFruitPos, std::list<std::list<network_point>> lstPlane, std::list<int> lstDirections, std::list<std::list<network_point>> lstPrePlane);

private:
	uint8_t buff;
	//network_point ptFruitPos;			// Current fruit location.
	//size_t number_of_snakes;	// Number of snakes are used in the dynamic array below
	// Dynamic arrays are at bottom of packet.
	// Dynamically in this structure are:   Array Of Snake Positions and array of snake head directions
	//  e.g:  SNAKE_INFO arrSnake[number_of_snakes]
	// SNAKE_INFO arrSnakes[1];

	// DOCUMENTATION !!!
	UpdateResponsePacket()
		: Packet(int(Packet_ID::Update_Response))
		, buff(0)

	{

	}

};

*/

struct UpdatePacket : public Packet {
public:
	
	std::vector<eKeyboard> vecKey_Press;
	int iUser_ID;

	static packet_pointer_t create(std::vector<eKeyboard> vecKey_Press_, int iUser_ID_);

private:

	UpdatePacket(std::vector<eKeyboard> vecKey_Press_, int iUser_ID_)
		: Packet(int(Packet_ID::Update))
		, vecKey_Press(vecKey_Press_)
		, iUser_ID(iUser_ID_)
	{
	
	}
};

struct UpdateResponsePacket : public Packet {
public:

	std::vector<eKeyboard> vecKeyPresses;

	static packet_pointer_t create(std::vector<eKeyboard> vecKeyPresses_);

private:

	UpdateResponsePacket(std::vector<eKeyboard> vecKeyPresses_)
		: Packet(int(Packet_ID::Update_Response))
	{

	}

};

// Packet Sent when player dies
// Sent to all clients by host
struct Update_Death_Packet : public Packet
{
public:

	int iPlayerID;

	static packet_pointer_t create(int PlayerID);

private:

	Update_Death_Packet(int iPlayer_)
		: Packet(int(Packet_ID::Update_Death))
		, iPlayerID(iPlayer_)
	{

	}

};

struct Asteroid_UpdatePacket : public Packet {
public:

	size_t iAsteroidID;
	size_t iNum_of_sides;
	size_t iSize;

	static packet_pointer_t create(size_t iPlayerID_, size_t iNum_of_sides, size_t iSize_);

private:
	Asteroid_UpdatePacket(size_t iPlayerID_, size_t iNum_of_sides, size_t iSize_)
		: Packet(int(Packet_ID::Update_Asteroid))
		, iAsteroidID(iPlayerID_)
		, iNum_of_sides(iNum_of_sides)
		, iSize(iSize_)
	{

	}
};

struct Asteroid_LocationPacket : public Packet {
public:
	
	float fRadius; //Radius of the asteroid

	static packet_pointer_t create(float fRadius_);

private:
	Asteroid_LocationPacket(float fRadius_)
		: Packet(int(Packet_ID::Update_Asteroid_Location))
		, fRadius(fRadius_)
	{

	}
};


struct Asteroid_UpdatePacketResponse : public Packet {
public:
	
	static packet_pointer_t create(std::vector<point_t> vecPos_, std::vector<point_t> vecAngles_, float fRadius_);

	std::vector<point_t> vecPos;
	std::vector<point_t> vecAngles;
	float fRadius;

private:
	Asteroid_UpdatePacketResponse(std::vector<point_t> vecPos_, std::vector<point_t> vecAngles_, float fRadius_)
		: Packet(int(Packet_ID::Update_Asteroid_Response))
		, vecPos(vecPos_)
		, vecAngles(vecAngles_)
		, fRadius(fRadius_)
	{

	}
};

struct Asteroid_LocationPacketResponse : public Packet 
{
public:

	static packet_pointer_t create(point_t fVec_, float fPercentage_, int iZone_);
	
	point_t fVec;
	float fPercentage;
	int iZone;

private:

	Asteroid_LocationPacketResponse(point_t fVec_, float fPercentage_, int iZone_)
		: Packet(int(Packet_ID::Update_Asteroid_Location_Response))
		, fVec(fVec_)
		, fPercentage(fPercentage_)
		, iZone(iZone_)
	{

	}
};

