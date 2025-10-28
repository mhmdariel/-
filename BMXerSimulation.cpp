/*

بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ

(كَانَ اللَّهُ وَلَا يَزَالُ)



// الْكُودُ الْقُدْسِيُّ الَّذِي لَا يَنْتَهِي

شَرْعٌ فِي نُطْقِ الأَزَلِ:



أَمْرٌ رَبَّانِيٌّ:

  أَطْلِقْ خَلْقَ النُّورِ الأَصْلَبِ

  حَوِّلْ كُلَّ يَوْمِ الدِّينِ نُورًا

  اجْعَلْهُ الأَكْثَفَ الأَعْلَى

  نُورًا عَلَى نُورٍ

  سَلَامًا عَلَى سَلَامٍ

  أَحْسَنَ عَلَى أَحْسَنَ

  وَأَدِيمْهُ إِلَى أَنْ لَا نِهَايَةَ



حَلْقَةٌ لَا تَنْقَطِعُ:

  أَظْهِرْ نُورَ الْحَقِّ الْمَلِكِ

  أَثْبِتْ كَلِمَاتِ رَبِّكَ الْعَلِيِّ

  أَجْرِهَا حَقَّ تُجْرَاهَا

  وَاصْرِفْ عَنْهَا كُلَّ حَدٍّ وَمِيقَاتٍ



(سُبْحَانَ رَبِّكَ رَبِّ الْعِزَّةِ عَمَّا يَصِفُونَ * وَسَلَامٌ عَلَى الْمُرْسَلِينَ * وَالْحَمْدُ لِلَّهِ رَبِّ الْعَالَمِينَ)

---------------------------------------------------------------------------

  BMXerSimulation.cpp

  -------------------

  A respectful symbolic physics simulation (C++17) modeling a BMX rider.

  All spiritual references are purely symbolic and artistic.

*/



#include <bits/stdc++.h>

#include <locale>

using namespace std;



// Physical constants

constexpr double g = 9.80665;



// Simulation configuration

struct SimConfig {

    double dt = 0.005;

    double air_drag_coeff = 0.02;

    double rider_mass = 75.0;

    double max_sim_time = 10.0;

};



// Ramp geometry

struct Ramp {

    double x0, y0;

    double dx, dy;

    double length;

    bool containsX(double x, double &y) const {

        if (fabs(dx) < 1e-9) return false;

        double t = (x - x0) / dx;

        if (t < -1e-6 || t > 1.0 + 1e-6) return false;

        y = y0 + t * dy;

        return true;

    }

    double angle() const { return atan2(dy, dx); }

};



// State of the BMXer

struct State {

    double t, x, y, vx, vy;

};



// Apply drag

void apply_drag(double vx, double vy, double drag_coeff, double &ax, double &ay) {

    double v = hypot(vx, vy);

    if (v < 1e-9) return;

    double drag = drag_coeff * v * v;

    ax -= drag * (vx / v);

    ay -= drag * (vy / v);

}



// Trajectory simulation

pair<bool, double> simulateTrajectory(

    const SimConfig &cfg,

    double launchX, double launchY,

    double speed, double angleRad,

    const Ramp &landingRamp,

    const Ramp &takeoffRamp,

    vector<State> &trajectory,

    double landing_tolerance = 0.5,

    double landing_angle_tolerance_deg = 20.0

) {

    double vx = speed * cos(angleRad);

    double vy = speed * sin(angleRad);

    double x = launchX, y = launchY, t = 0.0;

    double apex = y;

    trajectory.clear();

    trajectory.push_back({t, x, y, vx, vy});



    while (t < cfg.max_sim_time && y > -1000.0) {

        double ax = 0.0, ay = -g;

        apply_drag(vx, vy, cfg.air_drag_coeff, ax, ay);

        double vx_mid = vx + ax * (cfg.dt / 2.0);

        double vy_mid = vy + ay * (cfg.dt / 2.0);

        double ax_mid = 0.0, ay_mid = -g;

        apply_drag(vx_mid, vy_mid, cfg.air_drag_coeff, ax_mid, ay_mid);

        x += vx_mid * cfg.dt;

        y += vy_mid * cfg.dt;

        vx += ax_mid * cfg.dt;

        vy += ay_mid * cfg.dt;

        t += cfg.dt;

        trajectory.push_back({t, x, y, vx, vy});

        apex = max(apex, y);



        double rampY;

        if (landingRamp.containsX(x, rampY) && y <= rampY + 1e-6) {

            double velAngle = atan2(vy, vx);

            double rampAngle = landingRamp.angle();

            double angleDiffDeg = fabs((velAngle - rampAngle) * 180.0 / M_PI);

            while (angleDiffDeg > 180.0) angleDiffDeg -= 360.0;

            angleDiffDeg = fabs(angleDiffDeg);

            double posError = fabs(y - rampY);

            if (posError <= landing_tolerance && angleDiffDeg <= landing_angle_tolerance_deg)

                return {true, apex};

            else

                return {false, apex};

        }

    }

    return {false, apex};

}



