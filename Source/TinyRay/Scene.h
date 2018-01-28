#pragma once

#include "Camera.h"
#include "Primitive.h"
#include "Material.h"
#include "Light.h"
#include <vector>

//Class representing a scene
class Scene
{
	private:
		Camera							m_activeCamera;		//the active camera in the scene
		
		std::vector<Primitive*>			m_sceneObjects;		//A list of primitives (objects) in the scene
		std::vector<Material*>			m_objectMaterials;	//A list of materials used in the scene
		std::vector<Light*>				m_lights;			//A list of light source in the scene

		Colour							m_background;		//default background colour of the scene
		double							m_sceneWidth;		//metric width of the scene in view space
		double							m_sceneHeight;		//metric height of the scene in view space

	public:
		Scene();
		~Scene();

		void InitDefaultScene();

		inline void SetSceneWidth(double width)
		{
			m_sceneWidth = width;
		}

		inline Camera* GetSceneCamera()
		{
			return &m_activeCamera;
		}

		inline double GetSceneWidth() 
		{
			return m_sceneWidth;
		}

		inline double GetSceneHeight() 
		{
			return m_sceneHeight;
		}

		inline Colour& GetBackgroundColour()
		{
			return m_background;
		}	

		RayHitResult IntersectByRay(Ray& ray, bool shadowray); // Added the shadowray parameter so the hit result can be correctly determined with shadows

		inline std::vector<Light*>* GetLightList()
		{
			return &m_lights;
		}
		
		void		CleanupScene();
		
};

