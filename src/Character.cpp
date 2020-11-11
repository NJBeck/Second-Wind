#include "Character.h"

Character::Character(QuadParams quad, double xPos, double yPos, CharacterHandlers h)
					:handlers(h)
{
	handlers.qHandler->add(handle, quad);
	handlers.posHandler->add(handle, { xPos, yPos });
}

void Character::move(double x, double y) {
	handlers.posHandler->ChangePos(handle, { x, y });
}