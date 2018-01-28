#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if defined(WIN32) || defined(_WINDOWS)
#include <Windows.h>
#include <gl/GL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "perlin.h"
#include "Sphere.h"
#include "Triangle.h"

#define SHADOW_MULTI 0.2	// Defined a multiplier for shadows
#define REFLECT_MULTI 0.8	// Defined a multiplier for reflections
#define REFRACT_MULTI 0.3	// Defined a multiplier for refractions
#define STEP_MULTI 1E-3		// Defined a multiplier for small offset between rays


RayTracer::RayTracer()
{
	m_buffHeight = m_buffWidth = 0.0;
	m_renderCount = 0;
	SetTraceLevel(5);
	m_traceflag = (TraceFlags)(TRACE_AMBIENT | TRACE_DIFFUSE_AND_SPEC |
		TRACE_SHADOW | TRACE_REFLECTION | TRACE_REFRACTION);
}

RayTracer::RayTracer(int Width, int Height)
{
	m_buffWidth = Width;
	m_buffHeight = Height;
	m_renderCount = 0;
	SetTraceLevel(5);

	m_framebuffer = new Framebuffer(Width, Height);

	//default set default trace flag, i.e. no lighting, non-recursive
	m_traceflag = (TraceFlags)(TRACE_AMBIENT);
}

RayTracer::~RayTracer()
{
	delete m_framebuffer;
}

void RayTracer::DoRayTrace( Scene* pScene )
{
	Camera* cam = pScene->GetSceneCamera();
	
	Vector3 camRightVector = cam->GetRightVector();
	Vector3 camUpVector = cam->GetUpVector();
	Vector3 camViewVector = cam->GetViewVector();
	Vector3 centre = cam->GetViewCentre();
	Vector3 camPosition = cam->GetPosition();

	double sceneWidth = pScene->GetSceneWidth();
	double sceneHeight = pScene->GetSceneHeight();

	double pixelDX = sceneWidth / m_buffWidth;
	double pixelDY = sceneHeight / m_buffHeight;
	
	int total = m_buffHeight*m_buffWidth;
	int done_count = 0;
	
	Vector3 start;

	start[0] = centre[0] - ((sceneWidth * camRightVector[0])
		+ (sceneHeight * camUpVector[0])) / 2.0;
	start[1] = centre[1] - ((sceneWidth * camRightVector[1])
		+ (sceneHeight * camUpVector[1])) / 2.0;
	start[2] = centre[2] - ((sceneWidth * camRightVector[2])
		+ (sceneHeight * camUpVector[2])) / 2.0;
	
	Colour scenebg = pScene->GetBackgroundColour();

	if (m_renderCount == 0)
	{
		fprintf(stdout, "Trace start.\n");

		Colour colour;
//TinyRay on multiprocessors using OpenMP!!!
#pragma omp parallel for schedule (dynamic, 1) private(colour)
		for (int i = 0; i < m_buffHeight; i+=1) {
			for (int j = 0; j < m_buffWidth; j+=1) {

				//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
				Vector3 pixel;

				pixel[0] = start[0] + (i + 0.5) * camUpVector[0] * pixelDY
					+ (j + 0.5) * camRightVector[0] * pixelDX;
				pixel[1] = start[1] + (i + 0.5) * camUpVector[1] * pixelDY
					+ (j + 0.5) * camRightVector[1] * pixelDX;
				pixel[2] = start[2] + (i + 0.5) * camUpVector[2] * pixelDY
					+ (j + 0.5) * camRightVector[2] * pixelDX;

				/*
				* setup first generation view ray
				* In perspective projection, each view ray originates from the eye (camera) position 
				* and pierces through a pixel in the view plane
				*/
				Ray viewray;
				viewray.SetRay(camPosition,	(pixel - camPosition).Normalise());
				
				double u = (double)j / (double)m_buffWidth;
				double v = (double)i / (double)m_buffHeight;

				scenebg = pScene->GetBackgroundColour();

				//trace the scene using the view ray
				//default colour is the background colour, unless something is hit along the way
				colour = this->TraceScene(pScene, viewray, scenebg, m_traceLevel);

				/*
				* Draw the pixel as a coloured rectangle
				*/
				m_framebuffer->WriteRGBToFramebuffer(colour, j, i);
			}
		}

		fprintf(stdout, "Done!!!\n");
		m_renderCount++;
	}
}

