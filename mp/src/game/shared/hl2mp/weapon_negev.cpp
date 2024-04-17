//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#include "c_hl2mp_player.h"
#else
#include "grenade_ar2.h"
#include "hl2mp_player.h"
#include "basegrenade_shared.h"
#endif

#include "weapon_hl2mpbase.h"
#include "weapon_hl2mpbase_machinegun.h"

#ifdef CLIENT_DLL
#define CWeaponNegev C_WeaponNegev
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define Negev_GRENADE_DAMAGE 100.0f
#define Negev_GRENADE_RADIUS 250.0f

class CWeaponNegev : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS(CWeaponNegev, CHL2MPMachineGun);

	CWeaponNegev();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();


	void	Precache(void);
	void	AddViewKick(void);
	void	SecondaryAttack(void);

	float GetRecoilScale() { return 1.0; };
	float  GetMinRecoilSpread() { return 0.01; };
	float  GetMaxRecoilSpread() { return 0.01; };
	Vector	GetSpread() { return Vector(0,0,0); }

	int		GetMinBurst() { return 2; }
	int		GetMaxBurst() { return 5; }

	virtual void Equip(CBaseCombatCharacter* pOwner);
	bool	Reload(void);

	float	GetFireRate(void) { return 0.075f; }	// 13.3hz
	Activity	GetPrimaryAttackActivity(void);

	virtual Vector& GetBulletSpread(void)
	{
		static Vector cone = Vector(0,0,0);
		cone.x = VECTOR_CONE_1DEGREES.x;
		cone.y = cone.x;
		cone.z = cone.x;
		Vector vel = GetOwner()->GetLocalVelocity();
		float speed = vel.x + vel.y + vel.z;
		speed = Min(Max(abs(speed),1.0f), 300.0f);
		if (speed < 150) return cone;
		cone.x = cone.x * speed;
		cone.y = cone.x;
		cone.z = cone.x;
		Msg("%f : %f\n ", speed, cone.x);
		return cone;
	}

	const WeaponProficiencyInfo_t* GetProficiencyValues();

#if !defined( CLIENT_DLL ) || defined( SDK2013CE )
	DECLARE_ACTTABLE();
#endif

protected:

	Vector	m_vecTossVelocity;
	float	m_flNextGrenadeCheck;

private:
	CWeaponNegev(const CWeaponNegev&);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponNegev, DT_WeaponNegev)

BEGIN_NETWORK_TABLE(CWeaponNegev, DT_WeaponNegev)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponNegev)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_negev, CWeaponNegev);
PRECACHE_WEAPON_REGISTER(weapon_negev);

#if !defined( CLIENT_DLL ) || defined( SDK2013CE )
acttable_t	CWeaponNegev::m_acttable[] =
{
#ifdef SDK2013CE
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SMG1,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SMG1,						false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SMG1,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SMG1,					false },
#else
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SMG1,						false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SMG1,				false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SMG1,				false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SMG1,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SMG1,					false },
#endif // SDK2013CE
};

IMPLEMENT_ACTTABLE(CWeaponNegev);
#endif

//=========================================================
CWeaponNegev::CWeaponNegev()
{
	m_fMinRange1 = 0;// No minimum range. 
	m_fMaxRange1 = 1400;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponNegev::Precache(void)
{
#ifndef CLIENT_DLL
	//UTIL_PrecacheOther("grenade_ar2");
#endif

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Give this weapon longer range when wielded by an ally NPC.
//-----------------------------------------------------------------------------
void CWeaponNegev::Equip(CBaseCombatCharacter* pOwner)
{
	m_fMaxRange1 = 1400;

	BaseClass::Equip(pOwner);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponNegev::GetPrimaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
	if (m_nShotsFired < 2)
		return ACT_VM_PRIMARYATTACK;

	if (m_nShotsFired < 3)
		return ACT_VM_RECOIL1;

	if (m_nShotsFired < 4)
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponNegev::Reload(void)
{
	bool fRet;
	float fCacheTime = m_flNextSecondaryAttack;

	fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (fRet)
	{
		// Undo whatever the reload process has done to our secondary
		// attack timer. We allow you to interrupt reloading to fire
		// a grenade.
		m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;

		WeaponSound(RELOAD);
#ifdef SDK2013CE
		ToHL2MPPlayer(GetOwner())->DoAnimationEvent(PLAYERANIMEVENT_RELOAD);
#endif // SDK2013CE
	}

	return fRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponNegev::AddViewKick(void)
{
#define	EASY_DAMPEN			0.5f
#define	MAX_VERTICAL_KICK	1.0f	//Degrees
#define	SLIDE_LIMIT			2.0f	//Seconds

	//Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	DoMachineGunKick(pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponNegev::SecondaryAttack(void)
{
	return;
	/*
	// Only the player fires this way so we can cast
#ifdef SDK2013CE
	CHL2MP_Player* pPlayer = ToHL2MPPlayer(GetOwner());
#else
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
#endif // SDK2013CE

	if (pPlayer == NULL)
		return;

	//Must have ammo
	if ((pPlayer->GetAmmoCount(m_iSecondaryAmmoType) <= 0) || (pPlayer->GetWaterLevel() == 3))
	{
		SendWeaponAnim(ACT_VM_DRYFIRE);
		BaseClass::WeaponSound(EMPTY);
		m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;
		return;
	}

	if (m_bInReload)
		m_bInReload = false;

	// MUST call sound before removing a round from the clip of a CMachineGun
	BaseClass::WeaponSound(WPN_DOUBLE);

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors(pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow);
	VectorScale(vecThrow, 1000.0f, vecThrow);

#ifndef CLIENT_DLL
	//Create the grenade
	CGrenadeAR2* pGrenade = (CGrenadeAR2*)Create("grenade_ar2", vecSrc, vec3_angle, pPlayer);
	pGrenade->SetAbsVelocity(vecThrow);

	pGrenade->SetLocalAngularVelocity(RandomAngle(-400, 400));
	pGrenade->SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
	pGrenade->SetThrower(GetOwner());
	pGrenade->SetDamage(Negev_GRENADE_DAMAGE);
	pGrenade->SetDamageRadius(Negev_GRENADE_RADIUS);
#endif

	SendWeaponAnim(ACT_VM_SECONDARYATTACK);

	// player "shoot" animation
#ifdef SDK2013CE
	pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
#else
	pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif // SDK2013CE

	// Decrease ammo
	pPlayer->RemoveAmmo(1, m_iSecondaryAmmoType);

	// Can shoot again immediately
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

	// Can blow up after a short delay (so have time to release mouse button)
	m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;*/
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t* CWeaponNegev::GetProficiencyValues()
{
	static WeaponProficiencyInfo_t proficiencyTable[] =
	{
		{ 7.0,		0.75	},
		{ 5.00,		0.75	},
		{ 10.0 / 3.0, 0.75	},
		{ 5.0 / 3.0,	0.75	},
		{ 1.00,		1.0		},
	};

	COMPILE_TIME_ASSERT(ARRAYSIZE(proficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

	return proficiencyTable;
}
