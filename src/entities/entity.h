#pragma once
#include "SDL2/SDL.h"
#include <vector>


class entity {
	static uint64_t count;
public:
	typedef uint64_t ID;
	ID handle;
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
			return hash<uint64_t>()(e.handle);
		}
	};
}
