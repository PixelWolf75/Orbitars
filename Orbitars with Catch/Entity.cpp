#include "Entity.h"
#include <algorithm>
#include <cmath>

using namespace Gdiplus;

void Entity::Render_Entity(HWND hWnd, Client_ScreenINFO::map_o_images_t*, Window_Properties wpWinProp)
{
}

void Entity::Update_Entity(HWND hWnd, POINT ptCursorPos)
{
}

void Entity::Input_Entity(std::vector<eKeyboard> eKey_press)
{
}

//Checks to see if the hitboxes of the entities collide
//This is a quick search
bool Entity::General_Entity_Collision(Entity* pColliding_entity)
{
	float player_x = (fPos.fx + rcHitBox.right / 2);
	float player_y = (fPos.fy + rcHitBox.bottom / 2);
	float ast_x = (pColliding_entity->fPos.fx + pColliding_entity->rcHitBox.right / 2);
	float ast_y = (pColliding_entity->fPos.fy + pColliding_entity->rcHitBox.bottom / 2);

	float fDis_to_Entity = (float)sqrt(pow(player_x - ast_x, 2) + pow(player_y - ast_y, 2));
	if (fDis_to_Entity <= (fRadius + rcHitBox.right / 2) + (pColliding_entity->fRadius + pColliding_entity->rcHitBox.right))
	{
		return true;
	}
	return false;
}

bool Entity::Advance_Entity_Collision(Entity* pColliding_entity)
{
	return false;
}


// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool onSegment(point_t p, point_t q, point_t r)
{
	return (q.fx <= fmax(p.fx, r.fx) && q.fx >= fmin(p.fx, r.fx) &&
		q.fy <= fmax(p.fy, r.fy) && q.fy >= fmin(p.fy, r.fy));
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(point_t p, point_t q, point_t r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
	// for details of below formula. 
	float val = (q.fy - p.fy) * (r.fx - q.fx) -
		(q.fx - p.fx) * (r.fy - q.fy);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool Entity::doIntersect(const point_t& p1, const point_t& q1, const point_t& p2, const point_t& q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

void Entity::Wrap_Entity(RECT rcClientArea)
{
	// Length of 1 side of the square is the smallest dimention
	int iLengthOfWidth = rcClientArea.right - rcClientArea.left;
	int iLengthOfHeight = rcClientArea.bottom - rcClientArea.top;
	float fLenghtOfSquare =(float) min(iLengthOfWidth, iLengthOfHeight);

	float fDistanceToLeft = (iLengthOfWidth) / (fLenghtOfSquare) * 0.5f;
	float fDistanceToTop = (iLengthOfHeight) / (fLenghtOfSquare) * 0.5f;

	// Absolute coordinates of the edges
	RectFloat rcfScreenEdges = {
		0.5f - fDistanceToLeft, 0.5f - fDistanceToTop,
		0.5f + fDistanceToLeft, 0.5f + fDistanceToTop
	};

	// check if touching the edges
	if (fPos.fx < rcfScreenEdges.left - fRadius) {
		fPos.fx += (fDistanceToLeft * 2 + fRadius);
	}
	if (fPos.fy < rcfScreenEdges.top - fRadius) {
		fPos.fy += (fDistanceToTop * 2 + fRadius);
	}
	if (fPos.fx > rcfScreenEdges.right + fRadius) {
		fPos.fx -= (fDistanceToLeft * 2 + fRadius);
	}
	if (fPos.fy > rcfScreenEdges.bottom + fRadius) {
		fPos.fy -= (fDistanceToTop * 2 + fRadius);
	}
}

//Wraps the client around a constant resolution,
//If it reaches edge of map (not screen) it wraps to other side
void Entity::Wrap_Entity()
{
	if (fPos.fx > 1.00f + fRadius)
	{
		fPos.fx = 0.00f - fRadius;
	}

	if (fPos.fy > 1.00f + fRadius)
	{
		fPos.fy = 0.00f - fRadius;
	}

	if (fPos.fx < 0.00f - fRadius)
	{
		fPos.fx = 1.00f + fRadius;
	}

	if (fPos.fx < 0.00f - fRadius)
	{
		fPos.fx = 1.00f + fRadius;
	}


}

void Entity::UpdateHitBox(float fDisMin)
{
}



