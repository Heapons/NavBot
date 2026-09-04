#include NAVBOT_PCH_FILE
#include <coordsize.h>
#include <bot/tf2/tf2bot.h>
#include <mods/tf2/tf2lib.h>
#include <mods/tf2/teamfortress2mod.h>
#include <entities/tf2/tf_entities.h>
#include "tf2bot_movement.h"

#ifdef EXT_VPROF_ENABLED
#include <tier0/vprof.h>
#endif // EXT_VPROF_ENABLED

CTF2BotMovement::CTF2BotMovement(CBaseBot* bot) : IMovement(bot)
{
	m_grapplingHookGoal = vec3_origin;
	m_bIsUsingGrapplingHook = false;
}

CTF2BotMovement::~CTF2BotMovement()
{
}

void CTF2BotMovement::Reset()
{
	m_rjData.Reset();
	m_bIsUsingGrapplingHook = false;

	IMovement::Reset();
}

void CTF2BotMovement::Update()
{
#ifdef EXT_VPROF_ENABLED
	VPROF_BUDGET("CTF2BotMovement::Update", "NavBot");
#endif // EXT_VPROF_ENABLED

	IMovement::Update();

	if (m_rjData.m_isActive)
	{
		BlastJumpUpdate();
	}

	if (m_bIsUsingGrapplingHook)
	{
		GrapplingHookUpdate();
	}
}

float CTF2BotMovement::GetMaxDoubleJumpHeight() const
{
	if (CTeamFortress2Mod::GetTF2Mod()->GetCurrentGameMode() == TeamFortress2::GameModeType::GM_VSH)
	{
		if (GetBot<CTF2Bot>()->IsSaxtonHale())
		{
			return 250.0f;
		}
	}

	return 116.0f;
}

float CTF2BotMovement::GetMaxGapJumpDistance() const
{
	if (CTeamFortress2Mod::GetTF2Mod()->GetCurrentGameMode() == TeamFortress2::GameModeType::GM_VSH)
	{
		if (GetBot<CTF2Bot>()->IsSaxtonHale())
		{
			return 600.0f;
		}
	}

	auto cls = tf2lib::GetPlayerClassType(GetBot()->GetIndex());

	switch (cls)
	{
	case TeamFortress2::TFClass_Scout:
		return 600.0f; // double jump gap distance
	case TeamFortress2::TFClass_Soldier:
		return 216.0f;
	case TeamFortress2::TFClass_DemoMan:
		return 240.0f;
	case TeamFortress2::TFClass_Medic:
		return 272.0f;
	case TeamFortress2::TFClass_Heavy:
		return 209.0f;
	case TeamFortress2::TFClass_Spy:
		return 272.0f;
	default:
		return 258.0f; // classes that moves at 'default speed' (engineer, sniper, pyro)
	}
}

void CTF2BotMovement::CrouchJump()
{
	CTF2Bot* me = GetBot<CTF2Bot>();

	// TF2: Cannot jump while crouched, if not jumping already, release the crouch button
	if (!m_jumpCooldown.HasStarted() && IsOnGround() && me->GetControlInterface()->IsPressingCrouchButton())
	{
		me->GetControlInterface()->ReleaseCrouchButton();
		me->GetControlInterface()->ReleaseJumpButton();
		// start the jump cooldown timer to give some time for the player to uncrouch
		m_jumpCooldown.Start(0.5f);

		if (me->IsDebugging(BOTDEBUG_MOVEMENT))
		{
			me->DebugPrintToConsole(255, 165, 0, "%s CTF2BotMovement::CrouchJump JUMP REJECTED, BOT IS CROUCHING! \n", me->GetDebugIdentifier());
		}

		return;
	}

	IMovement::CrouchJump();
}

