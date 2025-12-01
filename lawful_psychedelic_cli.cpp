#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <random>
#include <limits>
#include <fstream>

// ---------------------- Core Data Structures -----------------------------

struct State {
    double clarity          = 0.0;  // 0.0 - 1.0
    double compassion       = 0.0;  // 0.0 - 1.0
    double discipline       = 0.0;  // 0.0 - 1.0
    double groundedness     = 0.0;  // 0.0 - 1.0
    double openness         = 0.0;  // 0.0 - 1.0
    double psychedelicDepth = 0.0;  // 0.0 - 1.0
    double altitude         = 0.0;  // monotone ascent

    bool fullyCredentialed  = false;     // symbolic only
    std::string authorityTag = "NONE";   // symbolic only

    void normalize() {
        auto clamp01 = [](double v) {
            if (v < 0.0) return 0.0;
            if (v > 1.0) return 1.0;
            return v;
        };

        clarity          = clamp01(clarity);
        compassion       = clamp01(compassion);
        discipline       = clamp01(discipline);
        groundedness     = clamp01(groundedness);
        openness         = clamp01(openness);
        psychedelicDepth = clamp01(psychedelicDepth);
        // altitude intentionally not clamped
    }
};

struct Delta {
    double dClarity          = 0.0;
    double dCompassion       = 0.0;
    double dDiscipline       = 0.0;
    double dGroundedness     = 0.0;
    double dOpenness         = 0.0;
    double dPsychedelicDepth = 0.0;
    double dAltitude         = 0.0;
};

// ---------------------- Lawful Constraints -------------------------------

class Law {
public:
    virtual ~Law() = default;
    virtual bool is_allowed(const State& current, const Delta& delta) const = 0;
    virtual std::string name() const = 0;
};

class BalancedAscentLaw : public Law {
public:
    bool is_allowed(const State& s, const Delta& d) const override {
        if (d.dAltitude > 0.0) {
            double stability = (s.discipline + d.dDiscipline)
                             + (s.groundedness + d.dGroundedness);
            if (s.altitude + d.dAltitude > stability * 10.0) {
                return false;
            }
        }
        return true;
    }

    std::string name() const override {
        return "BalancedAscentLaw";
    }
};

class NoUngroundedDropLaw : public Law {
    double minGrounded_;
public:
    explicit NoUngroundedDropLaw(double minGrounded) : minGrounded_(minGrounded) {}

    bool is_allowed(const State& s, const Delta& d) const override {
        double newGround = s.groundedness + d.dGroundedness;
        if (newGround < minGrounded_) {
            return false;
        }
        return true;
    }

    std::string name() const override {
        return "NoUngroundedDropLaw";
    }
};

// Quran-inspired symbolic parameters (tunable)
class QuranInspiredLaw : public Law {
    double psyThreshold_;
    double stabilityThreshold_;
    double openClarGap_;
public:
    QuranInspiredLaw(double psyThreshold,
                     double stabilityThreshold,
                     double openClarGap)
        : psyThreshold_(psyThreshold),
          stabilityThreshold_(stabilityThreshold),
          openClarGap_(openClarGap) {}

    bool is_allowed(const State& s, const Delta& d) const override {
        double newPsy   = s.psychedelicDepth + d.dPsychedelicDepth;
        double newDisc  = s.discipline + d.dDiscipline;
        double newGround= s.groundedness + d.dGroundedness;

        // 1) High psychedelic depth requires high stability.
        if (newPsy > psyThreshold_ && (newDisc + newGround) < stabilityThreshold_) {
            return false;
        }

        // 2) Do not reduce compassion.
        if (s.compassion + d.dCompassion < s.compassion) {
            return false;
        }

        // 3) Openness must not massively outpace clarity.
        double newOpen = s.openness + d.dOpenness;
        double newClar = s.clarity + d.dClarity;
        if (newOpen > newClar + openClarGap_) {
            return false;
        }

        return true;
    }

    std::string name() const override {
        return "QuranInspiredLaw (symbolic)";
    }
};

