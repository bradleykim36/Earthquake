#ifndef QUAKEAPP_H_
#define QUAKEAPP_H_

#include <mingfx.h>
using namespace mingfx;

#include "earthquake_database.h"
#include "earth.h"

#include <string>
#include <vector>


/** Main application class for the Earthquake app.
 */
class QuakeApp : public GraphicsApp {
public:

    QuakeApp();
    virtual ~QuakeApp();

    /// Dragging with the mouse tilts the earth when in globe mode
    void OnLeftMouseDrag(const Point2& pos, const Vector2& delta);

    /// Pressing the globe button toggles between flat earth and sphere earth modes
    void OnGlobeBtnPressed();

    /// Pressing the debug button toggles on/off the underlying triangle mesh and normals
    void OnDebugBtnPressed();

    /// The slides controls the speed of the playback for the earthquakes animation
    void OnSliderUpdate(float value);


    /// The animation gets updated inside this function.
    void UpdateSimulation(double dt);

    /// Initializes NanoGUI widgets
    void InitNanoGUI();

    /// The models and textures get initialized in this function.
    void InitOpenGL();

    /// The earth and earthquake spheres get drawn in this function.
    void DrawUsingOpenGL();

private:
    // controls playback
    double current_time_;
    double playback_scale_;

    // true if drawing debugging info for the mesh
    bool debug_mode_;

    // Database through which you can access the earthquakes
    EarthquakeDatabase quake_db_;

    // Object for rendering textured earth geometry
    Earth earth_;

    // Background image
    Texture2D stars_tex_;

    // Sets up the computer graphics camera
    Matrix4 view_matrix_;
    Matrix4 proj_matrix_;

    // Gui elements
    nanogui::Button* globe_btn_;
    nanogui::Label* date_label_;
    nanogui::TextBox* speed_box_;

    // A list of paths to search for data files (images and shaders)
    std::vector<std::string> search_path_;

    // Used to draw a background texture, you can also use this to draw the
    // earthquakes if you want.
    QuickShapes quick_shapes_;
};

#endif
