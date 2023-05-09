#pragma once
#include <vector>
#include <math.h>
#include <algorithm>
#include "Universal_Structures.h"
#include <memory>

//A 4 sided area to search nodes within
struct search_area_t {
	float fx; //The x position of the area
	float fy; //The y position of the area
	float fWidth; //The distance from the x position
	float fHeight; //The distance from the y position

	//Checks if the area contains the point
	bool contains_point(point_t ptNodePoint);
	//Checks if the areas intersects with another
	bool intersects_range(search_area_t pbRange);
};

//A Circle to search nodes within
struct search_radius_t {
	float fx;  //The x position of the area
	float fy;  //The y position of the area
	float fRadius; //The radius of the circle area

	//Checks if the area contains the point
	bool contains_point(point_t ptNodePoint);
	//Checks if the area intersects with a rectangular area
	bool intersects_range(search_area_t pbRange);
	//Checks if the area intersects with another cicular area
	bool intersects_circle(search_radius_t pbRange);

};

//Element of the quadtree
struct node {
	void* pData; //Raw pointer to the data in memory
	point_t ptPoint; //Position of the element in the tree
	bool bHasUpdated = false; //If the element has been updated
};

class IQuadTree
{
public:
	virtual ~IQuadTree() {};

	// Inserts a node into the Quadtree
	virtual void insert_node(node) = 0;
	// Searches the node based on a point
	virtual node Search_node(point_t) = 0;

	//Searches for all nodes within the area or radius provided
	virtual std::vector<node> search(search_area_t) = 0;
	virtual std::vector<node> search(search_radius_t) = 0;

	//Resets the Quad tree and erases all nodes and connected trees
	virtual void Reset_QuadTree(search_area_t) = 0;
};

class QuadTree_Factory
{
public:
	//Creates the Quadtree, with a set capacity for an arbiturary number of nodes
	static std::unique_ptr<IQuadTree> Create(int);
};