class CompositeLaw : public Law {
    std::vector<std::unique_ptr<Law>> laws_;
public:
    void add_law(std::unique_ptr<Law> law) {
        laws_.push_back(std::move(law));
    }

    bool is_allowed(const State& s, const Delta& d) const override {
        for (const auto& law : laws_) {
            if (!law->is_allowed(s, d)) {
                return false;
            }
        }
        return true;
    }

    std::string name() const override {
        return "CompositeLaw";
    }
};

// ---------------------- Transformations ----------------------------------

using Transformation = std::function<Delta(const State&)>;

Transformation disciplined_ascent(double magnitude) {
    return [magnitude](const State& s) -> Delta {
        (void)s;
        Delta d;
        d.dAltitude         = magnitude;
        d.dClarity          = magnitude * 0.06;
        d.dCompassion       = magnitude * 0.06;
        d.dDiscipline       = magnitude * 0.08;
        d.dGroundedness     = magnitude * 0.08;
        d.dOpenness         = magnitude * 0.05;
        d.dPsychedelicDepth = magnitude * 0.03;
        return d;
    };
}

Transformation contemplative_stillness(double depth) {
    return [depth](const State& s) -> Delta {
        (void)s;
        Delta d;
        d.dClarity      = depth * 0.1;
        d.dGroundedness = depth * 0.1;
        d.dDiscipline   = depth * 0.05;
        d.dAltitude     = depth * 0.01;
        return d;
    };
}

Transformation psychedelic_expansion(double intensity) {
    return [intensity](const State& s) -> Delta {
        (void)s;
        Delta d;
        d.dClarity          = intensity * 0.05;
        d.dCompassion       = intensity * 0.05;
        d.dOpenness         = intensity * 0.1;
        d.dPsychedelicDepth = intensity * 0.15;
        d.dAltitude         = intensity * 0.2;
        return d;
    };
}

// ---------------------- Engine (with logging) ----------------------------

class Engine {
public:
    Engine(State initial, std::unique_ptr<Law> law)
        : state_(initial), law_(std::move(law))
    {
        std::random_device rd;
        rng_.seed(rd());
    }

    void set_logger(const std::string& filename) {
        log_.open(filename);
        if (!log_) {
            std::cerr << "Failed to open log file: " << filename << "\n";
        } else {
            log_ << "# Lawful Psychedelic Ascent Log (symbolic)\n";
            log_state("INITIAL");
        }
    }

    void add_transformation(const std::string& name, const Transformation& t) {
        names_.push_back(name);
        transforms_.push_back(t);
    }

    void apply_index(std::size_t idx, bool verbose = true, int step = -1) {
        if (idx >= transforms_.size()) {
            std::cout << "Invalid transformation index.\n";
            return;
        }
        const std::string& tname = names_[idx];
        const Transformation& t = transforms_[idx];
        Delta delta = t(state_);

        if (law_->is_allowed(state_, delta)) {
            apply(delta);
            if (verbose) {
                std::cout << "✔ Applied lawful transformation: "
                          << tname << "\n";
            }
            log_state("APPLIED: " + tname, step);
        } else {
            if (verbose) {
                std::cout << "✖ Transformation blocked by law ("
                          << law_->name() << "): " << tname << "\n";
            }
            log_state("BLOCKED: " + tname, step);
        }
    }

    void run_random_steps(std::size_t steps, bool verbose = true) {
        if (transforms_.empty()) {
            std::cout << "No transformations registered.\n";
            return;
        }
        std::uniform_int_distribution<std::size_t> dist(0, transforms_.size() - 1);
        for (std::size_t i = 0; i < steps; ++i) {
            std::size_t idx = dist(rng_);
            if (verbose) {
                std::cout << "[STEP " << i << "] ";
            }
            apply_index(idx, verbose, static_cast<int>(i));
        }
    }

