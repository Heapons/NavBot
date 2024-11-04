#include <extension.h>
#include "tfnavmesh.h"
#include "tfnav_waypoint.h"

// Don't registers these commands on non TF2 mods
#if SOURCE_ENGINE == SE_TF2

CON_COMMAND_F(sm_tf_nav_waypoint_set_control_point, "Associate a control point index to the selected waypoint.", FCVAR_CHEAT)
{
	if (args.ArgC() < 2)
	{
		Msg("[SM] Usage: sm_tf_nav_waypoint_set_control_point <cp index>\n");
		Msg("Waypoint must be selected first.\nUse \"sm_navbot_tf_list_control_points\" to get a list of control points.\n");
		Msg("Pass -1 to remove association.\n");
		return;
	}

	int cpindex = atoi(args[1]);

	if (cpindex < CTFWaypoint::NO_CONTROL_POINT || cpindex >= MAX_CONTROL_POINTS)
	{
		Warning("Invalid control point index \"%i\"!", cpindex);
		cpindex = CTFWaypoint::NO_CONTROL_POINT;
	}

	auto& wptptr = TheNavMesh->GetSelectedWaypoint();

	if (wptptr.get() == nullptr)
	{
		Warning("Error: No waypoint selected!\n");
		TheNavMesh->PlayEditSound(CNavMesh::EditSoundType::SOUND_GENERIC_ERROR);
		return;
	}

	CTFWaypoint* wpt = static_cast<CTFWaypoint*>(wptptr.get());
	wpt->SetControlPointIndex(cpindex);
}

CON_COMMAND_F(sm_tf_nav_waypoint_set_hint, "Assings a TF hint to the selected waypoint.", FCVAR_CHEAT)
{
	if (args.ArgC() < 2)
	{
		Msg("[SM] Usage: sm_tf_nav_waypoint_set_hint <hint number>");
		Msg("A waypoint must be selected first.");
		return;
	}

	long long hint = atoll(args[1]);

	if (!CTFWaypoint::IsValidTFHint(hint))
	{
		Warning("Invalid TF Hint %ill", hint);
		TheNavMesh->PlayEditSound(CNavMesh::EditSoundType::SOUND_GENERIC_ERROR);
		return;
	}

	auto& wptptr = TheNavMesh->GetSelectedWaypoint();

	if (wptptr.get() == nullptr)
	{
		Warning("Error: No waypoint selected!\n");
		TheNavMesh->PlayEditSound(CNavMesh::EditSoundType::SOUND_GENERIC_ERROR);
		return;
	}

	CTFWaypoint* wpt = static_cast<CTFWaypoint*>(wptptr.get());
	wpt->SetTFHint(static_cast<CTFWaypoint::TFHint>(hint));
}

CON_COMMAND_F(sm_tf_nav_waypoint_list_all_hints, "List all available hints and their ID.", FCVAR_CHEAT)
{
	Msg("Listing all TF Hints.\n");

	for (unsigned int i = 0; i < static_cast<unsigned int>(CTFWaypoint::TFHint::MAX_TFHINT_TYPES); i++)
	{
		Msg("  TF Hint #%i : %s\n", i, CTFWaypoint::TFHintToString(static_cast<CTFWaypoint::TFHint>(i)));
	}
}

#endif // SOURCE_ENGINE == SE_TF2
