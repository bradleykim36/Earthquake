#include "quake_app.h"
#include "config.h"

#include <iostream>
#include <sstream>

 // for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>

 // Number of seconds in 1 year (approx.)
const int PLAYBACK_WINDOW = 12 * 28 * 24 * 60 * 60;

using namespace std;

std::vector<int> earthquakeVector;

QuakeApp::QuakeApp() : GraphicsApp(1280, 720, "Earthquake"),
playback_scale_(15000000.0), debug_mode_(false)
{
    // Define a search path for finding data files (images and earthquake db)x
    search_path_.push_back(".");
    search_path_.push_back("./data");
    search_path_.push_back(DATA_DIR_INSTALL);
    search_path_.push_back(DATA_DIR_BUILD);

    quake_db_ = EarthquakeDatabase(Platform::FindFile("earthquakes.txt", search_path_));
    current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();

}


QuakeApp::~QuakeApp() {
}


void QuakeApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window* window = new nanogui::Window(screen(), "Earthquake Controls");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(400, 200));
    window->setLayout(new nanogui::GroupLayout());

    date_label_ = new nanogui::Label(window, "Current Date: MM/DD/YYYY", "sans-bold");

    globe_btn_ = new nanogui::Button(window, "Globe");
    globe_btn_->setCallback(std::bind(&QuakeApp::OnGlobeBtnPressed, this));
    globe_btn_->setTooltip("Toggle between map and globe.");

    new nanogui::Label(window, "Playback Speed", "sans-bold");

    nanogui::Widget* panel = new nanogui::Widget(window);
    panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    nanogui::Slider* slider = new nanogui::Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(120);

    speed_box_ = new nanogui::TextBox(panel);
    speed_box_->setFixedSize(Eigen::Vector2i(60, 25));
    speed_box_->setValue("50");
    speed_box_->setUnits("%");
    slider->setCallback(std::bind(&QuakeApp::OnSliderUpdate, this, std::placeholders::_1));
    speed_box_->setFixedSize(Eigen::Vector2i(60, 25));
    speed_box_->setFontSize(20);
    speed_box_->setAlignment(nanogui::TextBox::Alignment::Right);

    nanogui::Button* debug_btn = new nanogui::Button(window, "Toggle Debug Mode");
    debug_btn->setCallback(std::bind(&QuakeApp::OnDebugBtnPressed, this));
    debug_btn->setTooltip("Toggle displaying mesh triangles and normals (can be slow)");

    screen()->performLayout();
}

void QuakeApp::OnLeftMouseDrag(const Point2& pos, const Vector2& delta) {
    // Optional: In our demo, we adjust the tilt of the globe here when the
    // mouse is dragged up/down on the screen.
}

float buttonPress = 0.0;
void QuakeApp::OnGlobeBtnPressed() {
    // TODO: This is where you can switch between flat earth mode and globe mode
    buttonPress = 1.0 - buttonPress;
}

void QuakeApp::OnDebugBtnPressed() {
    debug_mode_ = !debug_mode_;
}

void QuakeApp::OnSliderUpdate(float value) {
    speed_box_->setValue(std::to_string((int)(value * 100)));
    playback_scale_ = 30000000.0 * value;
}


float lerp = 0;
float movement = 0;
void QuakeApp::UpdateSimulation(double dt) {
    // Advance the current time and loop back to the start if time is past the last earthquake
    current_time_ += playback_scale_ * dt;
    if (current_time_ > quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
    }
    if (current_time_ < quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds();
    }

    Date d(current_time_);
    stringstream s;
    s << "Current date: " << d.month()
        << "/" << d.day()
        << "/" << d.year();
    date_label_->setCaption(s.str());

    // TODO: Any animation, morphing, rotation of the earth, or other things that should
    // be updated once each frame would go here.
    int recentQuake = quake_db_.FindMostRecentQuake(current_time_);
    earthquakeVector.clear();
    while (d.SecondsUntil(quake_db_.earthquake(recentQuake).date()) <= PLAYBACK_WINDOW && recentQuake >= quake_db_.min_index()) {
        earthquakeVector.push_back(recentQuake--);
    }
    if (buttonPress < lerp) {
        lerp -= dt / 2;
    }
    if (buttonPress > lerp) {
        lerp += dt / 2;
    }

    earth_.LerpHelper(lerp);
    float temp = (movement + ((float)dt / 4)) * sqrt(sqrt((float)fmax(0, lerp)));
    movement = fmodf(temp, float(2 * (float) M_PI));
}


void QuakeApp::InitOpenGL() {
    // Set up the camera in a good position to see the entire earth in either mode
    proj_matrix_ = Matrix4::Perspective(60.0f, aspect_ratio(), 0.1f, 50.0f);
    view_matrix_ = Matrix4::LookAt(Point3(0, 0, 3.5), Point3(0, 0, 0), Vector3(0, 1, 0));
    glClearColor(0.0, 0.0, 0.0, 1);

    // Initialize the earth object
    earth_.Init(search_path_);

    // Initialize the texture used for the background image
    stars_tex_.InitFromFile(Platform::FindFile("iss006e40544.png", search_path_));
}


void QuakeApp::DrawUsingOpenGL() {
    quick_shapes_.DrawFullscreenTexture(Color(1, 1, 1), stars_tex_);

    // You can leave this as the identity matrix and we will have a fine view of
    // the earth.  If you want to add any rotation or other animation of the
    // earth, the model_matrix is where you would apply that.
    Matrix4 model_matrix = Matrix4::RotationY(movement);
    // Draw the earth
    earth_.Draw(model_matrix, view_matrix_, proj_matrix_);
    if (debug_mode_) {
        earth_.DrawDebugInfo(model_matrix, view_matrix_, proj_matrix_);
    }

    // TODO: You'll also need to draw the earthquakes.  It's up to you exactly
    // how you wish to do that.
    Date d(current_time_);
    for (int i : earthquakeVector) {
        Earthquake singleEarthquake = quake_db_.earthquake(i);
        Point3 earthquakePlane = earth_.LatLongToPlane(singleEarthquake.latitude(), singleEarthquake.longitude());
        Point3 earthquakeSphere = earth_.LatLongToSphere(singleEarthquake.latitude(), singleEarthquake.longitude());
        Point3 position = earthquakePlane.Lerp(earthquakeSphere, lerp);

        float squared = ((float)singleEarthquake.magnitude() - quake_db_.min_magnitude()) * ((float)singleEarthquake.magnitude() - quake_db_.min_magnitude());
        float earthquakeSize = squared / quake_db_.max_magnitude();

        float earthquakeTime = d.SecondsUntil(singleEarthquake.date()) / PLAYBACK_WINDOW;
        float sphere_size = 0.07 * earthquakeSize *  earthquakeTime;

        Matrix4 earthquakeMatrix4 = model_matrix * Matrix4::Translation(position - Point3(0, 0, 0)) * Matrix4::Scale(Vector3(sphere_size, sphere_size, sphere_size));
        quick_shapes_.DrawSphere(earthquakeMatrix4, view_matrix_, proj_matrix_, Color(1.0, 1.0 - earthquakeSize, 0.0));
    }
}