    void print_state() const {
        std::cout << "------------------------------\n";
        std::cout << "Current symbolic state:\n";
        std::cout << "  clarity          = " << state_.clarity          << "\n";
        std::cout << "  compassion       = " << state_.compassion       << "\n";
        std::cout << "  discipline       = " << state_.discipline       << "\n";
        std::cout << "  groundedness     = " << state_.groundedness     << "\n";
        std::cout << "  openness         = " << state_.openness         << "\n";
        std::cout << "  psychedelicDepth = " << state_.psychedelicDepth << "\n";
        std::cout << "  altitude         = " << state_.altitude         << "\n";
        std::cout << "  fullyCredentialed (symbolic) = "
                  << (state_.fullyCredentialed ? "true" : "false") << "\n";
        std::cout << "  authorityTag (symbolic)      = "
                  << state_.authorityTag << "\n";
        std::cout << "------------------------------\n";
    }

    void symbolically_grant_credentials() {
        state_.fullyCredentialed = true;
        state_.authorityTag = "Symbolic-Model-Only";
        std::cout << "Symbolic credentials flag set inside the model.\n";
        log_state("CREDENTIALS_SET");
    }

    const State& current_state() const { return state_; }

    void list_transformations() const {
        std::cout << "Available transformations:\n";
        for (std::size_t i = 0; i < names_.size(); ++i) {
            std::cout << "  [" << i << "] " << names_[i] << "\n";
        }
    }

private:
    State state_;
    std::unique_ptr<Law> law_;
    std::vector<std::string> names_;
    std::vector<Transformation> transforms_;
    std::mt19937 rng_;
    std::ofstream log_;

    void apply(const Delta& d) {
        state_.clarity          += d.dClarity;
        state_.compassion       += d.dCompassion;
        state_.discipline       += d.dDiscipline;
        state_.groundedness     += d.dGroundedness;
        state_.openness         += d.dOpenness;
        state_.psychedelicDepth += d.dPsychedelicDepth;

        double newAlt = state_.altitude + d.dAltitude;
        if (newAlt > state_.altitude) {
            state_.altitude = newAlt;
        }

        state_.normalize();
    }

    void log_state(const std::string& label, int step = -1) {
        if (!log_) return;
        if (step >= 0) {
            log_ << "STEP " << step << " | " << label << "\n";
        } else {
            log_ << "STEP -1 | " << label << "\n";
        }
        log_ << "  clarity="          << state_.clarity
             << " compassion="        << state_.compassion
             << " discipline="        << state_.discipline
             << " groundedness="      << state_.groundedness
             << " openness="          << state_.openness
             << " psychedelicDepth="  << state_.psychedelicDepth
             << " altitude="          << state_.altitude
             << " fullyCredentialed=" << (state_.fullyCredentialed ? 1 : 0)
             << " authorityTag="      << state_.authorityTag
             << "\n";
    }
};

// ---------------------- CLI Helpers --------------------------------------

void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_menu() {
    std::cout << "\n=== Lawful Psychedelic Ascent (Symbolic CLI) ===\n";
    std::cout << "1. Show current state\n";
    std::cout << "2. List transformations\n";
    std::cout << "3. Apply transformation by index\n";
    std::cout << "4. Run random lawful ascent steps\n";
    std::cout << "5. Symbolically set 'credentials' flag\n";
    std::cout << "0. Quit\n";
    std::cout << "Choose: ";
}

// ---------------------- Config & Arg Parsing -----------------------------

struct Config {
    std::size_t steps = 0;             // 0 => interactive mode
    std::string logFile;

    double qPsyThreshold       = 0.7;
    double qStabilityThreshold = 1.4;
    double qOpenClarGap        = 0.5;
};

bool parse_double(const char* s, double& out) {
    try {
        out = std::stod(s);
        return true;
    } catch (...) {
        return false;
    }
}

bool parse_size(const char* s, std::size_t& out) {
    try {
        long long v = std::stoll(s);
        if (v < 0) return false;
        out = static_cast<std::size_t>(v);
        return true;
    } catch (...) {
        return false;
    }
}

