#include "Maths.h"



Maths::Maths()
{
}


Maths::~Maths()
{
}

float Maths::Dot(XMVECTOR * Vector1, XMVECTOR * Vector2)
{
	return (Vector1->x * Vector2->x) + (Vector1->y * Vector2->y) + (Vector1->z * Vector2->z);
}

XMVECTOR Maths::Cross(XMVECTOR * Vector1, XMVECTOR * Vector2)
{
	float x = (Vector1->y * Vector2->z) - (Vector1->z * Vector2->y);
	float y = (Vector1->z * Vector2->x) - (Vector1->x * Vector2->z);
	float z = (Vector1->x * Vector2->y) - (Vector1->y * Vector2->x);

	return XMVectorSet(x, y, z, 0);
}

XMVECTOR Maths::Normal(XMVECTOR * Point1, XMVECTOR * Point2, XMVECTOR * Point3)
{
	XMVECTOR vector1 = XMVectorSet(Point2->x - Point1->x, Point2->y - Point1->y, Point2->z - Point1->z, 1);
	XMVECTOR vector2 = XMVectorSet(Point3->x - Point1->x, Point3->y - Point1->y, Point3->z - Point1->z, 1);
	XMVECTOR cross = Cross(&vector1, &vector2);
	float magnitude = sqrt(pow(cross.x, 2) + pow(cross.y, 2) + pow(cross.z, 2));
	
	return XMVectorSet(cross.x * magnitude, cross.y * magnitude, cross.z * magnitude, 1);
}

Plane Maths::CreatePlane(XMVECTOR * Point1, XMVECTOR * Point2, XMVECTOR * Point3)
{
	Plane plane;
	plane.normal = Normal(Point1, Point2, Point3);
	plane.d = -Dot(&plane.normal, Point1);
	return plane;
}

float Maths::PlaneEquation(Plane * plane, XMVECTOR* point)
{
	return Dot(&plane->normal, point) + plane->d;
}

XMVECTOR Maths::PlaneIntersection(Plane * plane, XMVECTOR * Point1, XMVECTOR * Point2)
{
	XMVECTOR ray = XMVectorSet(Point2->x - Point1->x, Point2->y - Point1->y, Point2->z - Point1->z, 1);
	float t = (-plane->d - Dot(&plane->normal, Point1)) / Dot(&plane->normal, &ray);
	if (t > 0 && t < 1)
	{
		XMVECTOR intersect = XMVectorSet(Point1->x + (ray.x * t), Point1->y + (ray.y * t), Point1->z + (ray.z * t), 0);
		return intersect;
	}
	else
	{
		return XMVectorSet(9999999, 9999999, 9999999, 0);
	}
}

bool Maths::InTriangle(XMVECTOR * Triangle1, XMVECTOR * Triangle2, XMVECTOR * Triangle3, XMVECTOR * Point)
{
	float A = Dot(Triangle1, Point);
	float B = Dot(Triangle2, Point);
	float C = Dot(Triangle3, Point);

	return ((A <= 0 && B <= 0 && C <= 0) || (A >= 0 && B >= 0 && C >= 0));
}
