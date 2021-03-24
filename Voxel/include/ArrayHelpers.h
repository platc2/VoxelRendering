#ifndef VOXEL_ARRAYHELPERS_H
#define VOXEL_ARRAYHELPERS_H

#include <array>


namespace voxel
{

template<typename ...TValueType>
/**
 * @brief Constructs and std::array from the passed elements.
 *
 * @param rgValues  variable amount of values
 * @return std::array containing passed values
 */
constexpr auto make_array(const TValueType& ...rgValues) -> std::array<typename std::decay<typename std::common_type<TValueType...>::type>::type, sizeof...(TValueType)>
{
	using TArrayType = typename std::decay<typename std::common_type<TValueType...>::type>::type;
	return std::array<TArrayType, sizeof...(TValueType)>{ static_cast<TArrayType>(rgValues)... };
}


template<typename TValueType>
/**
 * @brief Constructs an empty std::array gien the template type
 *
 * @return empty std::array of specified type
 */
constexpr std::array<TValueType, 0> make_array()
{
	return std::array<TValueType, 0>{};
}

}


#endif
