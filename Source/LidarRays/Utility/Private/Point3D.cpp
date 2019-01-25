#include "Point3D.h"

Point3D::Point3D() : x(0), y(0), z(0) 
{}

Point3D::Point3D(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}