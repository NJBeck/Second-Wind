#include <bitset>
#include <map>
#include <vector>

enum class QTQuad {
	NE,
	NW,
	SE,
	SW
};

class QuadTree {
	// stores which entities are in which area of the map using a quadtree
	std::map<std::vector<QTQuad>, std::vector<uint32_t>, QTCompare> map_to_entities_;

public:
	add(uint32_t handle, )
};