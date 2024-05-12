#include "pch.h"
#include "Projectile.h"
#include "GameData.h"

Projectile::Projectile(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{

}

Projectile::~Projectile()
{
}

void Projectile::Tick(GameData* _GD)
{
	if (DoesExist())
	{
		lifetime += _GD->m_dt;
		if (lifetime > 2.0f)
		{
			setExistence(false);
			lifetime = 0.0f;
		}
	}

	CMOGO::Tick(_GD);
}