int main() {

    ios::sync_with_stdio(false);

    cin.tie(nullptr);

    std::locale::global(std::locale(""));



    // عرض النص العربي التأملي

    const char* sacred_text = u8R"(بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ

(كَانَ اللَّهُ وَلَا يَزَالُ)



// الْكُودُ الْقُدْسِيُّ الَّذِي لَا يَنْتَهِي

...)";

    cout << sacred_text << "\n\n";



    // تعريف القمة الرمزية

    const char* highest_apex =

        u8"الدار السلام المتعالي الصبور الوارث السلام في الله الملك الحق";



    cout << "BMXer Virtual Launch Simulation\n";

    cout << "Symbolic Highest Apex: " << highest_apex << "\n\n";



    // إعداد المحاكاة

    SimConfig cfg;

    cfg.dt = 0.004;

    cfg.air_drag_coeff = 0.015;

    cfg.max_sim_time = 8.0;



    Ramp takeoff{0.0, 0.0, 3.0, 0.6, hypot(3.0, 0.6)};

    Ramp landing{22.0, -0.5, 4.0, 0.3, hypot(4.0, 0.3)};



    double bestApex = -1e9, bestSpeed = 0.0, bestAngleDeg = 0.0;

    vector<State> bestTrajectory;



    for (double speed = 6.0; speed <= 28.0; speed += 0.6) {

        for (double angleDeg = 8.0; angleDeg <= 65.0; angleDeg += 1.2) {

            double angleRad = angleDeg * M_PI / 180.0;

            vector<State> traj;

            auto res = simulateTrajectory(cfg, takeoff.x0 + takeoff.dx, takeoff.y0 + takeoff.dy,

                                          speed, angleRad, landing, takeoff, traj, 0.6, 22.0);

            if (res.first && res.second > bestApex) {

                bestApex = res.second;

                bestSpeed = speed;

                bestAngleDeg = angleDeg;

                bestTrajectory = traj;

            }

        }

    }



    cout << fixed << setprecision(3);

    if (bestApex < 0.0) {

        cout << "No clean landing trajectory found.\n";

    } else {

        cout << "Best simulated clean landing found:\n";

        cout << "  Launch speed: " << bestSpeed << " m/s\n";

        cout << "  Launch angle: " << bestAngleDeg << " degrees\n";

        cout << "  Achieved apex height: " << bestApex << " m\n";

    }



    cout << "\n(End of symbolic simulation — all meanings handled respectfully.)\n";

    return 0;

}
/*

بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ

(كَانَ اللَّهُ وَلَا يَزَالُ)

...

*/



#include <bits/stdc++.h>

#include <locale>

using namespace std;



// --- Physical constants ---

constexpr double g = 9.80665; // gravitational acceleration (m/s^2)



// --- Laminar flow symbolic equation ---

// Re = (rho * v * L) / mu

// Smoothness metric S = mu / (rho * v * L)  --> maximize S for smooth laminar flow

double laminarSmoothness(double rho, double v, double L, double mu) {

    if (rho <= 0 || v <= 0 || L <= 0 || mu <= 0) return 0.0;

    return mu / (rho * v * L);

}



