//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#if defined( CLIENT_DLL )
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbase_machinegun.h"
#include "in_buttons.h"


#define	PISTOL_FASTEST_REFIRE_TIME		0.1f
#define	PISTOL_FASTEST_DRY_REFIRE_TIME	0.2f

#define	PISTOL_ACCURACY_SHOT_PENALTY_TIME		0.2f	// Applied amount of time each shot adds to the time we must recover from
#define	PISTOL_ACCURACY_MAXIMUM_PENALTY_TIME	1.5f	// Maximum penalty to deal out

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED(HL2MPMachineGun, DT_HL2MPMachineGun)

BEGIN_NETWORK_TABLE(CHL2MPMachineGun, DT_HL2MPMachineGun)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CHL2MPMachineGun)
END_PREDICTION_DATA()

//=========================================================
//	>> CHLSelectFireMachineGun
//=========================================================
BEGIN_DATADESC(CHL2MPMachineGun)

DEFINE_FIELD(m_nShotsFired, FIELD_INTEGER),
DEFINE_FIELD(m_flNextSoundTime, FIELD_TIME),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHL2MPMachineGun::CHL2MPMachineGun( void )
{
}

const Vector &CHL2MPMachineGun::GetBulletSpread( void )
{
	static Vector cone;

	return cone;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &info - 
//-----------------------------------------------------------------------------
void CHL2MPMachineGun::FireBullets( const FireBulletsInfo_t &info )
{
	if(CBasePlayer *pPlayer = ToBasePlayer ( GetOwner() ) )
	{
		pPlayer->FireBullets(info);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPMachineGun::DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime )
{
	#define	KICK_MIN_X			0.6f	//Degrees
	#define	KICK_MIN_Y			0.6f	//Degrees
	#define	KICK_MIN_Z			0.4f	//Degrees

	QAngle vecScratch;
	int iSeed = CBaseEntity::GetPredictionRandomSeed() & 255;
	
	//Find how far into our accuracy degradation we are
	float duration	= ( fireDurationTime > slideLimitTime ) ? slideLimitTime : fireDurationTime;
	float kickPerc = duration / slideLimitTime;

	// do this to get a hard discontinuity, clear out anything under 10 degrees punch
	pPlayer->ViewPunchReset( 10 );

	//Apply this to the view angles as well
	vecScratch.x = -( KICK_MIN_X + ( maxVerticleKickAngle * kickPerc ) );
	vecScratch.y = -( KICK_MIN_Y + ( maxVerticleKickAngle * kickPerc ) ) / 3;
	vecScratch.z = KICK_MIN_Z + ( maxVerticleKickAngle * kickPerc ) / 8;

	RandomSeed( iSeed );

	//Wibble left and right
	if ( RandomInt( -1, 1 ) >= 0 )
		vecScratch.y *= -1;

	iSeed++;

	//Wobble up and down
	if ( RandomInt( -1, 1 ) >= 0 )
		vecScratch.z *= -1;

	//Clip this to our desired min/max
	UTIL_ClipPunchAngleOffset( vecScratch, pPlayer->m_Local.m_vecPunchAngle, QAngle( 24.0f, 3.0f, 1.0f ) );

	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	pPlayer->ViewPunch( vecScratch * 0.5 );
}

//-----------------------------------------------------------------------------
// Purpose: Reset our shots fired
//-----------------------------------------------------------------------------
bool CHL2MPMachineGun::Deploy( void )
{
	m_nShotsFired = 0;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: Make enough sound events to fill the estimated think interval
// returns: number of shots needed
//-----------------------------------------------------------------------------
int CHL2MPMachineGun::WeaponSoundRealtime( WeaponSound_t shoot_type )
{
	int numBullets = 0;

	// ran out of time, clamp to current
	if (m_flNextSoundTime < gpGlobals->curtime)
	{
		m_flNextSoundTime = gpGlobals->curtime;
	}

	// make enough sound events to fill up the next estimated think interval
	float dt = clamp( m_flAnimTime - m_flPrevAnimTime, 0, 0.2 );
	if (m_flNextSoundTime < gpGlobals->curtime + dt)
	{
		WeaponSound( SINGLE_NPC, m_flNextSoundTime );
		m_flNextSoundTime += GetFireRate();
		numBullets++;
	}
	if (m_flNextSoundTime < gpGlobals->curtime + dt)
	{
		WeaponSound( SINGLE_NPC, m_flNextSoundTime );
		m_flNextSoundTime += GetFireRate();
		numBullets++;
	}

	return numBullets;
}

#if defined( CLIENT_DLL )

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f

extern float	g_lateralBob;
extern float	g_verticalBob;

static ConVar	cl_bobcycle("cl_bobcycle", "0.8");
static ConVar	cl_bob("cl_bob", "0.01");
static ConVar	cl_bobup("cl_bobup", "0.5");

// Register these cvars if needed for easy tweaking
static ConVar	v_iyaw_cycle("v_iyaw_cycle", "2", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iroll_cycle("v_iroll_cycle", "0.5", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_ipitch_cycle("v_ipitch_cycle", "1", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iyaw_level("v_iyaw_level", "0.3", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iroll_level("v_iroll_level", "0.1", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_ipitch_level("v_ipitch_level", "0.3", FCVAR_REPLICATED | FCVAR_CHEAT);

float CHL2MPMachineGun::CalcViewmodelBob(void)
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer *player = ToBasePlayer(GetOwner());
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ((!gpGlobals->frametime) || (player == NULL))
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	speed = clamp(speed, -320, 320);

	float bob_offset = RemapVal(speed, 0, 320, 0.0f, 1.0f);

	bobtime += (gpGlobals->curtime - lastbobtime) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX)*HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin(cycle);

	g_verticalBob = clamp(g_verticalBob, -7.0f, 4.0f);

	//Calculate the lateral bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX * 2)*HL2_BOB_CYCLE_MAX * 2;
	cycle /= HL2_BOB_CYCLE_MAX * 2;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin(cycle);
	g_lateralBob = clamp(g_lateralBob, -7.0f, 4.0f);

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CHL2MPMachineGun::AddViewmodelBob(CBaseViewModel *viewmodel, Vector &origin, QAngle &angles)
{

	Vector	forward, right;
	AngleVectors(angles, &forward, &right, NULL);

	CalcViewmodelBob();

	// Apply bob, but scaled down to 40%
	VectorMA(origin, g_verticalBob * 0.1f, forward, origin);

	// Z bob a bit more
	origin[2] += g_verticalBob * 0.1f;

	// bob the angles
	angles[ROLL] += g_verticalBob * 0.5f;
	angles[PITCH] -= g_verticalBob * 0.4f;

	angles[YAW] -= g_lateralBob  * 0.3f;

	VectorMA(origin, g_lateralBob * 0.9f, right, origin);
}


#else

// Server stubs
float CHL2MPMachineGun::CalcViewmodelBob(void)
{
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CHL2MPMachineGun::AddViewmodelBob(CBaseViewModel *viewmodel, Vector &origin, QAngle &angles)
{
}

#endif


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPMachineGun::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	// Debounce the recoiling counter
	if ( ( pOwner->m_nButtons & IN_ATTACK ) == false )
	{
		m_nShotsFired = 0;
	}

	BaseClass::ItemPostFrame();
}


