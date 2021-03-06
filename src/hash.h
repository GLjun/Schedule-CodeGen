/*************************************************************************
	> File Name: hash.h
	> Author: 
	> Mail: 
	> Created Time: Thu 14 Mar 2019 04:57:22 AM UTC
 ************************************************************************/

#ifndef _HASH_H
#define _HASH_H

#include <functional>

#include "iter.h"


namespace std
{

/**
 * \bref default hash funciton of subclasses of refcountptr.
 * just hash on the pointer
 */
template <typename T>
struct hash_ref
{
    std::size_t operator()(T const& o) const noexcept
    {
        return std::hash<void*>{}(reinterpret_cast<void*>(o.get()));
    }
};

/**
 * \bref hash code of Iter
 */
template <>
struct hash<::SC::Iter> : public hash_ref<::SC::Iter> {};

} // namespace std


#endif