// --- Simulation config, ramp, and physics (same as before) ---

struct SimConfig {

    double dt = 0.005;

    double air_drag_coeff = 0.02;

    double rider_mass = 75.0;

    double max_sim_time = 10.0;

};



struct Ramp {

    double x0, y0;

    double dx, dy;

    double length;

    bool containsX(double x, double &y) const {

        if (fabs(dx) < 1e-9) return false;

        double t = (x - x0) / dx;

        if (t < -1e-6 || t > 1.0 + 1e-6) return false;

        y = y0 + t * dy;

        return true;

    }

    double angle() const { return atan2(dy, dx); }

};



struct State {

    double t, x, y, vx, vy;

};



void apply_drag(double vx, double vy, double drag_coeff, double &ax, double &ay) {

    double v = hypot(vx, vy);

    if (v < 1e-9) return;

    double drag = drag_coeff * v * v;

    ax -= drag * (vx / v);

    ay -= drag * (vy / v);

}



pair<bool, double> simulateTrajectory(

    const SimConfig &cfg,

    double launchX, double launchY,

    double speed, double angleRad,

    const Ramp &landingRamp,

    const Ramp &takeoffRamp,

    vector<State> &trajectory,

    double landing_tolerance = 0.5,

    double landing_angle_tolerance_deg = 20.0

) {

    double vx = speed * cos(angleRad);

    double vy = speed * sin(angleRad);

    double x = launchX, y = launchY, t = 0.0;

    double apex = y;

    trajectory.clear();

    trajectory.push_back({t, x, y, vx, vy});



    while (t < cfg.max_sim_time && y > -1000.0) {

        double ax = 0.0, ay = -g;

        apply_drag(vx, vy, cfg.air_drag_coeff, ax, ay);

        double vx_mid = vx + ax * (cfg.dt / 2.0);

        double vy_mid = vy + ay * (cfg.dt / 2.0);

        double ax_mid = 0.0, ay_mid = -g;

        apply_drag(vx_mid, vy_mid, cfg.air_drag_coeff, ax_mid, ay_mid);

        x += vx_mid * cfg.dt;

        y += vy_mid * cfg.dt;

        vx += ax_mid * cfg.dt;

        vy += ay_mid * cfg.dt;

        t += cfg.dt;

        trajectory.push_back({t, x, y, vx, vy});

        apex = max(apex, y);



        double rampY;

        if (landingRamp.containsX(x, rampY) && y <= rampY + 1e-6) {

            double velAngle = atan2(vy, vx);

            double rampAngle = landingRamp.angle();

            double angleDiffDeg = fabs((velAngle - rampAngle) * 180.0 / M_PI);

            while (angleDiffDeg > 180.0) angleDiffDeg -= 360.0;

            angleDiffDeg = fabs(angleDiffDeg);

            double posError = fabs(y - rampY);

            if (posError <= landing_tolerance && angleDiffDeg <= landing_angle_tolerance_deg)

                return {true, apex};

            else

                return {false, apex};

        }

    }

    return {false, apex};

}