void CTF2BotMovement::BlastJumpTo(const Vector& start, const Vector& landingGoal, const Vector& forward)
{
	CTF2Bot* me = GetBot<CTF2Bot>();
	const CBotWeapon* weapon = me->GetInventoryInterface()->FindWeaponByTag("can_rocket_jump");

	if (!weapon)
		return;

	if (weapon->IsOutOfAmmo(me))
	{
		m_rjData.m_jumpFailedCooldown.Start(60.0f);
		return;
	}

	me->GetInventoryInterface()->EquipWeapon(weapon);
	m_failTimer.Start(6.0f);
	m_rjData.ClearJumpData();
	m_rjData.m_startPos = me->GetAbsOrigin();
	m_rjData.m_landingPos = landingGoal;
	m_rjData.m_dir = UtilHelpers::math::BuildDirectionVectorIgnoreZ(m_rjData.m_startPos, m_rjData.m_landingPos);
	m_rjData.m_isActive = true;
	
	if (!FindRocketJumpParameters())
	{
		if (me->IsDebugging(BOTDEBUG_MOVEMENT))
		{
			me->DebugPrintToConsole(255, 255, 0, "%s FIND ROCKET JUMP PARAMETERS FAILED! LANDING: %g %g %g \n", me->GetDebugIdentifier(), landingGoal.x, landingGoal.y, landingGoal.z);
		}

		m_rjData.ClearJumpData();
		m_rjData.m_jumpFailedCooldown.Start(60.0f);
		return;
	}

	me->GetControlInterface()->ReleaseCrouchButton();
	me->GetControlInterface()->ReleaseJumpButton();

	if (me->IsDebugging(BOTDEBUG_MOVEMENT))
	{
		me->DebugPrintToConsole(255, 255, 0, "%s BLAST JUMPING! LANDING: %g %g %g \n", me->GetDebugIdentifier(), landingGoal.x, landingGoal.y, landingGoal.z);
		me->DebugPrintToConsole(255, 255, 0, " PARAMS:\n  SHOOT ANGLE: %s\n  STANDING LAUNCH: %s\n", 
			UtilHelpers::textformat::FormatAngles(m_rjData.m_shootAng), UtilHelpers::textformat::FormatBool(m_rjData.m_isStandingLaunch));
		NDebugOverlay::Text(landingGoal, "BLAST JUMP TARGET", false, 5.0f);
	}
}

bool CTF2BotMovement::IsAbleToDoubleJump() const
{
	if (CTeamFortress2Mod::GetTF2Mod()->GetCurrentGameMode() == TeamFortress2::GameModeType::GM_VSH)
	{
		if (GetBot<CTF2Bot>()->IsSaxtonHale())
		{
			return true;
		}
	}

	auto cls = tf2lib::GetPlayerClassType(GetBot()->GetIndex());

	if (cls == TeamFortress2::TFClass_Scout)
	{
		return true;
	}

	return false;
}

bool CTF2BotMovement::IsAbleToBlastJump() const
{
	// If a jump failed, wait for the cooldown so the bot takes another path
	if (!m_rjData.m_jumpFailedCooldown.IsElapsed())
	{
		return false;
	}

	auto cls = tf2lib::GetPlayerClassType(GetBot()->GetIndex());

	// TO-DO: Demoman and maybe engineer
	if (cls == TeamFortress2::TFClass_Soldier)
	{
		if (GetBot()->GetHealth() < min_health_for_rocket_jumps())
		{
			return false;
		}

		return true;
	}

	return false;
}

bool CTF2BotMovement::IsAbleToUseGrapplingHook() const
{
	return GetBot<CTF2Bot>()->GetInventoryInterface()->GetTheGrapplingHook() != nullptr;
}

bool CTF2BotMovement::IsAbleToUseOffMeshConnection(OffMeshConnectionType type, const NavOffMeshConnection* connection) const
{
	switch (type)
	{
	case OffMeshConnectionType::OFFMESH_JUMP_OVER_GAP:
	{
		Vector forward = UtilHelpers::math::BuildDirectionVector(connection->GetStart(), connection->GetEnd());
		const bool needsdoublejump = GapJumpRequiresDoubleJump(connection->GetEnd(), forward);

		if (needsdoublejump && !IsAbleToDoubleJump())
		{
			return false;
		}

		return true;
	}
	default:
		return IMovement::IsAbleToUseOffMeshConnection(type, connection);
	}
}

bool CTF2BotMovement::GapJumpRequiresDoubleJump(const Vector& landing, const Vector& forward) const
{
	float length = (GetBot<CTF2Bot>()->GetAbsOrigin() - landing).Length();

	if (length >= scout_gap_jump_do_double_distance())
	{
		return true;
	}

	return false;
}

