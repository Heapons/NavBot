#include NAVBOT_PCH_FILE
#include <util/pawnutils.h>
#include "debugoverlay.h"

namespace natives::debugoverlay
{
	static bool IsDedicatedServer()
	{
		return engine->IsDedicatedServer();
	}

	static cell_t DrawLine(IPluginContext* context, const cell_t* params)
	{
		// debugoverlay cannot be used in a dedicated server
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector start = pawnutils::ReadVector(context, params, 1);
		Vector end = pawnutils::ReadVector(context, params, 2);
		int r = std::clamp(0, 255, static_cast<int>(params[3]));
		int g = std::clamp(0, 255, static_cast<int>(params[4]));
		int b = std::clamp(0, 255, static_cast<int>(params[5]));
		bool noDepthTest = pawnutils::ReadBool(params, 6);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 7));
		NDebugOverlay::Line(start, end, r, g, b, noDepthTest, duration);
		return 0;
	}

	static cell_t DrawBox(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector origin = pawnutils::ReadVector(context, params, 1);
		Vector mins = pawnutils::ReadVector(context, params, 2);
		Vector maxs = pawnutils::ReadVector(context, params, 3);
		int r = std::clamp(0, 255, static_cast<int>(params[4]));
		int g = std::clamp(0, 255, static_cast<int>(params[5]));
		int b = std::clamp(0, 255, static_cast<int>(params[6]));
		int a = std::clamp(0, 255, static_cast<int>(params[7]));
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 8));
		NDebugOverlay::Box(origin, mins, maxs, r, g, b, a, duration);
		return 0;
	}

	static cell_t DrawSweptBox(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector start = pawnutils::ReadVector(context, params, 1);
		Vector end = pawnutils::ReadVector(context, params, 2);
		Vector mins = pawnutils::ReadVector(context, params, 3);
		Vector maxs = pawnutils::ReadVector(context, params, 4);
		QAngle angle = pawnutils::ReadAngle(context, params, 5);
		int r = std::clamp(0, 255, static_cast<int>(params[5]));
		int g = std::clamp(0, 255, static_cast<int>(params[6]));
		int b = std::clamp(0, 255, static_cast<int>(params[7]));
		int a = std::clamp(0, 255, static_cast<int>(params[8]));
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 9));
		NDebugOverlay::SweptBox(start, end, mins, maxs, angle, r, g, b, a, duration);
		return 0;
	}

	static cell_t DrawEntityBounds(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		CBaseEntity* entity = pawnutils::ReadEntity(context, params, 1);

		if (!entity)
		{
			return 0;
		}

		int r = std::clamp(0, 255, static_cast<int>(params[2]));
		int g = std::clamp(0, 255, static_cast<int>(params[3]));
		int b = std::clamp(0, 255, static_cast<int>(params[4]));
		int a = std::clamp(0, 255, static_cast<int>(params[5]));
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 6));
		NDebugOverlay::EntityBounds(entity, r, g, b, a, duration);
		return 0;
	}

	static cell_t DrawTriangle(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector p1 = pawnutils::ReadVector(context, params, 1);
		Vector p2 = pawnutils::ReadVector(context, params, 2);
		Vector p3 = pawnutils::ReadVector(context, params, 3);		
		int r = std::clamp(0, 255, static_cast<int>(params[4]));
		int g = std::clamp(0, 255, static_cast<int>(params[5]));
		int b = std::clamp(0, 255, static_cast<int>(params[6]));
		int a = std::clamp(0, 255, static_cast<int>(params[7]));
		bool noDepthTest = pawnutils::ReadBool(params, 8);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 9));
		NDebugOverlay::Triangle(p1, p2, p3, r, g, b, a, noDepthTest, duration);
		return 0;
	}

	static cell_t Text(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector origin = pawnutils::ReadVector(context, params, 1);
		bool viewCheck = pawnutils::ReadBool(params, 2);
		char* pText = pawnutils::ReadString(context, params, 3);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 4));
		NDebugOverlay::Text(origin, viewCheck, duration, "%s", pText);
		return 0;
	}

	static cell_t ScreenText(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		float xpos = pawnutils::ReadFloat(params, 1);
		float ypos = pawnutils::ReadFloat(params, 2);
		char* pText = pawnutils::ReadString(context, params, 3);
		int r = std::clamp(0, 255, static_cast<int>(params[4]));
		int g = std::clamp(0, 255, static_cast<int>(params[5]));
		int b = std::clamp(0, 255, static_cast<int>(params[6]));
		int a = std::clamp(0, 255, static_cast<int>(params[7]));
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 8));
		
		return 0;
	}

	static cell_t DrawCross3D(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector pos = pawnutils::ReadVector(context, params, 1);
		float size = pawnutils::ReadFloat(params, 2);
		int r = std::clamp(0, 255, static_cast<int>(params[3]));
		int g = std::clamp(0, 255, static_cast<int>(params[4]));
		int b = std::clamp(0, 255, static_cast<int>(params[5]));
		bool noDepthTest = pawnutils::ReadBool(params, 6);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 7));
		NDebugOverlay::Cross3D(pos, size, r, g, b, noDepthTest, duration);
		return 0;
	}

	static cell_t DrawHorzArrow(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector start = pawnutils::ReadVector(context, params, 1);
		Vector end = pawnutils::ReadVector(context, params, 2);
		float width = std::max(1.0f, pawnutils::ReadFloat(params, 3));
		int r = std::clamp(0, 255, static_cast<int>(params[4]));
		int g = std::clamp(0, 255, static_cast<int>(params[5]));
		int b = std::clamp(0, 255, static_cast<int>(params[6]));
		int a = std::clamp(0, 255, static_cast<int>(params[7]));
		bool noDepthTest = pawnutils::ReadBool(params, 8);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 9));
		NDebugOverlay::HorzArrow(start, end, width, r, g, b, a, noDepthTest, duration);
		return 0;
	}

	static cell_t DrawVertArrow(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector start = pawnutils::ReadVector(context, params, 1);
		Vector end = pawnutils::ReadVector(context, params, 2);
		float width = std::max(1.0f, pawnutils::ReadFloat(params, 3));
		int r = std::clamp(0, 255, static_cast<int>(params[4]));
		int g = std::clamp(0, 255, static_cast<int>(params[5]));
		int b = std::clamp(0, 255, static_cast<int>(params[6]));
		int a = std::clamp(0, 255, static_cast<int>(params[7]));
		bool noDepthTest = pawnutils::ReadBool(params, 8);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 9));
		NDebugOverlay::VertArrow(start, end, width, r, g, b, a, noDepthTest, duration);
		return 0;
	}

	static cell_t DrawSphere(IPluginContext* context, const cell_t* params)
	{
		if (IsDedicatedServer())
		{
			return 0;
		}

		Vector pos = pawnutils::ReadVector(context, params, 1);
		float radius = std::max(1.0f, pawnutils::ReadFloat(params, 2));
		int r = std::clamp(0, 255, static_cast<int>(params[3]));
		int g = std::clamp(0, 255, static_cast<int>(params[4]));
		int b = std::clamp(0, 255, static_cast<int>(params[5]));
		bool noDepthTest = pawnutils::ReadBool(params, 6);
		float duration = std::max(NDEBUG_PERSIST_FOR_ONE_TICK, pawnutils::ReadFloat(params, 7));
		NDebugOverlay::Sphere(pos, radius, r, g, b, noDepthTest, duration);
		return 0;
	}

	void setup(std::vector<sp_nativeinfo_t>& nv)
	{
		sp_nativeinfo_t list[] = {
			{"NavBotDebugOverlay.Line", DrawLine},
			{"NavBotDebugOverlay.Box", DrawBox},
			{"NavBotDebugOverlay.SweptBox", DrawSweptBox},
			{"NavBotDebugOverlay.EntityBounds", DrawEntityBounds},
			{"NavBotDebugOverlay.Triangle", DrawTriangle},
			{"NavBotDebugOverlay.Text", Text},
			{"NavBotDebugOverlay.ScreenText", ScreenText},
			{"NavBotDebugOverlay.Cross3D", DrawCross3D},
			{"NavBotDebugOverlay.HorzArrow", DrawHorzArrow},
			{"NavBotDebugOverlay.VertArrow", DrawVertArrow},
			{"NavBotDebugOverlay.Sphere", DrawSphere},
		};

		nv.insert(nv.end(), std::begin(list), std::end(list));
	}
}