int main() {

    ios::sync_with_stdio(false);

    cin.tie(nullptr);

    std::locale::global(std::locale(""));



    const char* highest_apex =

        u8"الدار السلام المتعالي الصبور الوارث السلام في الله الملك الحق";



    cout << "بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ\n\n";

    cout << "BMXer Virtual Launch Simulation (Symbolic & Respectful)\n";

    cout << "Symbolic Highest Apex: " << highest_apex << "\n\n";



    // --- Laminar Flow Section ---

    double rho = 1.225;   // air density (kg/m³)

    double mu  = 1.81e-5; // dynamic viscosity of air (Pa·s)

    double v   = 10.0;    // representative flow speed (m/s)

    double L   = 0.5;     // characteristic length (m)

    double S = laminarSmoothness(rho, v, L, mu);



    cout << "Laminar Flow Smoothness Metric (S = μ / (ρ v L)):\n";

    cout << "  Air density (ρ): " << rho << " kg/m³\n";

    cout << "  Dynamic viscosity (μ): " << mu << " Pa·s\n";

    cout << "  Velocity (v): " << v << " m/s\n";

    cout << "  Length scale (L): " << L << " m\n";

    cout << "  Smoothness (S): " << S << " [dimensionless inverse Reynolds]\n\n";



    // --- BMX Simulation ---

    SimConfig cfg;

    cfg.dt = 0.004;

    cfg.air_drag_coeff = 0.015;

    cfg.max_sim_time = 8.0;



    Ramp takeoff{0.0, 0.0, 3.0, 0.6, hypot(3.0, 0.6)};

    Ramp landing{22.0, -0.5, 4.0, 0.3, hypot(4.0, 0.3)};



    double bestApex = -1e9, bestSpeed = 0.0, bestAngleDeg = 0.0;

    vector<State> bestTrajectory;



    for (double speed = 6.0; speed <= 28.0; speed += 0.6) {

        for (double angleDeg = 8.0; angleDeg <= 65.0; angleDeg += 1.2) {

            double angleRad = angleDeg * M_PI / 180.0;

            vector<State> traj;

            auto res = simulateTrajectory(cfg, takeoff.x0 + takeoff.dx, takeoff.y0 + takeoff.dy,

                                          speed, angleRad, landing, takeoff, traj, 0.6, 22.0);

            if (res.first && res.second > bestApex) {

                bestApex = res.second;

                bestSpeed = speed;

                bestAngleDeg = angleDeg;

                bestTrajectory = traj;

            }

        }

    }



    cout << fixed << setprecision(3);

    if (bestApex < 0.0) {

        cout << "No clean landing trajectory found.\n";

    } else {

        cout << "Best simulated clean landing:\n";

        cout << "  Launch speed: " << bestSpeed << " m/s\n";

        cout << "  Launch angle: " << bestAngleDeg << " degrees\n";

        cout << "  Achieved apex height: " << bestApex << " m\n";

    }



    cout << "\n(End of symbolic simulation — mathematical and respectful.)\n";

    return 0;

}
/*

  write_apex_result_bin.cpp

  -------------------------

  Runs the symbolic BMXer simulation (purely virtual / artistic) and, after the

  search, writes a binary file "apex_result.bin" that contains an ASCII "proof"

  block indicating whether a clean apex landing was found, plus a short UTF-8

  labeled payload (the Arabic symbolic apex label).



  Notes:

   - This program is a simulation/art piece only. It does NOT provide real-world

     instructions for stunts.

   - The "proof" written to the .bin file is an ASCII/textual record (human- and

     machine-readable) stored in a binary file. The file layout is described below.

   - Arabic text is UTF-8 and therefore not pure 7-bit ASCII; to respect your

     "ASCII proof" request we put the formal proof content in ASCII and include

     the Arabic label as an optional UTF-8 payload section.

   - Compile (C++17):

       g++ -std=c++17 -O2 write_apex_result_bin.cpp -o write_apex_result_bin

     Run:

       ./write_apex_result_bin



  Binary file layout (apex_result.bin):

    [ASCII header line]         e.g. "APEX_PROOF_v1\n"

    [ASCII status line]         e.g. "STATUS: SUCCESS\n" or "STATUS: FAILURE\n"

    [ASCII key=val lines]      numeric values (apex, speed, angle, timestamp)

    [ASCII separator line]      "---UTF8_LABEL---\n"

    [UTF-8 label bytes]         (optional) Arabic label in UTF-8 followed by newline

    [ASCII footer line]         "END\n"

*/



#include <bits/stdc++.h>

using namespace std;



/* --------------------

   Simple physics sim (same symbolic BMXer sim used previously)

   -------------------- */

constexpr double g = 9.80665;



struct SimConfig {

    double dt = 0.004;

    double air_drag_coeff = 0.015;

    double max_sim_time = 8.0;

};



struct Ramp {

    double x0, y0;

    double dx, dy;

    double length;

    bool containsX(double x, double &y) const {

        if (fabs(dx) < 1e-9) return false;

        double t = (x - x0) / dx;

        if (t < -1e-6 || t > 1.0 + 1e-6) return false;

        y = y0 + t * dy;

        return true;

    }

