#ifndef NAVBOT_TF2_MOVEMENT_H_
#define NAVBOT_TF2_MOVEMENT_H_
#pragma once

#include <bot/interfaces/movement.h>

class CTF2Bot;

class CTF2BotMovement : public IMovement
{
public:
	CTF2BotMovement(CBaseBot* bot);
	~CTF2BotMovement() override;

	void Reset() override;
	void Update() override;

	// https://developer.valvesoftware.com/wiki/Team_Fortress_2/Mapper%27s_Reference#Jump_Distances
	float GetMaxJumpHeight() const override { return 72.0f; }
	float GetMaxDoubleJumpHeight() const override;
	float GetMaxGapJumpDistance() const override;
	void CrouchJump() override;
	void BlastJumpTo(const Vector& start, const Vector& landingGoal, const Vector& forward) override;
	bool IsAbleToDoubleJump() const override;
	// Can the bot perform a 'blast jump' (Example: TF2's rocket jump)
	bool IsAbleToBlastJump() const override;
	bool IsAbleToUseGrapplingHook() const override;
	bool IsAbleToUseOffMeshConnection(OffMeshConnectionType type, const NavOffMeshConnection* connection) const override;
protected:
	bool GapJumpRequiresDoubleJump(const Vector& landing, const Vector& forward) const override;
public:
	bool IsEntityTraversable(CBaseEntity* entity, const bool now = true) const override;
	bool IsControllingMovements() const override;
	bool IsPathingAllowed() const override;
	bool NeedsWeaponControl() const override;
	bool UseGrapplingHook(const Vector& start, const Vector& end) override;
	bool IsPathSegmentReached(const CMeshNavigator* nav, const BotPathSegment* goal, bool& resultoverride) const override;

private:
	static constexpr float min_health_for_rocket_jumps() { return 130.0f; }
	// if the gap length on a jump over gap is greater than this, then a scout bot will perform a double jump
	static constexpr float scout_gap_jump_do_double_distance() { return 280.0f; }

	class TFRocketJumpData
	{
	public:
		TFRocketJumpData()
		{
			Reset();
		}

		void Reset()
		{
			ClearJumpData();
			m_jumpFailedCooldown.Invalidate();
		}

		static constexpr int JUMP_STATE_INIT = 0;
		static constexpr int JUMP_STATE_RELOAD = 1;
		static constexpr int JUMP_STATE_WALK = 2;
		static constexpr int JUMP_STATE_SHOOT = 3;
		static constexpr int JUMP_STATE_LAND = 4;

		void ClearJumpData()
		{
			m_startPos.Init(0.0f, 0.0f, 0.0f);
			m_landingPos.Init(0.0f, 0.0f, 0.0f);
			m_dir.Init(0.0f, 0.0f, 0.0f);
			m_headingAng.Init(0.0f, 0.0f, 0.0f);
			m_shootAng.Init(0.0f, 0.0f, 0.0f);
			m_state = JUMP_STATE_INIT;
			m_isActive = false;
			m_isStandingLaunch = false;
		}

		Vector m_startPos;
		Vector m_landingPos;
		Vector m_dir;
		QAngle m_headingAng;
		QAngle m_shootAng;
		CountdownTimer m_jumpFailedCooldown;
		int m_state;
		bool m_isActive;
		bool m_isStandingLaunch;
	};

	TFRocketJumpData m_rjData;
	bool m_bIsUsingGrapplingHook;
	CountdownTimer m_failTimer;
	Vector m_grapplingHookGoal;

	void BlastJumpUpdate();
	void GrapplingHookUpdate();
	bool FindRocketJumpParameters();

	// https://github.com/Jump-Academy/smbl/blob/bf5fb7be728c5f66dcb31783d81a8747d1a12ffe/scripting/smbl/action/soldier/move/smbl_action_soldier_move_rocketjump.sp#L32
	static constexpr float RJ_MIN_START_SPEED = 239.0f;
	// https://github.com/Jump-Academy/smbl/blob/bf5fb7be728c5f66dcb31783d81a8747d1a12ffe/scripting/smbl/action/soldier/move/smbl_action_soldier_move_rocketjump.sp#L37
	static constexpr float WALK_TIME = 0.1350f;
	static constexpr float LAUNCHER_AIM_TIME = 0.0045f;
	static constexpr float ROCKET_BLAST_TIME = 0.0600f;
	static constexpr float GROUND_START_TIME = WALK_TIME + LAUNCHER_AIM_TIME + ROCKET_BLAST_TIME;
};

#endif // !NAVBOT_TF2_MOVEMENT_H_
