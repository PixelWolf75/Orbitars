#include "qt.h"
#include <map>
#include <iterator>
#include <algorithm>
#include <stdexcept>
/*
#ifndef NDEBUG
#include <Windows.h>
#include <string>
#endif 
using namespace std;
static int entity_id_counter = 1;
Entity::Entity(postion_t position_, double radius_)
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


Astroid::Astroid(postion_t position, double radius)
	: Entity(position, radius)
{
}

Player::Player(postion_t position, double radius)
	: Entity(position, radius)
{
}


/*
player_t Player::create(postion_t position, double radius, qt* pQt_)
{
	
	player_t pPlayer = shared_ptr<Player>(new Player(position, radius));
	pQt_->add(pPlayer);
	pQt = pQt_;
	return pPlayer;
	
}

astroid_t Astroid::create(postion_t position, double radius, qt* pQt_)
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

int quad_id_counter = 0;
class impl_quadtree : public qt
{
public:
	int debug_id;
	struct rect_t {
		postion_t lt, rb;
	};
	size_t depth;
	rect_t area;
	postion_t mid;
	vector<unique_ptr<impl_quadtree>> vecSubs;
	vector<Entity*> vecEntities;

	unique_ptr<map<Entity*, postion_t>>  pmapOldCoords;  // coords valid on top only
	impl_quadtree* pTop;

	impl_quadtree(size_t depth_, rect_t area_, impl_quadtree* _pTop, bool is_first = true)
		: depth(depth_)
		, area(area_)
		, pmapOldCoords(nullptr)
		, pTop(_pTop)
	{
		debug_id = quad_id_counter++;
		this->pTop = this->pTop ? this->pTop : this; // Make the top be the top or current if nullptr.

		if (is_first) {
			pmapOldCoords = make_unique<map<Entity*, postion_t>>();
		}

		mid = {
				(area.rb.x - area.lt.x) / 2 + area.lt.x,
				(area.rb.y - area.lt.y) / 2 + area.lt.y
		};
		if (depth) {
			vecSubs.reserve(4);
			

			rect_t sub_mid[] = {
				{ area.lt, mid },
				{ {mid.x, area.lt.y}, {area.rb.x, mid.y}},
				{ {area.lt.x, mid.y}, {mid.x, area.rb.y}},
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
	
	vector<impl_quadtree*> get_sub_nodes_effected(postion_t pos, double radius)
	{
		vector<impl_quadtree*> vec;
		if (!depth) {
			return vec;
		}

		if (pos.x + radius < this->area.lt.x ||
			pos.y + radius < this->area.lt.y ||
			pos.x - radius > this->area.rb.x ||
			pos.y - radius > this->area.rb.y) {
			// not inside the quad node.
			return vec;
		}

		if (pos.x - radius < mid.x) {
			// it's in the left side
			if (pos.y - radius < mid.y) {
				// it's in the top side
				vec.push_back(vecSubs[0].get());
			}
			if (pos.y + radius > mid.y) {
				// it's in the bottom side
				vec.push_back(vecSubs[2].get());
			}
		}
		if (pos.x + radius > mid.x) {
			// it's in the right side
			if (pos.y - radius < mid.y) {
				// it's in the top side
				vec.push_back(vecSubs[1].get());
			}
			if (pos.y + radius > mid.y) {
				// it's in the bottom side
				vec.push_back(vecSubs[3].get());
			}
		}
		return vec;
	}

	/*
	* Returns the nodes that can fully hold the query.
	
	vector<impl_quadtree*> get_smallest_container_for_query(postion_t pos, double radius)
	{
		vector<impl_quadtree*> vec;
		if (!depth) {
			return vec;
		}
		for (auto& sub_node : this->vecSubs) {
			if (sub_node->area.lt.x <= pos.x - radius &&
				sub_node->area.lt.y <= pos.y - radius &&
				sub_node->area.rb.x >= pos.x + radius &&
				sub_node->area.rb.y >= pos.y + radius) {
				// this node fully contains the query
				vec.push_back(sub_node.get());
			}
		}
		return vec;
	}

	// Inherited via qt
	virtual void add(Entity* pEntity) override
	{
		auto do_add = [&](impl_quadtree* pTop, Entity* pEntity) {
			// only the top level records the old position.
			if (pTop->pmapOldCoords->find(pEntity) != pTop->pmapOldCoords->end()) {
				throw std::runtime_error("No double adding, entity already exists.");
			}
			pTop->pmapOldCoords->insert(pair<Entity*, postion_t>(pEntity, pEntity->position));

			vecEntities.push_back(pEntity);
		};
	
		if (depth) {
			// check which node tree it's in.	
			auto list = get_sub_nodes_effected(pEntity->position, pEntity->radius);
			if (list.size() > 1 || list.size() == 0) {
				// if in mulitple nodes or no nodes,
				// stop the nodes going down and add to current node
				do_add(pTop, pEntity);
				
				return;
			}
			for (auto& qt : list) {
				qt->add(pEntity);
			}
		}
		else {
			do_add(pTop, pEntity);
		}
	}



	virtual void remove(Entity* pEntity) override
	{
		// remove old position only from top level.
		pTop->pmapOldCoords->erase(pEntity);
		
		auto list = get_smallest_container_for_query(pEntity->position, pEntity->radius);
		if (!list.empty()) {
			for (auto& qt : list) {
				qt->remove(pEntity);
			}
		}
		else {
			// the current node has the item
			auto it = find(vecEntities.begin(), vecEntities.end(), pEntity);
			if (it != vecEntities.end()) {
				vecEntities.erase(it);
			}
			else {
				// log error!!!
				throw std::runtime_error("404  Cant remove object.");
			}
		}
		
	}

	virtual void update(Entity* pEntity) override
	{
		if (!pmapOldCoords) {
			throw runtime_error("No map?");
		}
		
		auto itPos = pmapOldCoords->find(pEntity);
		
		if (itPos == pmapOldCoords->end()) {
			throw runtime_error("Updating entity not in quad tree?");
		}

		update_inner(pEntity, itPos->second);
		pmapOldCoords.get()->at(pEntity) = pEntity->position;
	}
	void update_inner(Entity* pEntity, postion_t old_pos)
	{
		// check which node tree it's in.	
		auto list_old = get_smallest_container_for_query(old_pos, pEntity->radius);
		auto list_new = get_smallest_container_for_query(pEntity->position, pEntity->radius);
		
		vector<impl_quadtree*> vecToUpdate, vecToRemove, vecToAdd;
		// for each old in new, then update
		//          old not in new then remove
		//          new not in old then add
		//          if no new, then add to current level

		for (auto& qt : list_old) {
			if (find(list_new.begin(), list_new.end(), qt) == list_new.end()) {
				// not in, therefore remove
				vecToRemove.push_back(qt);
			}
			else {
				vecToUpdate.push_back(qt);
			}
		}
		if (list_new.empty()) {
			// if no new list, then it can't fix in a sub node so add to current node.
			list_new.push_back(this);
		}
		for (auto& qt : list_new) {
			auto it = find(list_old.begin(), list_old.end(), qt);
			if (it == list_old.end()) {
				// if not in old, then add

				// Check its not in the current node.
				// So if want to add to dpeth 3 and remove from depth 2, check it's not in d2
				if (find(vecEntities.begin(), vecEntities.end(), pEntity) != vecEntities.end()) {
					// It's in D2 wanting to add to another.
					vecToRemove.push_back(this);
				}

				vecToAdd.push_back(qt);				
			}
			else {
				// ignore if new is in old. 
			}
		}


		for (auto& qt : vecToUpdate) {
			qt->update_inner(pEntity, old_pos);
		}
		for (auto& qt : vecToRemove) {
			// Use the old position to remove.
			auto new_pos = pEntity->position;
			pEntity->position = old_pos;
			qt->remove(pEntity);
			pEntity->position = new_pos;
		}
		for (auto& qt : vecToAdd) {
			qt->add(pEntity);
		}
	}

	virtual std::vector<Entity*> get(postion_t position, double radius) override
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
			double dx = (pEntity->position.x - position.x);
			double dy = (pEntity->position.y - position.y);
			double delta_distance = sqrt(dx * dx + dy * dy);
			double delta_distance_less_radius = delta_distance - pEntity->radius;
			//double crash_dis_squared = sqrt(pEntity->radius * pEntity->radius + r2);
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

qt::qt()
{
}


class Astroid_Imp : public Astroid
{
public:
	Astroid_Imp(postion_t position, double radius, qt* pQt_);
	~Astroid_Imp() noexcept(false);
	qt* pQt;
};

Astroid_Imp::Astroid_Imp(postion_t position, double radius, qt* pQt_)
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

astroid_t Astroid_Factory::create(postion_t position, double radius, qt* pQt)
{
	astroid_t pAsteroid = shared_ptr<Astroid>(new Astroid(position, radius));
	pQt->add(pAsteroid.get());
	return pAsteroid;
}

class Player_Imp : public Player
{
public:
	Player_Imp(postion_t position, double radius, qt* pQt_);
	~Player_Imp() noexcept(false);
	qt* pQt;
};


Player_Imp::Player_Imp(postion_t position, double radius, qt* pQt_)
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

player_t Player_Factory::create(postion_t position, double radius, qt* pQt)
{
	player_t pPlayer = shared_ptr<Player>(new Player_Imp(position, radius, pQt));
	pQt->add(pPlayer.get());
	return pPlayer;
}
*/