#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>


struct Plane
{
	XMVECTOR normal; /* the normal to the plane */
	float d; /* the 'd' constant in the equation for this plane */
};

class Maths
{
private:

public:
	Maths();
	~Maths();
	float Dot(XMVECTOR* Vector1, XMVECTOR* Vector2);
	XMVECTOR Cross(XMVECTOR* Vector1, XMVECTOR* Vector2);
	XMVECTOR Normal(XMVECTOR* Point1, XMVECTOR* Point2, XMVECTOR* Point3);
	Plane CreatePlane(XMVECTOR* Point1, XMVECTOR* Point2, XMVECTOR* Point3);
	float PlaneEquation(Plane* plane, XMVECTOR* point);
	XMVECTOR PlaneIntersection(Plane* plane, XMVECTOR* Point1, XMVECTOR* Point2);
	bool InTriangle(XMVECTOR* Triangle1, XMVECTOR* Triangle2, XMVECTOR* Triangle3, XMVECTOR* Point);

};