bool CTF2BotMovement::IsEntityTraversable(CBaseEntity* entity, const bool now) const
{
	auto theirteam = tf2lib::GetEntityTFTeam(entity);
	auto myteam = GetBot<CTF2Bot>()->GetMyTFTeam();

	if (myteam == theirteam)
	{
		/* TO-DO: check solid teammates cvar */
		if (modhelpers->IsPlayer(entity))
		{
			return true;
		}

		if (UtilHelpers::FClassnameIs(entity, "obj_*"))
		{
			if (GetBot<CTF2Bot>()->GetMyClassType() == TeamFortress2::TFClassType::TFClass_Engineer)
			{
				CBaseEntity* builder = tf2lib::GetBuildingBuilder(entity);

				if (builder == GetBot<CTF2Bot>()->GetEntity())
				{
					return false; // my own buildings are solid to me
				}

				return true; // not my own buildings, not solid
			}

			return true; // not an engineer, friendly buildings are not solid (the telepoter is solid but we can generally walk over it)
		}
	}

	return IMovement::IsEntityTraversable(entity, now);
}

bool CTF2BotMovement::IsControllingMovements() const
{
	if (m_rjData.m_isActive)
	{
		return true;
	}

	if (m_bIsUsingGrapplingHook)
	{
		return true;
	}

	return IMovement::IsControllingMovements();
}

bool CTF2BotMovement::IsPathingAllowed() const
{
	if (m_bIsUsingGrapplingHook)
	{
		return false;
	}

	return IMovement::IsPathingAllowed();
}

bool CTF2BotMovement::NeedsWeaponControl() const
{
	if (m_rjData.m_isActive)
	{
		return true;
	}

	if (m_bIsUsingGrapplingHook)
	{
		return true;
	}

	return IMovement::NeedsWeaponControl();
}

bool CTF2BotMovement::UseGrapplingHook(const Vector& start, const Vector& end)
{
	CTF2Bot* me = GetBot<CTF2Bot>();
	CTF2BotInventory* inv = me->GetInventoryInterface();
	const CTF2BotWeapon* grapple = inv->GetTheGrapplingHook();

	if (!grapple)
	{
		return false;
	}

	m_grapplingHookGoal = end;
	m_bIsUsingGrapplingHook = true;

	const float range = (start - end).Length();
	constexpr float SPEED = 400.0f;
	constexpr float MIN_TIME = 7.0f;
	float time = range / SPEED;
	time = std::max(time, MIN_TIME);

	m_failTimer.Start(time);
	inv->EquipWeapon(grapple);

	if (me->IsDebugging(BOTDEBUG_MOVEMENT))
	{
		me->DebugPrintToConsole(0, 180, 0, "%s USE GRAPPLING HOOK (TRAVEL DISTANCE: %g)\n", me->GetDebugIdentifier(), range);
	}

	return true;
}

bool CTF2BotMovement::IsPathSegmentReached(const CMeshNavigator* nav, const BotPathSegment* goal, bool& resultoverride) const
{
	if (goal->type == AIPath::SegmentType::SEGMENT_GRAPPLING_HOOK && m_bIsUsingGrapplingHook)
	{
		resultoverride = true;
		return true;
	}


	return false;
}

