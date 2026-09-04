#if defined _navbot_interfaces_playercontroller_included
 #endinput
#endif
#define _navbot_interfaces_playercontroller_included

/**
 * @brief Priorities for look/aim commands.
 */
enum NavBotLookPriority
{
	LOOK_IDLE = 0, // Idle look.
	LOOK_PATH, // Looking at the direction I am going
	LOOK_INTERESTING, // Looking at something interesting
	LOOK_ALLY, // Looking at my teammate
	LOOK_SEARCH, // Looking at potential enemy positions
	LOOK_ALERT, // Looking at something that alerted me
	LOOK_DANGER, // Looking at a danger source
	LOOK_USE, // Looking to use a door, button, etc
	LOOK_COMBAT, // Looking at en enemy
	LOOK_SUPPORT, // Looking to support my teammates
	LOOK_PRIORITY, // Looking at something that requires high priority
	LOOK_MOVEMENT, // Movement that requires looking in a specific direction (ie: ladders, jumps)
	LOOK_CRITICAL, // Something of very high importance
	LOOK_MANDATORY, // Nothing can interrupt this

	MAX_LOOK_PRIORITY
};

/**
 * Unique IDs for each individual button.
 * These buttons are sent when the bot generates a UserCmd.
 * Note that the button name may not match their actual purpose since mods can give any meaning to these buttons.
 */
enum NavBotButtonID
{
	NAVBOT_BUTTON_INVALID = 0,
	NAVBOT_BUTTON_ATTACKPRIM,
	NAVBOT_BUTTON_ATTACKSEC,
	NAVBOT_BUTTON_ATTACKSPECIAL,
	NAVBOT_BUTTON_JUMP,
	NAVBOT_BUTTON_CROUCH,
	NAVBOT_BUTTON_FORWARDS,
	NAVBOT_BUTTON_BACKWARDS,
	NAVBOT_BUTTON_USE,
	NAVBOT_BUTTON_MOVELEFT,
	NAVBOT_BUTTON_MOVERIGHT,
	NAVBOT_BUTTON_MOVEUP,
	NAVBOT_BUTTON_MOVEDOWN,
	NAVBOT_BUTTON_RELOAD,
	NAVBOT_BUTTON_ALT1,
	NAVBOT_BUTTON_RUN,
	NAVBOT_BUTTON_SPEED,
	NAVBOT_BUTTON_WALK,
	/* NavBot custom buttons for games/mod that adds new buttons. */
	NAVBOT_BUTTON_MODCUSTOM1,
	NAVBOT_BUTTON_MODCUSTOM2,
	NAVBOT_BUTTON_MODCUSTOM3,
	NAVBOT_BUTTON_MODCUSTOM4,

	MAX_NAVBOT_BUTTON_ID
};

// Interface responsible for player inputs.
methodmap NavBotPlayerControllerInterface < NavBotBotInterface
{
	/**
	 * Instructs the player controller to aim at this entity.
	 * 
	 * @param ptr			Interface pointer.
	 * @param entity		Entity to aim at.
	 * @param priority		Aim at command priority.
	 * @param duration		How long to keep aiming in seconds.
	 * @param reason		An optional message explaining why this command was given (printed when debugging is enabled).
	 */
	public static native void AimAtEntity(Address ptr, int entity, NavBotLookPriority priorty, float duration, const char[] reason = NULL_STRING);
	/**
	 * Instructs the player controller to aim at this entity.
	 * 
	 * @param ptr			Interface pointer.
	 * @param vec			Position to aim at.
	 * @param priority		Aim at command priority.
	 * @param duration		How long to keep aiming in seconds.
	 * @param reason		An optional message explaining why this command was given (printed when debugging is enabled).
	 */
	public static native void AimAtPos(Address ptr, const float vec[3], NavBotLookPriority priorty, float duration, const char[] reason = NULL_STRING);
	// Returns true if the bot aim is currently on the aim target.
	public static native bool IsAimOnTarget(Address ptr);
	/**
	 * Press a button by ID.
	 * 
	 * @param ptr			Interface pointer.
	 * @param button		Button ID to press.
	 * @param duration		How long in seconds to keep the button pressed. Negative values to tap the button.
	 * @error				Invalid interface pointer or invalid button ID.
	 */
	public static native void PressButtonByID(Address ptr, NavBotButtonID button, float duration = -1.0);
	/**
	 * Releases a button by ID.
	 * 
	 * @param ptr			Interface pointer.
	 * @param button		Button ID to release.
	 * @error				Invalid interface pointer or invalid button ID.
	 */
	public static native void ReleaseButtonByID(Address ptr, NavBotButtonID button);
	/**
	 * Gets the yaw and pitch difference of the bot's current eye angles and the desired look.
	 * If no aim at command is active, this returns the last computed difference.
	 * The illustration below shows the Pitch and Yaw signs based on where the bot's crosshair is in relation to the desired aim angles.
	 * 
	 * P = Pitch, Y = Yaw, X = look target
	 * 
	 *			|
	 *	  +P -Y | +P +Y
	 *			|
	 *	 -------X-------
	 *			|
	 *	  -P -Y | -P +Y
	 *			|
	 *
	 * @param ptr			Interface pointer.
	 * @param yaw			Float ref to write the yaw value to.
	 * @param pitch			Float ref to write the pitch value to.
	 */
	public static native void GetPitchYawDifference(Address ptr, float& yaw = 0.0, float& pitch = 0.0);
	/**
	 * Gets the world position the bot is currently trying to aim at.
	 * Note: If no aim at command is active, this returns the last position.
	 * 
	 * @param ptr			Interface pointer.
	 * @param pos			Float array to write the position to.
	 */
	public static native void GetLookAtTarget(Address ptr, float pos[3]);
}