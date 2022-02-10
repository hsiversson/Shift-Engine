#pragma once

#include <map>
#include <unordered_map>

template<class KeyType, class ItemType>
using SC_Map = std::map<KeyType, ItemType>;

template<class KeyType, class ItemType>
using SC_UnorderedMap = std::unordered_map<KeyType, ItemType>;