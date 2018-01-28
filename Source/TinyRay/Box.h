#pragma	once
#include "Primitive.h"
#include "Vector3.h"
#include "Triangle.h"

//Class represents an axis-aligned box

class Box : public Primitive
{
	private:
		Triangle m_triangles[12]; //12 triangles forming the 6 faces of the box

	public:
		Box();
		Box(Vector3 position, double width, double height, double depth);
		~Box();

		void SetBox(Vector3 position, double width, double height, double depth);

		RayHitResult IntersectByRay(Ray& ray);

};

