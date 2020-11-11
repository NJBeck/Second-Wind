#include "entity.h"

unsigned long entity::handle = 0;

entity::entity() {
	++handle;
}