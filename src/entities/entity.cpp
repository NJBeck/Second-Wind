#include "entity.h"

uint64_t entity::count = 0;

entity::entity() {
	++count;
	handle = count;
}

void entity::OnNotify(std::vector<SDL_Event*>){}

bool entity::operator==(const entity& e) const
{
	return (e.handle == handle);
}
