#pragma once
#include "Primitive.h"
#include "Vector3.h"
#include "Ray.h"

class Plane : public Primitive
{
	private:
		Vector3			m_normal;			//normal to the plane
		double			m_offset;			//position of the plane along the normal

	public:
						Plane();
						~Plane();

		RayHitResult	IntersectByRay(Ray& ray);

		void SetPlane(const Vector3& normal, double offset);
};

