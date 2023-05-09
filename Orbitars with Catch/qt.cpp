#include "qt.h"
#include <map>
#include <iterator>
#include <algorithm>
#include <stdexcept>

#ifndef NDEBUG
#include <Windows.h>
#include <string>
#endif 
using namespace std;

/*
static int entity_id_counter = 1;
Entity::Entity(point_t position_, double radius_)
	: position(position_)
	, radius(radius_)
	, id(entity_id_counter++)
{
	char buff[200];
	snprintf(buff, sizeof(buff), "Entity %d\n", this->id);
	OutputDebugStringA(buff);
}
Entity::~Entity() noexcept(false) {
	char buff[200];
	snprintf(buff, sizeof(buff), "~Entity %d\n", this->id);
	OutputDebugStringA(buff);
}


Astroid::Astroid(point_t position, double radius)
	: Entity(position, radius)
{
}

Player::Player(point_t position, double radius)
	: Entity(position, radius)
{
}


/*
player_t Player::create(point_t position, double radius, qt* pQt_)
{

	player_t pPlayer = shared_ptr<Player>(new Player(position, radius));
	pQt_->add(pPlayer);
	pQt = pQt_;
	return pPlayer;

}

astroid_t Astroid::create(point_t position, double radius, qt* pQt_)
{
	//pQt = &(*pQt_);
	astroid_t pAsteroid =  shared_ptr<Astroid>(new Astroid(position, radius));
	pQt_->add(pAsteroid);
	return pAsteroid;

}

Astroid::~Astroid()
{
	//pQt->remove(make_shared<Astroid>(*this));
}

Player::~Player()
{

}
*/
int quad_id_counter = 0;
class impl_quadtree : public qt
{
public:
	int debug_id;
	struct rect_t {
		point_t lt, rb;
	};
	size_t depth;
	rect_t area;
	point_t mid;
	vector<unique_ptr<impl_quadtree>> vecSubs;
	vector<Entity*> vecEntities;
	struct object_detail_t {
		point_t pos;
		impl_quadtree* pNode;
	};
	unique_ptr<map<Entity*, object_detail_t>>  pmapObjectDetails;  // coords valid on top only
	impl_quadtree* pTop;

	impl_quadtree(size_t depth_, rect_t area_, impl_quadtree* _pTop, bool is_first = true)
		: depth(depth_)
		, area(area_)
		, pmapObjectDetails(nullptr)
		, pTop(_pTop)
	{
		debug_id = quad_id_counter++;
		this->pTop = this->pTop ? this->pTop : this; // Make the top be the top or current if nullptr.

		if (is_first) {
			pmapObjectDetails = make_unique<map<Entity*, object_detail_t>>();
		}

		mid = {
				(area.rb.fx - area.lt.fx) / 2 + area.lt.fx,
				(area.rb.fy - area.lt.fy) / 2 + area.lt.fy
		};
		if (depth) {
			vecSubs.reserve(4);


			rect_t sub_mid[] = {
				{ area.lt, mid },
				{ {mid.fx, area.lt.fy}, {area.rb.fx, mid.fy}},
				{ {area.lt.fx, mid.fy}, {mid.fx, area.rb.fy}},
				{ mid, area.rb },
			};

			for (int i = 0; i < 4; i++) {
				vecSubs.push_back(make_unique<impl_quadtree>(depth - 1, sub_mid[i], pTop, false));
			}
		}
	}
	virtual ~impl_quadtree() {

	}

	/*
		Return a vector of references of the nodes effected.
	*/
	vector<impl_quadtree*> get_sub_nodes_effected(point_t pos, double radius)
	{
		vector<impl_quadtree*> vec;
		if (!depth) {
			return vec;
		}

		if (pos.fx + radius < this->area.lt.fx ||
			pos.fy + radius < this->area.lt.fy ||
			pos.fx - radius > this->area.rb.fx ||
			pos.fy - radius > this->area.rb.fy) {
			// not inside the quad node.
			return vec;
		}

		if (pos.fx - radius < mid.fx) {
			// it's in the left side
			if (pos.fy - radius < mid.fy) {
				// it's in the top side
				vec.push_back(vecSubs[0].get());
			}
			if (pos.fy + radius > mid.fy) {
				// it's in the bottom side
				vec.push_back(vecSubs[2].get());
			}
		}
		if (pos.fx + radius > mid.fx) {
			// it's in the right side
			if (pos.fy - radius < mid.fy) {
				// it's in the top side
				vec.push_back(vecSubs[1].get());
			}
			if (pos.fy + radius > mid.fy) {
				// it's in the bottom side
				vec.push_back(vecSubs[3].get());
			}
		}
		return vec;
	}

	/*
	* Returns the nodes that can fully hold the query.
	*/
	//vector<impl_quadtree*> get_smallest_container_for_query(point_t pos, double radius)
	//{
	//	vector<impl_quadtree*> vec;
	//	if (!depth) {
	//		return vec;
	//	}
	//	for (auto& sub_node : this->vecSubs) {
	//		if (sub_node->area.lt.fx <= pos.fx - radius &&
	//			sub_node->area.lt.fy <= pos.fy - radius &&
	//			sub_node->area.rb.fx >= pos.fx + radius &&
	//			sub_node->area.rb.fy >= pos.fy + radius) {
	//			// this node fully contains the query
	//			vec.push_back(sub_node.get());
	//		}
	//	}
	//	return vec;
	//}

