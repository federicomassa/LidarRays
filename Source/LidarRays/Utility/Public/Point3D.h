#pragma once

#include <array>

class Point3D {

public:
	float x;
	float y;
	float z;

	Point3D();
	Point3D(float, float, float);

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(x);
		ar(y);
		ar(z);
	}
};