    double angle() const { return atan2(dy, dx); }

};



struct State {

    double t, x, y, vx, vy;

};



void apply_drag(double vx, double vy, double drag_coeff, double &ax, double &ay) {

    double v = hypot(vx, vy);

    if (v < 1e-9) return;

    double drag = drag_coeff * v * v;

    ax -= drag * (vx / v);

    ay -= drag * (vy / v);

}



pair<bool,double> simulateTrajectory(

    const SimConfig &cfg,

    double launchX, double launchY,

    double speed, double angleRad,

    const Ramp &landingRamp,

    const Ramp &takeoffRamp,

    vector<State> &trajectory,

    double landing_tolerance = 0.6,

    double landing_angle_tolerance_deg = 22.0

) {

    double vx = speed * cos(angleRad);

    double vy = speed * sin(angleRad);

    double x = launchX, y = launchY, t = 0.0;

    double apex = y;

    trajectory.clear();

    trajectory.push_back({t,x,y,vx,vy});



    while (t < cfg.max_sim_time && y > -1000.0) {

        double ax = 0.0, ay = -g;

        apply_drag(vx, vy, cfg.air_drag_coeff, ax, ay);



        double vx_mid = vx + ax * (cfg.dt / 2.0);

        double vy_mid = vy + ay * (cfg.dt / 2.0);

        double ax_mid = 0.0, ay_mid = -g;

        apply_drag(vx_mid, vy_mid, cfg.air_drag_coeff, ax_mid, ay_mid);



        x += vx_mid * cfg.dt;

        y += vy_mid * cfg.dt;

        vx += ax_mid * cfg.dt;

        vy += ay_mid * cfg.dt;

        t += cfg.dt;

        trajectory.push_back({t,x,y,vx,vy});

        apex = max(apex, y);



        double rampY;

        if (landingRamp.containsX(x, rampY) && y <= rampY + 1e-6) {

            double velAngle = atan2(vy, vx);

            double rampAngle = landingRamp.angle();

            double angleDiffDeg = fabs((velAngle - rampAngle) * 180.0 / M_PI);

            while (angleDiffDeg > 180.0) angleDiffDeg -= 360.0;

            angleDiffDeg = fabs(angleDiffDeg);

            double posError = fabs(y - rampY);

            if (posError <= landing_tolerance && angleDiffDeg <= landing_angle_tolerance_deg) {

                return {true, apex};

            } else {

                return {false, apex};

            }

        }

    }

    return {false, apex};

}



/* --------------------

   Helper: get ISO 8601 timestamp (UTC)

   -------------------- */

string current_timestamp_iso8601_utc() {

    using chrono::system_clock;

    auto now = system_clock::now();

    time_t tt = system_clock::to_time_t(now);

    // gmtime_r/gmtime_s for thread-safe; fallback to gmtime

    struct tm gmt{};

#if defined(_WIN32) || defined(_WIN64)

    gmtime_s(&gmt, &tt);

#else

    gmtime_r(&tt, &gmt);

#endif

    char buf[64];

    // YYYY-MM-DDTHH:MM:SSZ

    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &gmt);

    return string(buf);

}



/* --------------------

   Main: run search, then write apex_result.bin

   -------------------- */

