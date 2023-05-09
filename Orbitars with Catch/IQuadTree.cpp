#include "IQuadTree.h"
#include <iterator>
using namespace std;


class QuadTree : public IQuadTree
{
public:
	QuadTree(int iCapacity_, search_area_t pbBoundary_);
	~QuadTree();

	unsigned int iCapacity; //The maximum number of nodes that can be in the quadtree
	search_area_t pbBoundary; //The area of the quadtree
	bool bIsSubdivided; //If the node has been divided

	std::vector<node> vecNodes; //Container of nodes within the tree
	std::vector<QuadTree> vecQuads; //Container of the 4 smaller quadtrees

	//Creates 4 quadtrees with smaller boundarys
	void sub_divide();

	// Inserts a node into the Quadtree
	virtual void insert_node(node) override;

	// Searches the node based on a point
	virtual node Search_node(point_t) override;

	//Searches for all nodes within the area or radius provided
	virtual std::vector<node> search(search_area_t) override;
	virtual std::vector<node> search(search_radius_t) override;

	//Resets the Quad tree and erases all nodes and connected trees
	virtual void Reset_QuadTree(search_area_t) override;

};

QuadTree::QuadTree(int iCapacity_, search_area_t pbBoundary_ = search_area_t{ 0, 0, 0, 0 })
	: iCapacity(iCapacity_)
	, pbBoundary(pbBoundary_)
	, bIsSubdivided(false)
{
	vecNodes.reserve(iCapacity);
}

QuadTree::~QuadTree()
{
}

void QuadTree::sub_divide()
{
	bIsSubdivided = true;

	search_area_t pbNW = { pbBoundary.fx, pbBoundary.fy, (pbBoundary.fWidth) / 2, (pbBoundary.fHeight) / 2 };
	search_area_t pbNE = { (pbBoundary.fx + pbBoundary.fWidth) / 2, pbBoundary.fy, (pbBoundary.fWidth) / 2, (pbBoundary.fHeight) / 2 };
	search_area_t pbSW = { pbBoundary.fx, (pbBoundary.fy + pbBoundary.fHeight) / 2, (pbBoundary.fWidth) / 2, (pbBoundary.fHeight) / 2 };
	search_area_t pbSE = { (pbBoundary.fx + pbBoundary.fWidth) / 2, (pbBoundary.fy + pbBoundary.fHeight) / 2, (pbBoundary.fWidth) / 2, (pbBoundary.fHeight) / 2 };

	vecQuads.push_back(QuadTree(iCapacity, pbNW));
	vecQuads.push_back(QuadTree(iCapacity, pbNE));
	vecQuads.push_back(QuadTree(iCapacity, pbSW));
	vecQuads.push_back(QuadTree(iCapacity, pbSE));
}

void QuadTree::insert_node(node newNode)
{
	if (!pbBoundary.contains_point(newNode.ptPoint))
	{
		return;
	}

	if (vecNodes.size() < iCapacity)
	{
		vecNodes.push_back(newNode);
		return;
	}
	else {
		if (!bIsSubdivided)
		{
			sub_divide();
		}

		for (auto it = vecQuads.begin(); it != vecQuads.end(); it++)
		{
			it->insert_node(newNode);
		}
	}
}

node QuadTree::Search_node(point_t ptNode)
{
	if (!pbBoundary.intersects_range(search_area_t({ ptNode.fx, ptNode.fy, 0, 0 })))
	{
		return node();
	}

	for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
	{
		if (it->ptPoint.fx == ptNode.fy && it->ptPoint.fy == ptNode.fy)
		{
			return *it;
		}
	}

	if (bIsSubdivided)
	{
		for (auto it = vecQuads.begin(); it != vecQuads.end(); it++)
		{
			return it->Search_node(ptNode);
		}
	}

	return node();

}

