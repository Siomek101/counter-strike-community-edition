//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2MP_WEAPON_knife_H
#define HL2MP_WEAPON_knife_H
#pragma once


#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "weapon_hl2mpbasebasebludgeon.h"


#ifdef CLIENT_DLL
#define CWeaponknife C_Weaponknife
#endif

//-----------------------------------------------------------------------------
// CWeaponknife
//-----------------------------------------------------------------------------

class CWeaponknife : public CBaseHL2MPBludgeonWeapon
{
public:
	DECLARE_CLASS( CWeaponknife, CBaseHL2MPBludgeonWeapon );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

#if !defined( CLIENT_DLL ) || defined( SDK2013CE )
	DECLARE_ACTTABLE();
#endif

	CWeaponknife();

	float		GetRange( void );
	float		GetFireRate( void );

	void		AddViewKick( void );
	float		GetDamageForActivity( Activity hitActivity );
	void		SecondaryAttack( void )	{	return;	}

	void		Drop( const Vector &vecVelocity );


	// Animation event
#ifndef CLIENT_DLL
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	void HandleAnimEventMeleeHit( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	int WeaponMeleeAttack1Condition( float flDot, float flDist );
#endif

	CWeaponknife( const CWeaponknife & );

private:
		
};


#endif // HL2MP_WEAPON_knife_H