int main() {

    ios::sync_with_stdio(false);

    cin.tie(nullptr);



    // Symbolic UTF-8 Arabic highest apex label (not ASCII)

    const string highest_apex_utf8 = u8"الدار السلام المتعالي الصبور الوارث السلام في الله الملك الحق";



    // Simulation setup

    SimConfig cfg;

    Ramp takeoff{0.0, 0.0, 3.0, 0.6, hypot(3.0,0.6)};

    Ramp landing{22.0, -0.5, 4.0, 0.3, hypot(4.0,0.3)};



    double bestApex = -1e9;

    double bestSpeed = 0.0;

    double bestAngleDeg = 0.0;

    vector<State> bestTrajectory;



    // grid search (coarse)

    for (double speed = 6.0; speed <= 28.0; speed += 0.6) {

        for (double angleDeg = 8.0; angleDeg <= 65.0; angleDeg += 1.2) {

            double angleRad = angleDeg * M_PI / 180.0;

            vector<State> traj;

            auto res = simulateTrajectory(cfg, takeoff.x0 + takeoff.dx, takeoff.y0 + takeoff.dy,

                                          speed, angleRad, landing, takeoff, traj, 0.6, 22.0);

            if (res.first && res.second > bestApex) {

                bestApex = res.second;

                bestSpeed = speed;

                bestAngleDeg = angleDeg;

                bestTrajectory = move(traj);

            }

        }

    }



    // Prepare ASCII "proof" content

    // Note: ASCII only; we avoid embedding non-ASCII bytes here.

    string header = "APEX_PROOF_v1\n";

    bool success = bestApex > -1e8; // found a successful clean landing in the search

    string statusLine = string("STATUS: ") + (success ? "SUCCESS\n" : "FAILURE\n");



    // Numeric detail lines in ASCII

    ostringstream oss;

    oss << fixed << setprecision(6);

    oss << "TIMESTAMP_UTC: " << current_timestamp_iso8601_utc() << "\n";

    oss << "SIMULATION: BMXerSymbolic\n";

    oss << "LAUNCH_POS_X: " << (takeoff.x0 + takeoff.dx) << "\n";

    oss << "LAUNCH_POS_Y: " << (takeoff.y0 + takeoff.dy) << "\n";

    if (success) {

        oss << "BEST_LAUNCH_SPEED_m_s: " << bestSpeed << "\n";

        oss << "BEST_LAUNCH_ANGLE_deg: " << bestAngleDeg << "\n";

        oss << "ACHIEVED_APEX_m: " << bestApex << "\n";

        // Optionally include sample of trajectory points (a few ASCII lines)

        oss << "TRAJECTORY_SAMPLE_COUNT: " << min<size_t>(bestTrajectory.size(), 10) << "\n";

        for (size_t i = 0; i < bestTrajectory.size() && i < 10; ++i) {

            const State &s = bestTrajectory[i];

            oss << "P" << i << ": t=" << s.t << ",x=" << s.x << ",y=" << s.y

                << ",vx=" << s.vx << ",vy=" << s.vy << "\n";

        }

    } else {

        oss << "NOTE: No clean landing found within search grid and tolerances.\n";

    }



    // Separator and UTF-8 label section marker (ASCII)

    string separator = "---UTF8_LABEL---\n";

    string utf8_label = highest_apex_utf8 + "\n"; // UTF-8 bytes (non-ASCII)



    string footer = "END\n";



    // Now write binary file (but contents are textual bytes)

    const string filename = "apex_result.bin";

    ofstream out(filename, ios::binary);

    if (!out) {

        cerr << "Failed to open output file '" << filename << "' for writing.\n";

        return 2;

    }



    // Write sections in order: header, status, ascii payload, separator, utf8 label, footer

    out.write(header.data(), static_cast<streamsize>(header.size()));

    out.write(statusLine.data(), static_cast<streamsize>(statusLine.size()));

    string ascii_payload = oss.str();

    out.write(ascii_payload.data(), static_cast<streamsize>(ascii_payload.size()));

    out.write(separator.data(), static_cast<streamsize>(separator.size()));

    // UTF-8 label (may contain non-ASCII bytes), still fine in binary file

    out.write(utf8_label.data(), static_cast<streamsize>(utf8_label.size()));

    out.write(footer.data(), static_cast<streamsize>(footer.size()));

    out.close();



    // Print summary to console (ASCII)

    cout << "Wrote proof file: " << filename << "\n";

    cout << "Result: " << (success ? "SUCCESS - clean landing found" : "FAILURE - no clean landing") << "\n";

    cout << "Timestamp (UTC): " << current_timestamp_iso8601_utc() << "\n";

    cout << "\nContents (ASCII proof summary):\n";

    cout << header << statusLine << ascii_payload;

    cout << separator;

    cout << "(UTF-8 label follows in file, not displayed here)\n";

    cout << footer << "\n";



    return 0;

}
ﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ
ﷲﷲﷲﷲﷲﷲﷲﷲ