std::vector<node> QuadTree::search(search_area_t pbRange)
{
	std::vector<node> vecFoundNodes;

	if (!pbBoundary.intersects_range(pbRange))
	{
		return vector<node>() = {};
	}

	for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
	{
		if (pbRange.contains_point(it->ptPoint) && !it->bHasUpdated)
		{
			vecFoundNodes.push_back(*it);
		}
	}

	if (bIsSubdivided)
	{
		for (auto it = vecQuads.begin(); it != vecQuads.end(); it++)
		{
			vector<node> vecQuadFound = it->search(pbRange);
			if (vecQuadFound.size() <= 0)
			{
				continue;
			}
			copy(vecQuadFound.begin(), vecQuadFound.end(), back_inserter(vecFoundNodes));
		}
	}

	return vecFoundNodes;

}

std::vector<node> QuadTree::search(search_radius_t pbRadius)
{
	std::vector<node> vecFoundNodes;
	if (!pbRadius.intersects_range(pbBoundary))
	{
		return std::vector<node>();
	}

	for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
	{
		if (pbRadius.contains_point(it->ptPoint) && !it->bHasUpdated)
		{
			vecFoundNodes.push_back(*it);
		}
	}

	if (bIsSubdivided)
	{
		for (auto it = vecQuads.begin(); it != vecQuads.end(); it++)
		{
			std::vector<node>vecQuadNodes = it->search(pbRadius);
			copy(vecQuadNodes.begin(), vecQuadNodes.end(), std::back_inserter(vecFoundNodes));
		}
	}

	return vecFoundNodes;
}

void QuadTree::Reset_QuadTree(search_area_t pbNewBaseBoundary)
{
	/*if (vecNodes.size() != 0)
	{
		if (vecNodes.size() > 0)
		{
			vecNodes.erase(vecNodes.begin());
		}
		if (vecQuads.size() > 0)
		{
			vecQuads.erase(vecQuads.begin());
		}
	}*/
	vecNodes.clear();
	vecQuads.clear();

	bIsSubdivided = false;
	pbBoundary = pbNewBaseBoundary;
}

std::unique_ptr<IQuadTree> QuadTree_Factory::Create(int iCapacity)
{
	return std::make_unique<QuadTree>(iCapacity);
}

bool search_area_t::contains_point(point_t ptNodePoint)
{
	return(fx <= ptNodePoint.fx && ptNodePoint.fx <= (fx + fWidth) &&
		fy <= ptNodePoint.fy && ptNodePoint.fy <= (fy + fHeight));
}
bool search_area_t::intersects_range(search_area_t pbRange)
{
	return !((pbRange.fx) > (fx + fWidth) ||
		(pbRange.fy > (fy + fHeight)) ||
		((pbRange.fx + pbRange.fWidth) < fx) ||
		((pbRange.fy + pbRange.fHeight) < fy));
};

bool search_radius_t::contains_point(point_t ptNodePoint)
{
	return(((fx - ptNodePoint.fx) * (fx - ptNodePoint.fx) +
		(fy - ptNodePoint.fy) * (fy - ptNodePoint.fy))
		<= (fRadius * fRadius));
}

bool search_radius_t::intersects_range(search_area_t pbRange)
{
	float fDisX = fabsf(fx - pbRange.fx);
	float fDisY = abs(fy - pbRange.fy);

	if (fDisX > (pbRange.fWidth / 2 + fRadius)) { return false; }
	if (fDisY > (pbRange.fHeight / 2 + fRadius)) { return false; }

	if (fDisX <= (pbRange.fWidth / 2)) { return true; }
	if (fDisY <= (pbRange.fHeight / 2)) { return true; }

	float cornerDistance_sq = float(pow((fDisX - pbRange.fWidth / 2) ,2) +
		pow((fDisY - pbRange.fHeight / 2) ,2));

	return (cornerDistance_sq <= pow(fRadius, 2));
}
bool search_radius_t::intersects_circle(search_radius_t pbRange)
{
	float fDistanceX = fabsf(fx - pbRange.fx);
	float fDistanceY = fabsf(fy - pbRange.fy);

	float fEdge = (fDistanceX * fDistanceX) + (fDistanceY * fDistanceY);

	return fEdge <= fRadius + pbRange.fRadius;
}
