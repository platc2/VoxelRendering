#ifndef VOXEL_FILEUTILITIES_H
#define VOXEL_FILEUTILITIES_H

// Stdlib
#include <string>


namespace voxel
{

/**
* @brief Opens and returns the contents of a file
*
* @param strPath  Path to the file to open
*
* @return string content of the file
*/
std::string readFile(const std::string& strPath);


/**
 * Wrapper object for image data
 */
struct ImageFile
{
	/** binary image data */
	std::uint8_t* m_pImageData;
	/** image width */
	std::uint32_t m_uiWidth;
	/** image height */
	std::uint32_t m_uiHeight;
};


/**
 * @brief Opens and returns the contents of an image
 *
 * @param strPath  Path to the image to open
 *
 * @return image wrapper object
 */
ImageFile readImage(const std::string& strPath);

}


#endif