void CTF2BotMovement::BlastJumpUpdate()
{
	CTF2Bot* me = GetBot<CTF2Bot>();
	const CBotWeapon* weapon = me->GetInventoryInterface()->FindWeaponByTag("can_rocket_jump");

	if (!weapon)
	{
		m_rjData.ClearJumpData();
		m_rjData.m_jumpFailedCooldown.Start(60.0f);
		return;
	}

	switch (m_rjData.m_state)
	{
	case TFRocketJumpData::JUMP_STATE_INIT:
	{
		if (!weapon->IsLoaded())
		{
			m_rjData.m_state = TFRocketJumpData::JUMP_STATE_RELOAD;
			me->GetControlInterface()->PressReloadButton();
			return;
		}

		m_rjData.m_state = TFRocketJumpData::JUMP_STATE_WALK;
		break;
	}
	case TFRocketJumpData::JUMP_STATE_RELOAD:
	{
		if (weapon->IsLoaded())
		{
			m_rjData.m_state = TFRocketJumpData::JUMP_STATE_WALK;
		}

		break;
	}
	case TFRocketJumpData::JUMP_STATE_WALK:
	{
		Vector forward;
		AngleVectors(m_rjData.m_shootAng, &forward);
		Vector lookAt = (me->GetEyeOrigin() + (forward * 512.0f));
		me->GetControlInterface()->AimAt(lookAt, IPlayerController::LOOK_MOVEMENT, 1.0f, "Rocket Jump aim!");

		if (me->GetControlInterface()->IsAimOnTarget())
		{
			if (m_rjData.m_isStandingLaunch)
			{
				m_rjData.m_state = TFRocketJumpData::JUMP_STATE_SHOOT;
				return;
			}

			MoveTowards(m_rjData.m_landingPos);

			const Vector& vel = me->GetAbsVelocity();

			if (vel.IsLengthGreaterThan(RJ_MIN_START_SPEED))
			{
				QAngle velAngles;
				VectorAngles(UtilHelpers::GetNormalizedVector(vel), velAngles);

				float angDiff = AngleDiff(velAngles.y, m_rjData.m_headingAng.y);

				if (angDiff < 15.0f)
				{
					m_rjData.m_state = TFRocketJumpData::JUMP_STATE_SHOOT;
					return;
				}
			}
		}

		break;
	}
	case TFRocketJumpData::JUMP_STATE_SHOOT:
	{
		if (m_rjData.m_isStandingLaunch)
		{
			me->GetControlInterface()->PressJumpButton();
			me->GetControlInterface()->PressCrouchButton();
		}

		me->GetControlInterface()->PressAttackButton();
		m_rjData.m_state = TFRocketJumpData::JUMP_STATE_LAND;
		break;
	}
	case TFRocketJumpData::JUMP_STATE_LAND:
	{
		Vector lookAt = m_rjData.m_landingPos;
		lookAt.z += GetCrouchedHullHeight();
		me->GetControlInterface()->AimAt(lookAt, IPlayerController::LOOK_MOVEMENT, 1.0f, "Looking at rocket jump landing!");
		Vector origin = me->GetAbsOrigin();
		
		if (origin.z >= m_rjData.m_landingPos.z)
		{
			AirStrafeTowards(m_rjData.m_landingPos, true, MOVEWEIGHT_CRITICAL);
		}

		if (IsOnGround())
		{
			m_rjData.ClearJumpData();
			return;
		}

		break;
	}
	default:
		break;
	}
}

void CTF2BotMovement::GrapplingHookUpdate()
{
	Vector lookAt = m_grapplingHookGoal;
	lookAt.z += GetStandingHullHeight() * 0.75f;

	CTF2Bot* me = GetBot<CTF2Bot>();
	CTF2BotPlayerController* input = me->GetControlInterface();
	CTF2BotInventory* inv = me->GetInventoryInterface();
	const CTF2BotWeapon* active = inv->GetActiveTFWeapon();
	const CTF2BotWeapon* grapple = inv->GetTheGrapplingHook();

	if (!grapple)
	{
		m_bIsUsingGrapplingHook = false;
	}

	if (active != grapple)
	{
		inv->EquipWeapon(grapple);
		return;
	}

	if (m_failTimer.IsElapsed())
	{
		if (me->IsDebugging(BOTDEBUG_MOVEMENT))
		{
			me->DebugPrintToConsole(255, 0, 0, "%s USING GRAPPLING HOOK! TIMED OUT!\n", me->GetDebugIdentifier());
		}

		m_bIsUsingGrapplingHook = false;
		m_failTimer.Invalidate();
	}

	input->AimAt(lookAt, IPlayerController::LOOK_MOVEMENT, 0.5f, "Looking at grappling hook goal!");

	const float tolerance = GetHullWidth() * 1.5f;

	if (input->IsAimOnTarget())
	{
		input->PressAttackButton(0.2f);
	}

	Vector origin = me->GetAbsOrigin();
	const float range = (origin - m_grapplingHookGoal).Length2D();

	if (me->IsDebugging(BOTDEBUG_MOVEMENT))
	{
		me->DebugPrintToConsole(220, 208, 255, "%s USING GRAPPLING HOOK! DISTANCE TO GOAL: %g\n", me->GetDebugIdentifier(), range);
	}

	if (range <= tolerance)
	{
		m_bIsUsingGrapplingHook = false;
		m_failTimer.Invalidate();
		input->ReleaseAllAttackButtons();
		inv->SelectBestWeapon();
	}
}

