#pragma once
#include <memory>
#include <vector>
#include <functional>


/*
class Astroid_Factory;
class Player_Factory;

struct postion_t {
	double x, y;
};

class Entity;
class Astroid;
class Player;
class qt;
typedef std::shared_ptr<Entity> entity_t;
typedef std::shared_ptr<Astroid> astroid_t;
typedef std::shared_ptr<Player> player_t;

// Everything that exists is a entity in the quad tree
class Entity {
public:
	int id;    // unique id
	double radius;
	postion_t position;
	Entity(postion_t position, double radius);
	virtual ~Entity() noexcept(false);
protected:
	//qt* pQt;
};

class Astroid : public Entity {
public:
	//static astroid_t  create(postion_t position, double radius, qt* pQt);
	
	virtual ~Astroid() noexcept(false) = default;

protected:
	Astroid(postion_t position, double radius);
	friend class Astroid_Factory;
};

class Astroid_Factory
{
public:
	static astroid_t  create(postion_t position, double radius, qt* pQt);
};


class Player : public Entity {
public:
	//static player_t create(postion_t position, double radius, qt* pQt);
	virtual ~Player() noexcept(false) = default;
protected:
	Player(postion_t position, double radius);
	//static qt* pQt;
	static int foobar();
	friend class Player_Factory;
};

class Player_Factory {
public:
	static player_t create(postion_t position, double radius, qt* pQt);
};



class qt
{
protected:
	qt();
public:
	class node {
	public:
		
	};
	
	virtual void add(Entity* pEntity)= 0;
	virtual void remove(Entity*pEntity) = 0;
	virtual void update(Entity* pEntity) = 0;

	// Get all entities from a circle
	virtual std::vector<Entity*> get(postion_t position, double radius) = 0;
};

class factory {
public:
	static std::unique_ptr<qt> create_qt();
};
*/

