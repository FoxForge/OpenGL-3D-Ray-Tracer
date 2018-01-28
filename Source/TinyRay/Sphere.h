#pragma once
#include "Primitive.h"
#include "Vector3.h"
#include "Ray.h"

class Sphere : public Primitive
{
	private:
		Vector3				m_centre;			//location of the centre of the sphere
		double				m_radius;			//the radius of the sphere

	public:
		Sphere();
		Sphere(double x, double y, double z, double r);
		~Sphere();

		inline Vector3&		GetCentre()
		{
			return m_centre;
		}

		inline double		GetRadius()
		{
			return m_radius;
		}

		RayHitResult		IntersectByRay(Ray& ray);
};