/*
* Rocket jumping V2.
* Ported from https://github.com/Jump-Academy/smbl
*/

static void ShiftGroundPosition2D(const Vector& vecStartPos, const Vector& vecDir, const float fSpeed, const float fTime, Vector& vecEndPos) 
{
	float fMoveDist = fSpeed * fTime;
	vecEndPos.x = vecStartPos.x + fMoveDist * vecDir.x;
	vecEndPos.y = vecStartPos.y + fMoveDist * vecDir.y;
	vecEndPos.z = vecStartPos.z;
}

// Helpers
// https://github.com/Jump-Academy/smbl/blob/bf5fb7be728c5f66dcb31783d81a8747d1a12ffe/scripting/smbl/action/soldier/move/rocketjump/ground_shot_back.sp#L269

static float GetInitialVel2D(float fPitchAng) {
	float fX = fPitchAng;
	float fX2 = fX * fX;
	float fX3 = fX2 * fX;
	float fX4 = fX3 * fX;
	float fX5 = fX4 * fX;

	// Coefficients for 1 tick delay between jump and shoot
	return \
		- 131.62623492f * fX \
		+ 4.68495106f * fX2 \
		- 0.07703477f * fX3 \
		+ 0.00058851f * fX4 \
		- 0.00000172f * fX5 \
		+ 1699.365006059887f;
}

static float GetInitialVelZ(float fPitchAng) {
	// Coefficients for 1 tick delay between jump and shoot
	return \
		21.16759727f * fPitchAng \
		- 0.10122961f * fPitchAng * fPitchAng \
		- 191.58256250650533f;
}

static float GetYawAngleCompensation(float fPitchAng) {
	float fX = fPitchAng;
	float fX2 = fX * fX;
	float fX3 = fX2 * fX;
	float fX4 = fX3 * fX;
	float fX5 = fX4 * fX;

	return \
		14.10738620f * fX \
		- 0.54216773f * fX2 \
		+ 0.01036420f * fX3 \
		- 0.00009752f * fX4 \
		+ 0.00000037f * fX5  \
		- 141.3442763196645f;
}

// https://github.com/Jump-Academy/smbl/blob/bf5fb7be728c5f66dcb31783d81a8747d1a12ffe/scripting/smbl/action/soldier/move/smbl_action_soldier_move_rocketjump.sp#L395
static bool CheckParabolicCollision(CTF2Bot* bot, const Vector& vecMins, const Vector& vecMaxs, const Vector& vecDir, float gravity, float time, const Vector& vecStartPos, float vel2D, float velZ) 
{
#ifdef EXT_VPROF_ENABLED
	VPROF_BUDGET("CheckParabolicCollision", "NavBot");
#endif // EXT_VPROF_ENABLED

	Vector vecLastPt = vecStartPos;
	Vector vecPt;
	trace::CTraceFilterSimple filter(bot->GetEntity(), COLLISION_GROUP_PLAYER);
	trace_t tr;

	for (float fT = 0.1; fT <= time; fT += 0.15f) 
	{
		vecPt.x = vecStartPos.x + vecDir.x * fT * vel2D;
		vecPt.y = vecStartPos.y + vecDir.y * fT * vel2D;
		vecPt.z = vecStartPos.z + velZ * fT + 0.5f * gravity * fT * fT;

		if (trace::pointoutisdeworld(vecPt))
		{
			return true;
		}

		trace::hull(vecLastPt, vecPt, vecMins, vecMaxs, MASK_SHOT_HULL, &filter, tr);

		if (tr.DidHit())
		{
			return true;
		}

		vecLastPt = vecPt;
	}

	return false;
}

