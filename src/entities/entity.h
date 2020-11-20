#pragma once
#include "SDL2/SDL.h"
#include <vector>

class entity {
	static unsigned long count;
public:
	unsigned long handle;
	entity();
	bool operator==(const entity&) const;
	virtual void OnNotify(std::vector<SDL_Event*>);
};


namespace std
{
	template <>
	struct hash<entity>
	{
		size_t operator()(const entity& e) const
		{
			return hash<unsigned long>()(e.handle);
		}
	};
}
