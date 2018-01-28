#include <stdlib.h>
#include "Material.h"
#include "ImageIO.h"

Material::Material()
{
	SetDefaultMaterial();
}


Material::~Material()
{
	if (mDiffuse_texture) delete mDiffuse_texture;
}

void Material::SetDefaultMaterial()
{
	//default material
	//black ambient colour
	//white diffuse and specular
	//spec power is 10

	SetAmbientColour(0.0, 0.0, 0.0);
	SetDiffuseColour(1.0, 1.0, 1.0);
	SetSpecularColour(1.0, 1.0, 1.0);
	SetSpecPower(10.0);
	mDiffuse_texture = NULL;
	mNormal_texture = NULL;
	mCastShadow = true;
	mReflective = false;	// Set the added default reflection to false (the material cannot reflect light until stated otherwise)
	mRefractiveIndex = 1;	// Set the added default refractive index to the same as light (the material cannot refract light until this value is set > 1)
}

void Material::SetAmbientColour(float r, float g, float b)
{
	mAmbient.SetVector(r, g, b);
}

void Material::SetDiffuseColour(float r, float g, float b)
{
	mDiffuse.SetVector(r, g, b);
}

void Material::SetSpecularColour(float r, float g, float b)
{
	mSpecular.SetVector(r, g, b);
}

void Material::SetSpecPower(double spow)
{
	mSpecpower = spow;
}