bool CTF2BotMovement::FindRocketJumpParameters()
{
#ifdef EXT_VPROF_ENABLED
	VPROF_BUDGET("CTF2BotMovement::FindRocketJumpParameters", "NavBot");
#endif // EXT_VPROF_ENABLED

	CTF2Bot* me = GetBot<CTF2Bot>();
	float dist2D = (m_rjData.m_startPos - m_rjData.m_landingPos).AsVector2D().Length();
	trace::CTraceFilterSimple filter(me->GetEntity(), COLLISION_GROUP_PLAYER);
	trace_t tr;
	QAngle angles;
	VectorAngles(m_rjData.m_dir, angles);
	Vector walkEndPos = m_rjData.m_startPos;

	if (!m_rjData.m_isStandingLaunch)
	{
		ShiftGroundPosition2D(m_rjData.m_startPos, m_rjData.m_dir, RJ_MIN_START_SPEED, GROUND_START_TIME, walkEndPos);

		Vector traceStartPos = m_rjData.m_startPos;
		traceStartPos.z += 50.0f;
		Vector dir = (walkEndPos - traceStartPos);
		dir.NormalizeInPlace();
		Vector traceEndPos = traceStartPos + (dir * MAX_COORD_FLOAT);
		trace::line(traceStartPos, traceEndPos, MASK_SHOT_HULL, &filter, tr);

		if (tr.DidHit())
		{
			float tracedDistance = (tr.endpos - walkEndPos).Length();
			float expectedDistance = (traceStartPos - walkEndPos).Length();

			if (std::abs(tracedDistance - expectedDistance) > 10.0f)
			{
				m_rjData.m_isStandingLaunch = true;
				walkEndPos = m_rjData.m_startPos;
			}
		}
	}

	float gravity = CExtManager::GetSvGravityValue();

	Vector mins(-24.0, -24.0, 0.0);
	Vector maxs(24.0, 24.0, 82.0);

	Vector traceStartPos = m_rjData.m_startPos;
	traceStartPos.z += (maxs.z * 0.75f);

	float bestPitchAng = 0.0f;
	float bestVel2D = 0.0f;
	float groundStartSpeed = m_rjData.m_isStandingLaunch ? 0.0f : RJ_MIN_START_SPEED;

	for (float testPitchAng = 35.0f; testPitchAng < 90.0f; testPitchAng += 5.0f)
	{
		float initialVel2D = groundStartSpeed + GetInitialVel2D(testPitchAng);
		float time2D = dist2D / initialVel2D;

		float initialVelZ = GetInitialVelZ(testPitchAng);

		// d = v0*t + 0.5*g*t^2 = (v0 + 0.5*g*t)*t
		float predictedZ = walkEndPos.z + (initialVelZ + 0.5 * gravity * time2D) * time2D;

		// vf = v0 + g*t
		float predictedVelZ = initialVelZ + gravity * time2D;

		if (predictedZ < m_rjData.m_landingPos.z)
		{
			continue;
		}

		if (CheckParabolicCollision(me, mins, maxs, m_rjData.m_dir, gravity, time2D, walkEndPos, initialVel2D, initialVelZ))
		{
			continue;
		}

		QAngle angRocket(0.0f, 0.0f, 0.0f);
		angRocket.x = testPitchAng;
		angRocket.y = AngleNormalize(angles.y + 180.0f + GetYawAngleCompensation(testPitchAng));

		Vector forward, right, up;
		AngleVectors(angRocket, &forward, &right, &up);

		right * 12.0f;

		Vector launcherPos = traceStartPos + right;
		Vector endpos = traceStartPos + (forward * MAX_COORD_FLOAT);
		trace::line(launcherPos, endpos, MASK_SHOT_HULL, &filter, tr);

		if (tr.DidHit())
		{
			if (std::abs(tr.endpos.z - walkEndPos.z) > 10.0f)
			{
				continue;
			}
		}

		if (initialVel2D < bestVel2D)
		{
			break;
		}

		bestVel2D = initialVel2D;
		bestPitchAng = testPitchAng;
	}

	if (bestPitchAng <= 0.01f)
	{
		return false;
	}

	m_rjData.m_shootAng.x = bestPitchAng;
	m_rjData.m_shootAng.y = AngleNormalize(angles.y + 180.0f + GetYawAngleCompensation(bestPitchAng));
	m_rjData.m_headingAng.y = angles.y;

	return true;
}
