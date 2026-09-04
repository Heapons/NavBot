#if defined _navbot_debugoverlay_included
 #endinput
#endif
#define _navbot_debugoverlay_included

/**
 * Provides access to NavBot's implementation of the game's NDebugOverlay.
 * Important note: These functions only works on a Listen Server and the debug overlays are only visible to the host.
 * These natives will do nothing if called on a dedicated server.
 * Having too many debug overlays at once can crash the game.
 */
methodmap NavBotDebugOverlay
{
	/**
	 * Draws a line.
	 * 
	 * @param start         Line start position.
	 * @param end           Line end position.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param noDepthTest   If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Line(const float start[3], const float end[3], int r, int g, int b, const bool noDepthTest, const float duration = -1.0);
	/**
	 * Draws a box with no orientation.
	 * 
	 * @param origin        Box origin.
	 * @param mins          Box min bounds.
	 * @param maxs          Box max bounds.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param a             Alpha (range 0 - 255)
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Box(const float origin[3], const float mins[3], const float maxs[3], int r, int g, int b, int a, const float duration = -1.0);
	/**
	 * Draws a swept box.
	 * 
	 * @param start         Start position.
	 * @param end           End position.
	 * @param mins          Box min bounds.
	 * @param maxs          Box max bounds.
	 * @param angles        Box angles.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param a             Alpha (range 0 - 255)
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void SweptBox(const float start[3], const float end[3], const float mins[3], const float maxs[3], const float angles[3], int r, int g, int b, int a, float duration = -1.0)
	/**
	 * Draws an entity's AABB bounds.
	 * 
	 * @param entity		Entity to draw.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param a             Alpha (range 0 - 255)
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void EntityBounds(int entity, int r, int g, int b, int a, float duration = -1.0);

	/**
	 * Draws a triangle.
	 * 
	 * @param p1			Triangle point 1.
	 * @param p2			Triangle point 2.
	 * @param p3			Triangle point 3.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param a             Alpha (range 0 - 255)
	 * @param noDepthTest	If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Triangle(const float p1[3], const float p2[3], const float p3[3], int r, int g, int b, int a, const bool noDepthTest, float duration = -1.0);
	/**
	 * Draws a world text.
	 * 
	 * @param origin		Text world position.
	 * @param viewCheck		If true, check for LOS before drawing.
	 * @param text			Text char array.
	 * @param duration		How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Text(const float origin[3], const bool viewCheck, const char[] text, float duration = -1.0);
	/**
	 * Draws a screen text.
	 * 
	 * @param xpos			Screen position X axis.
	 * @param ypos			Screen position Y axis.
	 * @param text			Text to draw.
	 * @param r             Red color (range: 0 - 255)
	 * @param g             Green color (range: 0 - 255)
	 * @param b             Blue color (range 0 - 255)
	 * @param a				Alpha (range 0 - 255)
	 * @param duration      How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void ScreenText(const float xpos, const float ypos, const char[] text, int r, int g, int b, int a, float duration = -1.0);
	/**
	 * Draws a colored 3D cross of the given size at the given position.
	 * 
	 * @param pos			Position.
	 * @parm size			Size of the cross.
	 * @param r				Red color (range: 0 - 255)
	 * @param g				Green color (range: 0 - 255)
	 * @param b				Blue color (range 0 - 255)
	 * @param noDepthTest	If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration		How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Cross3D(const float pos[3], const float size, int r, int g, int b, const bool noDepthTest, float duration = -1.0);
	/**
	 * Draws a horizontal arrow pointing in the specified direction.
	 * 
	 * @param start			Start position.
	 * @param end			End position.
	 * @param width			Arrow width.
	 * @param r				Red color (range: 0 - 255)
	 * @param g				Green color (range: 0 - 255)
	 * @param b				Blue color (range 0 - 255)
	 * @param a				Alpha (range 0 - 255)
	 * @param noDepthTest	If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration		How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void HorzArrow(const float start[3], const float end[3], const float width, int r, int g, int b, int a, bool noDepthTest, float duration = -1.0);
	/**
	 * Draws a vertical arrow pointing in the specified direction.
	 * 
	 * @param start			Start position.
	 * @param end			End position.
	 * @param width			Arrow width.
	 * @param r				Red color (range: 0 - 255)
	 * @param g				Green color (range: 0 - 255)
	 * @param b				Blue color (range 0 - 255)
	 * @param a				Alpha (range 0 - 255)
	 * @param noDepthTest	If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration		How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void VertArrow(const float start[3], const float end[3], const float width, int r, int g, int b, int a, bool noDepthTest, float duration = -1.0);
	/**
	 * Draws a sphere.
	 * 
	 * @param pos			Sphere position.
	 * @param radius		Sphere radius.
	 * @param r				Red color (range: 0 - 255)
	 * @param g				Green color (range: 0 - 255)
	 * @param b				Blue color (range 0 - 255)
	 * @param noDepthTest	If true, the overlay is rendered on top of everything, else it can get obstructed by world geometry.
	 * @param duration		How long to draw the overlay for. 0 or less for a single server frame.
	 */
	public static native void Sphere(const float pos[3], const float radius, int r, int g, int b, bool noDepthTest, float duration = -1.0);
}

