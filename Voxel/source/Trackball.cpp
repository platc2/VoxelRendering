#include "Trackball.h"

using namespace voxel;


/**
* Creates a new Trackball instance given a radius.
*
* @param fRadius  Radius, behaviour depends on screen-space coordinate-range
*/
Trackball::Trackball(const float fRadius) : k_fRadiusSq{fRadius * fRadius}, m_xAnchorPosition{}
{
}


/**
* To be called when a drag-movement shall be initiated,
* usually called when the user presses the left mousebutton.
*
* @param xMouseCoordinates  screen-space coordinates of the mouse click
*/
void Trackball::beginDrag(const glm::vec2& xMouseCoordinates)
{
	m_xAnchorPosition = mapped(xMouseCoordinates);
}


/**
* To be called when a drag-event occured, e.g. when the mouse was dragged,
* i.e. the mouse moved and a drag event is on.
*
* @param xMouseCoordinates  screen-space coordinates of the mouse click
* @return rotation matrix given the current drag-event
*/
glm::mat4 Trackball::drag(const glm::vec2& xMouseCoordinates)
{
	const glm::vec3 xCurrentPosition = mapped(xMouseCoordinates);

	const glm::vec3 xRotationAxis = glm::normalize(glm::cross(m_xAnchorPosition, xCurrentPosition));
	const float fRotationAngle = glm::acos(glm::dot(xCurrentPosition, m_xAnchorPosition) / (glm::length(xCurrentPosition) * glm::length(m_xAnchorPosition))) * 1.0f;

	// Rodriguez
	const float fSin = glm::sin(fRotationAngle);
	const float fCos = glm::cos(fRotationAngle);
	const glm::mat3 xBase =
	{
		0.0f, xRotationAxis.z, -xRotationAxis.y,
		-xRotationAxis.z, 0.0f, xRotationAxis.x,
		xRotationAxis.y, -xRotationAxis.x, 0.0f
	};
	const glm::mat3 xVector =
	{
		xRotationAxis.x * xRotationAxis.x, xRotationAxis.x * xRotationAxis.y, xRotationAxis.x * xRotationAxis.z,
		xRotationAxis.y * xRotationAxis.x, xRotationAxis.y * xRotationAxis.y, xRotationAxis.y * xRotationAxis.z,
		xRotationAxis.z * xRotationAxis.x, xRotationAxis.z * xRotationAxis.y, xRotationAxis.z * xRotationAxis.z
	};
	const glm::mat3 xRotation = fCos * glm::mat3(1.0f) + ((1.0f - fCos) * xVector) + fSin * xBase;
	return glm::mat4(xRotation);
}


/**
 * Projects a screen-space coordinate onto the trackball
 *
 * @param xCoordinate  screen-space coordinate
 * @return 3D vector, projected onto the trackball
 */
glm::vec3 Trackball::mapped(const glm::vec2& xCoordinate)
{
	return glm::vec3(xCoordinate, project(xCoordinate));
}


/**
 * Projects a screen-space coordinate onto the trackball.
 *
 * @param xCoordinate  screen-space coordinate
 * @return z coordinate of projected vector
 */
float Trackball::project(const glm::vec2& xCoordinate)
{
	// Squared distance to the center of the ball
	const float fDistanceSq = glm::dot(xCoordinate, xCoordinate);
	if(fDistanceSq > k_fRadiusSq / 2.0f)
	{
		// Outside of the trackball, use hypersheet instead
		return (k_fRadiusSq / 2.0f) / glm::sqrt(fDistanceSq);
	}
	else
	{
		return glm::sqrt(k_fRadiusSq - fDistanceSq);
	}
}