	impl_quadtree* get_smallest_container_for_entity(point_t pos, double radius)
	{
		vector<impl_quadtree*> vec;
		if (!depth) {
			return this;
		}
		for (auto& sub_node : this->vecSubs) {
			if (sub_node->area.lt.fx <= pos.fx - radius &&
				sub_node->area.lt.fy <= pos.fy - radius &&
				sub_node->area.rb.fx >= pos.fx + radius &&
				sub_node->area.rb.fy >= pos.fy + radius) {
				// this node fully contains the query
				vec.push_back(sub_node.get());
			}
		}
		if (vec.size() > 1) {
			return this;
		}
		if (vec.size() == 1)
			return vec[0]->get_smallest_container_for_entity(pos, radius);

		return this;
	}

	// Inherited via qt
	virtual void add(Entity* pEntity) override
	{
		auto pNode = get_smallest_container_for_entity(pEntity->fPos, pEntity->fRadius);
	
		// only the top level records the old position.
		if (pTop->pmapObjectDetails->find(pEntity) != pTop->pmapObjectDetails->end()) {
			throw std::runtime_error("No double adding, entity already exists.");
		}
		pTop->pmapObjectDetails->insert(pair<Entity*, object_detail_t>(pEntity, { pEntity->fPos, pNode }));

		pNode->vecEntities.push_back(pEntity);
	}



	virtual void remove(Entity* pEntity) override
	{
		// remove old position only from top level.
		auto itObjectDetails = pTop->pmapObjectDetails->find(pEntity);
		if (itObjectDetails == pTop->pmapObjectDetails->end()) {
			throw std::runtime_error("404  Cant remove object.");
		}

		auto& vec = itObjectDetails->second.pNode->vecEntities;
		auto itEntity = find(vec.begin(), vec.end(), pEntity);
		if (itEntity == vec.end()) {
			throw std::runtime_error("40404  Cant remove object.");
		}
		vec.erase(itEntity);
		pTop->pmapObjectDetails->erase(pEntity);



		//pTop->pmapObjectDetails->erase(pEntity);

		//auto list = get_smallest_container_for_query(pEntity->fPos, pEntity->fRadius);
		//if (!list.empty()) {
		//	for (auto& qt : list) {
		//		qt->remove(pEntity);
		//	}
		//}
		//else {
		//	// the current node has the item
		//	auto it = find(vecEntities.begin(), vecEntities.end(), pEntity);
		//	if (it != vecEntities.end()) {
		//		vecEntities.erase(it);
		//	}
		//	else {
		//		// log error!!!
		//		throw std::runtime_error("404  Cant remove object.");
		//	}
		//}

	}

	virtual void update(Entity* pEntity) override
	{
		if (!pmapObjectDetails) {
			throw runtime_error("No map?");
		}

		auto itPos = pmapObjectDetails->find(pEntity);

		if (itPos == pmapObjectDetails->end()) {
			throw runtime_error("Updating entity not in quad tree?");
		}

		update_inner(pEntity, (*pmapObjectDetails)[pEntity]);
		(*pmapObjectDetails)[pEntity].pos = pEntity->fPos;
	}
	void update_inner(Entity* pEntity, object_detail_t& details)
	{
		if (pEntity->enType == enEntity_Type::enAsteroid) {
			char sz[200];
			sprintf_s(sz, "[%d]: Updating to {%f,%f}\n", pEntity->ID,
				pEntity->fPos.fx, pEntity->fPos.fy
			);
			OutputDebugStringA(sz);
		}

		// check which node tree it's in.	
		// auto list_old = get_smallest_container_for_query(old_pos, pEntity->fRadius);
		auto pDestinationNode = get_smallest_container_for_entity(pEntity->fPos, pEntity->fRadius);
		auto pSrcNode = details.pNode;

		if (pDestinationNode == pSrcNode) {
			// simple update only
			details.pos = pEntity->fPos;
			return;
		}

		// If node is different,  remove from old and and to new.
		pSrcNode->remove(pEntity);
		pDestinationNode->add(pEntity);





		//vector<impl_quadtree*> vecToUpdate, vecToRemove, vecToAdd;
		//// for each old in new, then update
		////          old not in new then remove
		////          new not in old then add
		////          if no new, then add to current level
		//







		//for (auto& qt : list_old) {
		//	if (find(list_new.begin(), list_new.end(), qt) == list_new.end()) {
		//		// not in, therefore remove
		//		vecToRemove.push_back(qt);
		//	}
		//	else {
		//		vecToUpdate.push_back(qt);
		//	}
		//}
		//if (list_new.empty()) {
		//	// if no new list, then it can't fix in a sub node so add to current node.
		//	list_new.push_back(this);
		//}
		//for (auto& qt : list_new) {
		//	auto it = find(list_old.begin(), list_old.end(), qt);
		//	if (it == list_old.end()) {
		//		// if not in old, then add

		//		// Check its not in the current node.
		//		// So if want to add to dpeth 3 and remove from depth 2, check it's not in d2
		//		if (find(vecEntities.begin(), vecEntities.end(), pEntity) != vecEntities.end()) {
		//			// It's in D2 wanting to add to another.
		//			vecToRemove.push_back(this);
		//		}

		//		vecToAdd.push_back(qt);
		//	}
		//	else {
		//		// ignore if new is in old. 
		//	}
		//}


		//for (auto& qt : vecToUpdate) {
		//	qt->update_inner(pEntity, old_pos);
		//}
		//for (auto& qt : vecToRemove) {
		//	// Use the old position to remove.
		//	if (pEntity->enType == enEntity_Type::enAsteroid) {
		//		char sz[200];
		//		sprintf_s(sz, "[%d]: Remove from %d:{%f,%f,%f,%f}\n", pEntity->ID,
		//			qt->debug_id,
		//			qt->area.lt.fx, qt->area.lt.fy, qt->area.rb.fx, qt->area.rb.fy
		//		);
		//		OutputDebugStringA(sz);
		//	}

		//	auto new_pos = pEntity->fPos;
		//	pEntity->fPos = old_pos;
		//	qt->remove(pEntity);
		//	pEntity->fPos = new_pos;
		//}
		//for (auto& qt : vecToAdd) {
		//	if (pEntity->enType == enEntity_Type::enAsteroid) {
		//		char sz[200];
		//		sprintf_s(sz, "[%d]: Add to %d:{%f,%f,%f,%f}\n", pEntity->ID,
		//			qt->debug_id,
		//			qt->area.lt.fx, qt->area.lt.fy, qt->area.rb.fx, qt->area.rb.fy
		//		);
		//		OutputDebugStringA(sz);
		//	}
		//	qt->add(pEntity);
		//}
	}

