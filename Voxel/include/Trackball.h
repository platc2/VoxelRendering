#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <glm/glm.hpp>


namespace voxel
{


/**
* Component for supporting trackball-like orientation in graphics applications.
*/
class Trackball final
{
private:
	/** Squared radius, behaviour depends on screen-space coordinate-range passed in the drag-functions */
	const float k_fRadiusSq;

	/** Position on the trackball when the last drag-movement was initiated */
	glm::vec3 m_xAnchorPosition;

public:
	/**
	* Creates a new Trackball instance given a radius.
	*
	* @param fRadius  Radius, behaviour depends on screen-space coordinate-range
	*/
	Trackball(const float fRadius = 1.0f);

	/**
	* To be called when a drag-movement shall be initiated,
	* usually called when the user presses the left mousebutton.
	*
	* @param xMouseCoordinates  screen-space coordinates of the mouse click
	*/
	void beginDrag(const glm::vec2& xMouseCoordinates);

	/**
	* To be called when a drag-event occured, e.g. when the mouse was dragged,
	* i.e. the mouse moved and a drag event is on.
	*
	* @param xMouseCoordinates  screen-space coordinates of the mouse click
	* @return rotation matrix given the current drag-event
	*/
	glm::mat4 drag(const glm::vec2& xMouseCoordinates);

private:
	/**
	 * Projects a screen-space coordinate onto the trackball
	 *
	 * @param xCoordinate  screen-space coordinate
	 * @return 3D vector, projected onto the trackball
	 */
	glm::vec3 mapped(const glm::vec2& xCoordinate);

	/**
	* Projects a screen-space coordinate onto the trackball.
	*
	* @param xCoordinate  screen-space coordinate
	* @return z coordinate of projected vector
	*/
	float project(const glm::vec2& xCoordinate);
};


}


#endif