Colour RayTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray)
{
	if (tracelevel <= 0) // No more recursion to be done so return the incolour
		return incolour;

	RayHitResult result;
	Colour outcolour = incolour; // The output colour based on the ray-primitive intersection
	std::vector<Light*> *light_list = pScene->GetLightList();
	Vector3 cameraPosition = pScene->GetSceneCamera()->GetPosition();

	// Intersect the ray with the scene
	result = pScene->IntersectByRay(ray, shadowray); // Pass in the shadowray so we can determine to correct hit result data from the lights

	if (result.data) // The ray has hit something with the correct shadow data
	{
		if (shadowray)							// If the ray is a shadow then we can return the same colour (Cast Shadow already determined)
			return (outcolour * SHADOW_MULTI);	// Return the shadow colour multiplied by the shadow percentage
		
		outcolour = CalculateLighting(light_list, &cameraPosition, &result); // Calculate the lighting using the camera position as the start point

		if (m_traceflag & TRACE_REFLECTION)
		{
			if (((Primitive*)result.data)->GetMaterial()->IsReflective())													// Check the material, is it reflective?
			{
				Vector3 reflection = ray.GetRay().Reflect(result.normal);													// Get the reflection ray using the normal
				ray.SetRay(result.point + reflection * STEP_MULTI, reflection);												// Set the ray with a slight offest in the calculated direction
				outcolour = outcolour * TraceScene(pScene, ray, outcolour, (tracelevel - 1), shadowray) * REFLECT_MULTI;	// Reccurisve call the method multiplying the colour to the current and lower trace level
			}
		}

		if (m_traceflag & TRACE_REFRACTION)
		{
			float refractiveIndex = ((Primitive*)result.data)->GetMaterial()->GetRefractiveIndex(); // Get the refractive index on the material of the primitive
			if (refractiveIndex > 1)																// If the refractive index is greater than 1 then the light can be refracted
			{
				float coefficient = ray.GetRay().DotProduct(result.normal);													// Obtain the coefficient from the angle of incidence
				if (coefficient > 1) coefficient = 1.0f;																	// Clamp the coefficient
				if (coefficient < 0) coefficient = 0.0f;																	// Clamp the coefficient
				coefficient /= refractiveIndex;																				// Make the coefficient respective of index
				Vector3 refraction = ray.GetRay().Refract(result.normal, coefficient);										// Create the refraction vector
				ray.SetRay(result.point + refraction * STEP_MULTI, refraction);												// Set the ray with a slight offest in the calculated direction
				outcolour = outcolour + TraceScene(pScene, ray, outcolour, (tracelevel - 1), shadowray) * REFRACT_MULTI;	// Reccursive call the method adding the colour to the current and lower trace level
			}
		}

		if (m_traceflag & TRACE_SHADOW)
		{	
			for (std::vector<Light*>::iterator l = light_list->begin(); l != light_list->end(); l++) // Loop through all the lights in the list
			{
				Vector3 direction = ((*l)->GetLightPosition() - result.point).Normalise();	// Get the direction of where the ray should point
				ray.SetRay(result.point + direction * STEP_MULTI, direction);				// Set the ray with a slight offest in the calculated direction
				outcolour = TraceScene(pScene, ray, outcolour, (tracelevel - 1), true);		// Call the method with the new ray which is a shadow and lower the tracel level
			}
		}
	}
		
	return outcolour;
}

Colour RayTracer::CalculateLighting(std::vector<Light*>* lights, Vector3* campos, RayHitResult* hitresult)
{
	Colour outcolour;
	Primitive* prim = (Primitive*)hitresult->data;
	Material* mat = prim->GetMaterial();
	outcolour = mat->GetAmbientColour();
	
	// Generate the grid pattern on the plane
	if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
	{
		int dx = hitresult->point[0]/2.0;
		int dy = hitresult->point[1]/2.0;
		int dz = hitresult->point[2]/2.0;

		if (dx % 2 || dy % 2 || dz % 2)
		{
			return Vector3(0.1, 0.1, 0.1);
		}
		else
		{
			outcolour = mat->GetDiffuseColour();
		} 
	}
	
	if (m_traceflag & TRACE_DIFFUSE_AND_SPEC)
	{
		// Iterate through the lights
		for (std::vector<Light*>::iterator l = lights->begin(); l != lights->end(); l++)
		{
			Vector3 cDir = (*campos - hitresult->point).Normalise();					// Camera Direction
			Vector3 lDir = ((*l)->GetLightPosition() - hitresult->point).Normalise();	// Light Direction			
			Vector3 iDir = (hitresult->point - (*l)->GetLightPosition()).Normalise();	// Inverted Light Direction
			Vector3 rDir = iDir.Reflect(hitresult->normal).Normalise();				// Reflect Direction

			Colour dColour, sColour;								// Variables for the diffuse and specular Colours
			float dAngle = (lDir.DotProduct(hitresult->normal));	// Obtain dot product for the diffuse factor
			float sAngle = (cDir.DotProduct(rDir));					// Obtain dot product for specular factor

			/*
			// Remove * comments: Blinn Phong-Implementation with half angle (Feel free to test to see) 			
			Vector3 nDir = (cDir + lDir).Normalise();		// Normal Direction from light to camera
			sAngle = (nDir.DotProduct(hitresult->normal));	// Obtain dot product for the specular factor (Half angle)
			*/

			// Clamp the values between 0 and 1
			if (dAngle > 1) dAngle = 1.0f;
			if (sAngle > 1) sAngle = 1.0f;
			if (dAngle < 0) dAngle = 0.0f;
			if (sAngle < 0) sAngle = 0.0f;

			sAngle = pow(sAngle, mat->GetSpecPower());									// Calculate the final angle using the "hardness" of the specular reflection
			dColour = mat->GetDiffuseColour() * ((*l)->GetLightColour() * dAngle);		// Multiply the base material diffuse colour by the current light and new diffuse factor
			sColour = mat->GetSpecularColour() * ((*l)->GetLightColour() * sAngle);		// Multiply the base material specular colour by the current light and new specular factor 
			outcolour = outcolour + sColour + dColour;									// Combine the diffuse and specular colours with the current colour
		}
	}

	return outcolour;
}

