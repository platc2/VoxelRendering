#include "FileUtilities.h"

#ifdef _MSC_VER
	#include <ciso646>
#endif

// Stdlib
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


/**
* @brief Opens and returns the contents of a file
*
* @param strPath  Path to the file to open
*
* @return string content of the file
*/
std::string voxel::readFile(const std::string& strPath)
{
	std::ifstream xFileStream(strPath);
	if(not xFileStream.good())
	{
		throw std::runtime_error("Failed to load file " + strPath);
	}
	xFileStream.seekg(0, std::ios::end);
	std::string strContent;
	strContent.reserve(xFileStream.tellg());
	xFileStream.seekg(0);
	strContent.assign(std::istreambuf_iterator<char>(xFileStream), std::istreambuf_iterator<char>());
	return strContent;
}


/**
* @brief Opens and returns the contents of an image
*
* @param strPath  Path to the image to open
*
* @return image wrapper object
*/
voxel::ImageFile voxel::readImage(const std::string& strPath)
{
	std::int32_t iWidth = 0;
	std::int32_t iHeight = 0;
	std::int32_t iNumChannels = 0;
	stbi_uc* const pImageData = stbi_load(strPath.c_str(), &iWidth, &iHeight, &iNumChannels, STBI_rgb_alpha);
	if(pImageData == nullptr)
	{
		throw new std::runtime_error("Failed to open image");
	}
	else
	{
		return { static_cast<std::uint8_t*>(pImageData), static_cast<std::uint32_t>(iWidth), static_cast<std::uint32_t>(iHeight) };
	}
}
