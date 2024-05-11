#include "pch.h"
#include "Player.h"
#include <dinput.h>
#include "GameData.h"

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos = Vector3(-10.0f, 30.0f, -10.0f);

	SetDrag(10.0);
	SetPhysicsOn(true);
}

Player::~Player()
{
	//tidy up anything I've created
}


void Player::Tick(GameData* _GD)
{
	float speed = 675.0f;

	if (_GD->m_KBS.LeftShift)
	{
		speed = 1200.0f;
	}
	else
	{
		speed = 675.0f;
	}

	//player gravity
	Vector3 gravity = 200.0f * Vector3::Down;
	m_acc += gravity;
			
	//STRAFE AND FORWARD CONTROL HERE
	Vector3 forwardMove = speed * Vector3::Forward;
	Vector3 leftMove = (speed + 225.0f) * Vector3::Left;
	Vector3 rightMove = (speed + 225.0f) * Vector3::Right;
	Matrix rotMove = Matrix::CreateRotationY(m_yaw);

	forwardMove = Vector3::Transform(forwardMove, rotMove);
	leftMove = Vector3::Transform(leftMove, rotMove);
	rightMove = Vector3::Transform(rightMove, rotMove);

	if (_GD->m_KBS.W)
	{
		m_acc += forwardMove;
	}
	if (_GD->m_KBS.S)
	{
		m_acc -= forwardMove;
	}

	//change orinetation of player
	float rotSpeed = 2.0f * _GD->m_dt;

	m_yaw -= rotSpeed * _GD->m_MS.x;
	m_pitch -= rotSpeed * _GD->m_MS.y;

	if (m_pitch > XMConvertToRadians(60))
	{
		m_pitch = XMConvertToRadians(60);
	}
	if (m_pitch < -XMConvertToRadians(60))
	{
		m_pitch = -XMConvertToRadians(60);
	}

	if (_GD->m_KBS.A)
	{
		m_acc += leftMove;
	}
	if (_GD->m_KBS.D)
	{
		m_acc += rightMove;
	}

	//move player up and down
	if (_GD->m_KBS.R)
	{
		m_acc.y += 100.0f;
	}

	if (_GD->m_KBS.F)
	{
		m_acc.y -= 100.0f;
	}

	//limit motion of the player
	float length = m_pos.Length();
	float maxLength = 500.0f;
	if (length > maxLength)
	{
		m_pos.Normalize();
		m_pos *= maxLength;
		m_vel *= -0.9; //VERY simple bounce back
	}

	//apply my base behaviour
	CMOGO::Tick(_GD);
}