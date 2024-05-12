#pragma once
#include "CMOGO.h"
#include "GameData.h"

class Target :
    public CMOGO
{
public:
    Target(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
    ~Target();

    void targetHit();
    void targetMove();

    bool target_moves = false;

    virtual void Tick(GameData* _GD) override;
};

