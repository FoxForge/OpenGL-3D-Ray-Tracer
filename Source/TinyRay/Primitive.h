#pragma once

#include "Ray.h"

class Material;


//An abstract class representing a basic primitive in TinyRay
class Primitive
{
	private:
		Material				*m_pMaterial;		//pointer to the material associated to the primitive
		
	public:
		//enum for primitive types
		enum PRIMTYPE
		{
			PRIMTYPE_Plane = 0,	//plane
			PRIMTYPE_Sphere, //sphere
			PRIMTYPE_Triangle, //generic triangle
			PRIMTYPE_Box, //box
			PRIMTYPE_TRIMESH // Added for Texture Mapping onto polygons
		};

		PRIMTYPE				m_primtype; //primitive type

								Primitive(){ m_pMaterial = nullptr; }
		virtual					~Primitive(){ ; }


		virtual RayHitResult	IntersectByRay(Ray& ray) = 0;  //An interface for computing intersection between a ray and this primitve

		inline void				SetMaterial(Material* pMat)
		{
			m_pMaterial = pMat;
		}

		inline Material*		GetMaterial()
		{
			return m_pMaterial;
		}
};
