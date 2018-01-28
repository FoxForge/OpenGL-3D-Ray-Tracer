#include "Camera.h"

Camera::Camera()
{
	InitDefaultCamera();
}

Camera::~Camera()
{
}

void Camera::InitDefaultCamera()
{
	m_position.SetVector(0.0, 6.0, 13.0);
	m_upVector.SetVector(0.0f, 1.0f, 0.0f);
	m_viewVector.SetVector(0.0f, 0.0, -1.0f);
	m_rightVector = m_viewVector.CrossProduct(m_upVector);
	m_upVector = m_rightVector.CrossProduct(m_viewVector);
	m_focalLength = 1.0;

	//Calculate viewplane centre;
	m_viewCentre = m_position + m_viewVector*m_focalLength;
}

void Camera::SetPositionAndLookAt( const Vector3& pos, const Vector3& lookat)
{
	m_position = pos;
	m_upVector.SetVector(0.0f, 1.0f, 0.0f);
	m_viewVector = lookat - m_position;
	m_viewVector.Normalise();

	m_rightVector = m_viewVector.CrossProduct(m_upVector);
	m_upVector = m_rightVector.CrossProduct(m_viewVector);
	m_focalLength = 1.0;

	//Calculate viewplane centre;
	m_viewCentre = m_position + m_viewVector*m_focalLength;
}

