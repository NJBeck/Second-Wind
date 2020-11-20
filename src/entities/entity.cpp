#include "entity.h"

unsigned long entity::count = 0;

entity::entity() {
	++count;
	handle = count;
}

void entity::OnNotify(std::vector<SDL_Event*>){}

bool entity::operator==(const entity& e) const
{
	return (e.handle == handle);
}