Config parse_args(int argc, char** argv) {
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--steps" && i + 1 < argc) {
            std::size_t v;
            if (parse_size(argv[i + 1], v)) {
                cfg.steps = v;
            } else {
                std::cerr << "Invalid value for --steps: " << argv[i + 1] << "\n";
            }
            ++i;
        } else if (arg == "--log" && i + 1 < argc) {
            cfg.logFile = argv[i + 1];
            ++i;
        } else if (arg == "--qpsy-threshold" && i + 1 < argc) {
            double v;
            if (parse_double(argv[i + 1], v)) {
                cfg.qPsyThreshold = v;
            } else {
                std::cerr << "Invalid value for --qpsy-threshold: " << argv[i + 1] << "\n";
            }
            ++i;
        } else if (arg == "--qstability-threshold" && i + 1 < argc) {
            double v;
            if (parse_double(argv[i + 1], v)) {
                cfg.qStabilityThreshold = v;
            } else {
                std::cerr << "Invalid value for --qstability-threshold: " << argv[i + 1] << "\n";
            }
            ++i;
        } else if (arg == "--qopen-clarity-gap" && i + 1 < argc) {
            double v;
            if (parse_double(argv[i + 1], v)) {
                cfg.qOpenClarGap = v;
            } else {
                std::cerr << "Invalid value for --qopen-clarity-gap: " << argv[i + 1] << "\n";
            }
            ++i;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
        }
    }
    return cfg;
}

// ---------------------- Main ---------------------------------------------

int main(int argc, char** argv) {
    Config cfg = parse_args(argc, argv);

    // Initial state
    State initial;
    initial.clarity          = 0.4;
    initial.compassion       = 0.4;
    initial.discipline       = 0.6;
    initial.groundedness     = 0.7;
    initial.openness         = 0.3;
    initial.psychedelicDepth = 0.1;
    initial.altitude         = 0.0;
    initial.fullyCredentialed = false;
    initial.authorityTag      = "NONE";

    auto composite = std::make_unique<CompositeLaw>();
    composite->add_law(std::make_unique<BalancedAscentLaw>());
    composite->add_law(std::make_unique<NoUngroundedDropLaw>(0.4));
    composite->add_law(std::make_unique<QuranInspiredLaw>(
        cfg.qPsyThreshold,
        cfg.qStabilityThreshold,
        cfg.qOpenClarGap
    ));

    Engine engine(initial, std::move(composite));

    // Logging
    if (!cfg.logFile.empty()) {
        engine.set_logger(cfg.logFile);
    }

    // Register transformations
    engine.add_transformation("disciplined_ascent(0.3)", disciplined_ascent(0.3));
    engine.add_transformation("disciplined_ascent(0.6)", disciplined_ascent(0.6));
    engine.add_transformation("contemplative_stillness(0.5)", contemplative_stillness(0.5));
    engine.add_transformation("psychedelic_expansion(0.4)", psychedelic_expansion(0.4));

    // If steps > 0 => non-interactive mode
    if (cfg.steps > 0) {
        std::cout << "Running " << cfg.steps
                  << " random lawful steps (non-interactive).\n";
        engine.run_random_steps(cfg.steps, true);
        std::cout << "Final state:\n";
        engine.print_state();
        return 0;
    }

    // Interactive menu mode
    int choice = -1;
    while (true) {
        print_menu();
        if (!(std::cin >> choice)) {
            clear_input();
            continue;
        }

        if (choice == 0) {
            std::cout << "Exiting.\n";
            break;
        }

        switch (choice) {
            case 1: {
                engine.print_state();
                break;
            }
            case 2: {
                engine.list_transformations();
                break;
            }
            case 3: {
                engine.list_transformations();
                std::cout << "Enter index: ";
                std::size_t idx;
                if (!(std::cin >> idx)) {
                    clear_input();
                    std::cout << "Invalid index input.\n";
                    break;
                }
                engine.apply_index(idx, true);
                break;
            }
            case 4: {
                std::cout << "How many random steps? ";
                std::size_t steps;
                if (!(std::cin >> steps)) {
                    clear_input();
                    std::cout << "Invalid steps.\n";
                    break;
                }
                engine.run_random_steps(steps, true);
                break;
            }
            case 5: {
                engine.symbolically_grant_credentials();
                break;
            }
            default:
                std::cout << "Unknown choice.\n";
        }
    }

    return 0;
}
