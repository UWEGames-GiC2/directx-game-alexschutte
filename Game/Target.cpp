#include "pch.h"
#include "Target.h"
#include "Door.h"

Target::Target(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	
}

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
	CMOGO::Tick(_GD);
	if (target_moves)
	{
		targetMove();
	}
}