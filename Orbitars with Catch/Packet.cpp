#include "Packet.h"

packet_pointer_t ClientConnected::create(std::string strName)
{
	// calculate total memory require
	const size_t number_of_bytes = sizeof(ClientConnected) + strName.length() + 1;
	uint8_t* raw = new uint8_t[number_of_bytes];

	// This is a placement new operator
	ClientConnected* p = new (raw) ClientConnected(strName);

	//Creates unique with custom delete, custom delete is so that it knows how to delete itself automatically
	std::unique_ptr<ClientConnected, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t ServerStartPacket::create()
{
	const size_t number_of_bytes = sizeof(ServerStartPacket);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	ServerStartPacket* p = new (raw_data) ServerStartPacket();

	std::unique_ptr<ServerStartPacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t ClientStartPacket::create()
{
	const size_t number_of_bytes = sizeof(ClientStartPacket);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	ClientStartPacket* p = new (raw_data) ClientStartPacket();

	std::unique_ptr<ClientStartPacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t ServerEndPacket::create()
{
	const size_t number_of_bytes = sizeof(ServerEndPacket);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	ServerEndPacket* p = new (raw_data) ServerEndPacket();

	std::unique_ptr<ServerEndPacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t ClientEndPacket::create()
{
	const size_t number_of_bytes = sizeof(ClientEndPacket);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	ClientEndPacket* p = new (raw_data) ClientEndPacket();

	std::unique_ptr<ClientEndPacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t GameStartResponse::create(std::vector<PlayerStartingInfo> vecPlayerAddresses)
{

	size_t number_of_bytes = sizeof(GameStartResponse);

	//For every player add the size of its attributes
	for (auto& player : vecPlayerAddresses)
	{
		number_of_bytes += sizeof(PlayerStartingInfo) + sizeof(int) * 2 + (sizeof(char) * (std::strlen(player.name)));
	}

	uint8_t* raw_data = new uint8_t[number_of_bytes];

	GameStartResponse* p = new (raw_data) GameStartResponse(vecPlayerAddresses);

	std::unique_ptr<GameStartResponse, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t ClientConnectedResponse::create(size_t iPlayerID_)
{
	const size_t number_of_bytes = sizeof(ClientConnectedResponse) + sizeof(size_t);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	ClientConnectedResponse* p = new (raw_data) ClientConnectedResponse(iPlayerID_);

	std::unique_ptr<ClientConnectedResponse, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t UpdatePacket::create(std::vector<eKeyboard> vecKeyboard, int iUser_ID_)
{
	const size_t number_of_bytes = sizeof(UpdatePacket) + (vecKeyboard.size() * sizeof(eKeyboard)) + sizeof(int);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	UpdatePacket* p = new (raw_data) UpdatePacket(vecKeyboard, iUser_ID_);

	std::unique_ptr<UpdatePacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

/*
// Lack of documentation,  wtf is going on.
packet_pointer_t UpdateResponsePacket::create(network_point ptFruitPos, std::list<std::list<network_point>> lstPlane, std::list<int> lstDirections, std::list<std::list<network_point>> lstPrePlane)
{
	size_t number_of_bytes = sizeof(UpdateResponsePacket)   // Packet header
		+ sizeof(network_point) // ptFruitPos
		+ sizeof(size_t);       // number_of_snakes


	// For each list of nodes, build up the size of the SNAKE_INFO.
	// Add up the bytes required for vector<network_point> + SNAKE_INFO overhead
	for (auto& it : lstPlane)
	{
		size_t len = it.size();
		number_of_bytes += (len * sizeof(network_point) * 2) + sizeof(SNAKE_INFO_COMMON);
	}

	//size = number_of_bytes;

	// Allocate the buffer to hold the packet.
	void* raw_data = new uint8_t[number_of_bytes];

	// Run the constructor in the previousally allocated buffer.
	UpdateResponsePacket* p = new (raw_data) UpdateResponsePacket();


	Packet* pPacket = (Packet*)raw_data;
	pPacket->id = int(Packet_ID::Update_Response);

	network_point& ptPacketFruitPos = *(network_point*)(void*)(pPacket + 1); // moves to the start of pPacketFruitPos
	size_t& number_of_snakes = *(size_t*)(&ptPacketFruitPos + 1); // moves to the start of number_of_snakes
	SNAKE_INFO* pCurrentSnake = (SNAKE_INFO*)(&number_of_snakes + 1); // moves to the start of pCurrentSnake
	ptPacketFruitPos = ptFruitPos;
	number_of_snakes = lstPlane.size();
	auto itSnake = lstPlane.begin();
	auto itDir = lstDirections.begin();
	auto itPrevious = lstPrePlane.begin();
	for (; itSnake != lstPlane.end(); itSnake++, itDir++, itPrevious++)
	{
		pCurrentSnake->is_alive = 1;
		pCurrentSnake->head_direction = *itDir;
		pCurrentSnake->length = itSnake->size();

		copy(itSnake->begin(), itSnake->end(), (network_point*)pCurrentSnake->nodes);
		copy(itPrevious->begin(), itPrevious->end(), (network_point*)pCurrentSnake->Prenodes);

		pCurrentSnake = (SNAKE_INFO*)(pCurrentSnake->nodes + pCurrentSnake->length);
		pCurrentSnake = (SNAKE_INFO*)(pCurrentSnake->Prenodes + pCurrentSnake->length);
	}

	//network_point* pnpPos = (network_point*)(void*)(pPacket + 1);
	//*pnpPos = ptFruitPos;
	//size_t* piPos = (size_t*)(pnpPos + 1);
	//*piPos = lstPlane.size();
	//SNAKE_INFO* pSIPos = (SNAKE_INFO*)(piPos + 1);

	//// Put the data in to the new buffer

	//auto itSnake = lstPlane.begin();
	//auto itDir = lstDirections.begin();


	//// pBuf points to just below the number_of_snakes in the buffer.				
	//for (size_t i = 0; i < *piPos; i++, itSnake++, itDir++)
	//{
	//	// Write the current snake info in the buffer.
	//	pCurrentSnake->is_alive = 1;
	//	pCurrentSnake->head_direction = *itDir;
	//	pCurrentSnake->length = itSnake->size();
	//	copy(itSnake->begin(), itSnake->end(), pCurrentSnake->nodes);

	//	// Calculate new pSIPos position (just after current pos)
	//	pCurrentSnake = (SNAKE_INFO*)(pSIPos->nodes + pSIPos->length);
	//}

	std::unique_ptr<UpdateResponsePacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return(result);
}

// unpacks the raw data into variables we can interpret
UpdateResponsePacket::ans UpdateResponsePacket::unpack() const
{
	//The buffer containning raw info
	const uint8_t* p = &this->buff;
	size_t& number_of_snakes = *(size_t*)((network_point*)p + 1);
	SNAKE_INFO* pSnakeInfo = (SNAKE_INFO*)(&number_of_snakes + 1);
	pSnakeInfo = (SNAKE_INFO*)(&number_of_snakes + 1);
	ans result = {
		*(network_point*)p,
		number_of_snakes
	};

	// Do a loop to extract all the snake infos
	for (size_t current_snake = 0; current_snake < number_of_snakes; current_snake++)
	{
		SNAKE_INFO_Dyn si;
		si.is_alive = pSnakeInfo->is_alive;
		si.head_direction = pSnakeInfo->head_direction;
		si.length = pSnakeInfo->length;
		copy(pSnakeInfo->nodes, pSnakeInfo->nodes + si.length, std::back_inserter(si.nodes));
		copy(pSnakeInfo->Prenodes, pSnakeInfo->Prenodes + si.length, std::back_inserter(si.Prenodes));
		result.vec.push_back(si);
	}
	return result;
}

*/

packet_pointer_t Update_Death_Packet::create(int iPlayerID)
{
	const size_t number_of_bytes = sizeof(Update_Death_Packet);

	uint8_t* raw_data = new uint8_t[number_of_bytes];

	Update_Death_Packet* result = new (raw_data) Update_Death_Packet(iPlayerID);

	std::unique_ptr<Update_Death_Packet, custom_delete> p(result);
	p->size = number_of_bytes;
	return(p);
}

packet_pointer_t Asteroid_UpdatePacket::create(size_t iAsteroidID_, size_t iNum_of_sides_, size_t iSize_)
{
	const size_t number_of_bytes = sizeof(Asteroid_UpdatePacket) + sizeof(size_t)*3;
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	Asteroid_UpdatePacket* p = new (raw_data) Asteroid_UpdatePacket(iAsteroidID_, iNum_of_sides_, iSize_);

	std::unique_ptr<Asteroid_UpdatePacket, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t Asteroid_UpdatePacketResponse::create(std::vector<point_t> vecPos_, std::vector<point_t> vecAngles_, float fRadius_)
{
	const size_t number_of_bytes = sizeof(Asteroid_UpdatePacketResponse) + (vecPos_.size()+ vecAngles_.size()) * sizeof(point_t);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	Asteroid_UpdatePacketResponse* p = new (raw_data) Asteroid_UpdatePacketResponse(vecPos_, vecAngles_, fRadius_);

	std::unique_ptr<Asteroid_UpdatePacketResponse, custom_delete> result(p);
	p->size = number_of_bytes;
	return result;
}

packet_pointer_t Asteroid_LocationPacketResponse::create(point_t fVec_, float fPercentage_, int iZone_)
{
	const size_t number_of_bytes = sizeof(Asteroid_LocationPacketResponse);
	uint8_t* raw_data = new uint8_t[number_of_bytes];
	
	Asteroid_LocationPacketResponse* p = new (raw_data) Asteroid_LocationPacketResponse(fVec_, fPercentage_, iZone_);
	std::unique_ptr<Asteroid_LocationPacketResponse, custom_delete> result(p);
	p->size = number_of_bytes;

	return result;
}

packet_pointer_t Asteroid_LocationPacket::create(float fRadius_)
{
	const size_t number_of_bytes = sizeof(Asteroid_LocationPacket);
	uint8_t* raw_data = new uint8_t[number_of_bytes];

	Asteroid_LocationPacket* p = new (raw_data) Asteroid_LocationPacket(fRadius_);
	std::unique_ptr<Asteroid_LocationPacket, custom_delete> result(p);
	p->size = number_of_bytes;

	return result;
}
