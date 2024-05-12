#include "pch.h"
#include "Target.h"

Target::~Target()
{
}

void Target::targetHit()
{
	setExistence(false);
	//TODO increment PUBLIC door value
}

void Target::targetMove()
{
	//TODO make targets move back and forth!!!! (call in tick function)
}

void Target::Tick(GameData* _GD)
{
	if (target_moves)
	{
		targetMove();
	}
}