	virtual std::vector<Entity*> get(point_t position, double radius) override
	{
		vector<Entity*> vecResult;
		vector<Entity*> vecHits;

		auto list = get_sub_nodes_effected(position, radius);
		for (auto& qt : list) {
			for (auto& entity : qt->get(position, radius)) {
				vecResult.push_back(entity);

			}
		}
		// also include current node
		//copy(vecEntities.begin(), vecEntities.end(), back_inserter(vecResult));
		if (vecEntities.empty())
			return vecResult;


		double r2 = radius * radius;
		// filter all entitys and add to results as needed
		for (auto& pEntity : vecEntities) {
			double dx = (pEntity->fPos.fx - position.fx);
			double dy = (pEntity->fPos.fy - position.fy);
			double delta_distance = sqrt(dx * dx + dy * dy);
			double delta_distance_less_radius = delta_distance - pEntity->fRadius;
			//double crash_dis_squared = sqrt(pEntity->fRadius * pEntity->fRadius + r2);
			if (delta_distance_less_radius < radius) {
				// hit
				vecResult.push_back(pEntity);
			}
		}
		return vecResult;
	}

};

std::unique_ptr<qt> factory::create_qt()
{
	impl_quadtree::rect_t area = { { 0, 0 }, {1.0, 1.0} };
	return make_unique<impl_quadtree>(3, area, nullptr, true);
}

std::shared_ptr<qt> factory::create_shared_qt()
{
	impl_quadtree::rect_t area = { {0,0}, {1.0, 1.0} };
	return make_shared<impl_quadtree>(3, area, nullptr, true);
}

qt::qt()
{
}


/*
class Astroid_Imp : public Astroid
{
public:
	Astroid_Imp(point_t position, double radius, qt* pQt_);
	~Astroid_Imp() noexcept(false);
	qt* pQt;
};

Astroid_Imp::Astroid_Imp(point_t position, double radius, qt* pQt_)
	:Astroid(position, radius)
	, pQt(pQt_)
{

}

Astroid_Imp::~Astroid_Imp() noexcept(false)
{
	try {
		pQt->remove(this);
	}
	catch (runtime_error&) {

	}
}

astroid_t Astroid_Factory::create(point_t position, double radius, qt* pQt)
{
	astroid_t pAsteroid = shared_ptr<Astroid>(new Astroid(position, radius));
	pQt->add(pAsteroid.get());
	return pAsteroid;
}

class Player_Imp : public Player
{
public:
	Player_Imp(point_t position, double radius, qt* pQt_);
	~Player_Imp() noexcept(false);
	qt* pQt;
};


Player_Imp::Player_Imp(point_t position, double radius, qt* pQt_)
	: Player(position, radius)
	, pQt(pQt_)
{

}

Player_Imp::~Player_Imp() noexcept(false)
{
	try {
		pQt->remove(this);
	}
	catch (runtime_error&) {

	}
}

player_t Player_Factory::create(point_t position, double radius, qt* pQt)
{
	player_t pPlayer = shared_ptr<Player>(new Player_Imp(position, radius, pQt));
	pQt->add(pPlayer.get());
	return pPlayer;
}
*/