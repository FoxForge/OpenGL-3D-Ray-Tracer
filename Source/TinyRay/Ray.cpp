#include "Ray.h"

RayHitResult Ray::s_defaultHitResult;

Ray::Ray()
{
	s_defaultHitResult.data = nullptr;
	s_defaultHitResult.t = FARFAR_AWAY;
}


Ray::~Ray()
{
}
