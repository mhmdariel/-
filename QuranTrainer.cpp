#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Ayah {
    int surah = 0;
    int ayah = 0;
    std::string text; // UTF-8
};

struct Bookmark {
    int surah = 0;
    int ayah = 0;
    std::string note;
    std::string created_iso;
};

static std::string now_iso_utc() {
    using namespace std::chrono;
    auto t = system_clock::to_time_t(system_clock::now());
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

static std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) a++;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) b--;
    return s.substr(a, b - a);
}

static bool parse_ayah_line(const std::string& line, Ayah& out) {
    // format: surah|ayah|text
    auto p1 = line.find('|');
    if (p1 == std::string::npos) return false;
    auto p2 = line.find('|', p1 + 1);
    if (p2 == std::string::npos) return false;

    std::string s_s = trim(line.substr(0, p1));
    std::string s_a = trim(line.substr(p1 + 1, p2 - (p1 + 1)));
    std::string s_t = line.substr(p2 + 1);

    try {
        out.surah = std::stoi(s_s);
        out.ayah  = std::stoi(s_a);
        out.text  = trim(s_t);
        return out.surah > 0 && out.ayah > 0 && !out.text.empty();
    } catch (...) {
        return false;
    }
}

static std::vector<Ayah> load_quran(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Could not open quran file: " + path);
    }
    std::vector<Ayah> v;
    std::string line;
    size_t line_no = 0;
    while (std::getline(in, line)) {
        line_no++;
        line = trim(line);
        if (line.empty()) continue;
        Ayah a;
        if (!parse_ayah_line(line, a)) {
            std::ostringstream oss;
            oss << "Bad line format at line " << line_no << ". Expected: surah|ayah|text";
            throw std::runtime_error(oss.str());
        }
        v.push_back(std::move(a));
    }
    if (v.empty()) throw std::runtime_error("quran file is empty.");
    return v;
}

static std::string key_of(int surah, int ayah) {
    return std::to_string(surah) + ":" + std::to_string(ayah);
}

struct Progress {
    struct Item { int score = 0; std::string last_iso; };
    std::unordered_map<std::string, Item> items;

    int last_surah = 1;
    int last_ayah  = 1;
};

static void save_progress(const std::string& path, const Progress& p) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) throw std::runtime_error("Could not write progress file: " + path);

    out << "LAST|" << p.last_surah << "|" << p.last_ayah << "\n";
    for (const auto& kv : p.items) {
        out << "ITEM|" << kv.first << "|" << kv.second.score << "|" << kv.second.last_iso << "\n";
    }
}

static Progress load_progress(const std::string& path) {
    Progress p;
    std::ifstream in(path);
    if (!in) return p; // first run
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts;
        {
            std::stringstream ss(line);
            std::string tok;
            while (std::getline(ss, tok, '|')) parts.push_back(tok);
        }

        if (parts.size() >= 3 && parts[0] == "LAST") {
            try {
                p.last_surah = std::stoi(parts[1]);
                p.last_ayah  = std::stoi(parts[2]);
            } catch (...) {}
        } else if (parts.size() >= 4 && parts[0] == "ITEM") {
            try {
                Progress::Item it;
                it.score = std::stoi(parts[2]);
                it.last_iso = parts[3];
                p.items[parts[1]] = it;
            } catch (...) {}
        }
    }
    return p;
}

static void save_bookmarks(const std::string& path, const std::vector<Bookmark>& bms) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) throw std::runtime_error("Could not write bookmarks file: " + path);
    for (const auto& b : bms) {
        out << b.surah << "|" << b.ayah << "|" << b.created_iso << "|" << b.note << "\n";
    }
}

static std::vector<Bookmark> load_bookmarks(const std::string& path) {
    std::vector<Bookmark> bms;
    std::ifstream in(path);
    if (!in) return bms;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string tok;
        while (std::getline(ss, tok, '|')) parts.push_back(tok);
        if (parts.size() < 4) continue;

        try {
            Bookmark b;
            b.surah = std::stoi(parts[0]);
            b.ayah  = std::stoi(parts[1]);
            b.created_iso = parts[2];

            std::string note = parts[3];
            for (size_t i = 4; i < parts.size(); i++) {
                note += "|";
                note += parts[i];
            }
            b.note = note;

            bms.push_back(std::move(b));
        } catch (...) {}
    }
    return bms;
}

static std::optional<size_t> find_index(const std::vector<Ayah>& q, int surah, int ayah) {
    for (size_t i = 0; i < q.size(); i++) {
        if (q[i].surah == surah && q[i].ayah == ayah) return i;
    }
    return std::nullopt;
}

static std::vector<size_t> search_text(const std::vector<Ayah>& q, const std::string& needle) {
    std::vector<size_t> hits;
    if (needle.empty()) return hits;

    for (size_t i = 0; i < q.size(); i++) {
        if (q[i].text.find(needle) != std::string::npos) hits.push_back(i);
    }
    return hits;
}

static void print_ayah(const Ayah& a) {
    std::cout << "[" << a.surah << ":" << a.ayah << "] " << a.text << "\n";
}

static void print_tajweed_quick_tips() {
    std::cout << "\nTajweed quick tips (non-exhaustive):\n";
    std::cout << "  • Make wudu' if possible; read with calm and respect.\n";
    std::cout << "  • Slow down: clarity > speed. (Tarteel)\n";
    std::cout << "  • Listen to a qualified reciter to model makharij (letter exits).\n";
    std::cout << "  • If unsure, mark it and ask an imam/teacher later.\n\n";
}

static int read_int(const std::string& prompt, int minv, int maxv) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        s = trim(s);
        try {
            int v = std::stoi(s);
            if (v >= minv && v <= maxv) return v;
        } catch (...) {}
        std::cout << "Please enter a number between " << minv << " and " << maxv << ".\n";
    }
}

static std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static std::vector<size_t> build_review_queue(
    const std::vector<Ayah>& q,
    const Progress& prog,
    size_t max_items)
{
    struct Cand { size_t idx; int score; };
    std::vector<Cand> cands;
    cands.reserve(prog.items.size());

    for (const auto& kv : prog.items) {
        auto pos = kv.first.find(':');
        if (pos == std::string::npos) continue;
        int s = 0, a = 0;
        try {
            s = std::stoi(kv.first.substr(0, pos));
            a = std::stoi(kv.first.substr(pos + 1));
        } catch (...) { continue; }

        auto idx = find_index(q, s, a);
        if (!idx) continue;
        cands.push_back({*idx, kv.second.score});
    }

    std::sort(cands.begin(), cands.end(), [](const Cand& x, const Cand& y) {
        return x.score < y.score;
    });

    std::vector<size_t> out;
    for (size_t i = 0; i < cands.size() && out.size() < max_items; i++) {
        out.push_back(cands[i].idx);
    }
    return out;
}

static void run_reading_session(std::vector<Ayah>& q, Progress& prog, int verses) {
    size_t start_idx = 0;
    if (auto idx = find_index(q, prog.last_surah, prog.last_ayah)) start_idx = *idx;

    std::cout << "\nReading session: " << verses << " ayat\n";
    print_tajweed_quick_tips();

    for (int i = 0; i < verses; i++) {
        size_t idx = start_idx + static_cast<size_t>(i);
        if (idx >= q.size()) break;

        const auto& a = q[idx];
        print_ayah(a);

        auto k = key_of(a.surah, a.ayah);
        auto& it = prog.items[k];
        it.last_iso = now_iso_utc();

        prog.last_surah = a.surah;
        prog.last_ayah  = a.ayah;
    }
    std::cout << "\nSession complete. Tip: Read translation/tafsir separately if you have access.\n";
}

static void run_review(std::vector<Ayah>& q, Progress& prog, size_t max_items) {
    auto queue = build_review_queue(q, prog, max_items);
    if (queue.empty()) {
        std::cout << "\nNo review items yet. Do a reading session first.\n";
        return;
    }

    std::cout << "\nReview session (" << queue.size() << " items)\n";
    std::cout << "For each ayah: try to recall it (or its theme) before revealing.\n\n";

    for (size_t idx : queue) {
        const auto& a = q[idx];
        std::cout << "Ayah reference: [" << a.surah << ":" << a.ayah << "]\n";
        std::cout << "Press Enter to reveal text...";
        std::string dummy; std::getline(std::cin, dummy);

        print_ayah(a);

        std::cout << "Did you recall/recite it well? (y/n): ";
        std::string ans; std::getline(std::cin, ans);
        bool ok = !ans.empty() && (ans[0] == 'y' || ans[0] == 'Y');

        auto k = key_of(a.surah, a.ayah);
        auto& it = prog.items[k];
        it.last_iso = now_iso_utc();
        if (ok) it.score = std::min(it.score + 1, 10);
        else    it.score = std::max(it.score - 2, -10);

        std::cout << "Score now: " << it.score << "\n\n";
    }
}

static void run_quiz(std::vector<Ayah>& q, Progress& prog, int questions) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, q.size() - 1);

    std::cout << "\nQuiz (" << questions << " questions)\n";
    std::cout << "You’ll be shown an ayah. Answer the surah number and ayah number.\n\n";

    int correct = 0;
    for (int i = 0; i < questions; i++) {
        const auto& a = q[dist(rng)];
        print_ayah(a);

        int s = read_int("Surah number? ", 1, 114);
        int v = read_int("Ayah number?  ", 1, 300);

        bool ok = (s == a.surah && v == a.ayah);
        if (ok) {
            std::cout << "Correct.\n\n";
            correct++;
        } else {
            std::cout << "Not quite. Correct is [" << a.surah << ":" << a.ayah << "].\n\n";
        }

        auto k = key_of(a.surah, a.ayah);
        auto& it = prog.items[k];
        it.last_iso = now_iso_utc();
        if (ok) it.score = std::min(it.score + 2, 10);
        else    it.score = std::max(it.score - 1, -10);
    }

    std::cout << "Score: " << correct << "/" << questions << "\n";
}

static void add_bookmark(const Ayah& a, std::vector<Bookmark>& bms) {
    Bookmark b;
    b.surah = a.surah;
    b.ayah  = a.ayah;
    b.created_iso = now_iso_utc();
    b.note = read_line("Add a note (optional): ");
    bms.push_back(std::move(b));
    std::cout << "Saved bookmark.\n";
}

static void list_bookmarks(const std::vector<Ayah>& q, const std::vector<Bookmark>& bms) {
    if (bms.empty()) {
        std::cout << "\nNo bookmarks.\n";
        return;
    }
    std::cout << "\nBookmarks:\n";
    for (size_t i = 0; i < bms.size(); i++) {
        const auto& b = bms[i];
        std::cout << i + 1 << ") [" << b.surah << ":" << b.ayah << "] " << b.created_iso;
        if (!trim(b.note).empty()) std::cout << " — " << b.note;
        std::cout << "\n";

        if (auto idx = find_index(q, b.surah, b.ayah)) {
            std::cout << "    ";
            print_ayah(q[*idx]);
        }
    }
}

static void goto_and_read(const std::vector<Ayah>& q, Progress& prog, std::vector<Bookmark>& bms) {
    int s = read_int("Surah number (1-114): ", 1, 114);
    int a = read_int("Ayah number: ", 1, 300);

    auto idx = find_index(q, s, a);
    if (!idx) {
        std::cout << "Not found in your quran-uthmani.txt.\n";
        return;
    }
    const auto& ay = q[*idx];
    print_ayah(ay);

    prog.last_surah = s;
    prog.last_ayah  = a;
    prog.items[key_of(s, a)].last_iso = now_iso_utc();

    std::cout << "Bookmark this ayah? (y/n): ";
    std::string ans; std::getline(std::cin, ans);
    if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
        add_bookmark(ay, bms);
    }
}

static void search_menu(const std::vector<Ayah>& q, Progress& prog, std::vector<Bookmark>& bms) {
    std::string needle = read_line("Search text (Arabic or English if your file includes it): ");
    needle = trim(needle);

    auto hits = search_text(q, needle);
    if (hits.empty()) {
        std::cout << "No matches.\n";
        return;
    }

    std::cout << "Found " << hits.size() << " matches. Showing up to 10:\n\n";
    size_t show = std::min<size_t>(10, hits.size());
    for (size_t i = 0; i < show; i++) {
        const auto& a = q[hits[i]];
        std::cout << (i + 1) << ") ";
        print_ayah(a);
    }

    int pick = read_int("Open which result? (0 to cancel): ", 0, static_cast<int>(show));
    if (pick == 0) return;

    const auto& ay = q[hits[static_cast<size_t>(pick - 1)]];
    print_ayah(ay);

    prog.last_surah = ay.surah;
    prog.last_ayah  = ay.ayah;
    prog.items[key_of(ay.surah, ay.ayah)].last_iso = now_iso_utc();

    std::cout << "Bookmark this ayah? (y/n): ";
    std::string ans; std::getline(std::cin, ans);
    if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
        add_bookmark(ay, bms);
    }
}

static void print_mission_friendly_plan() {
    std::cout << "\nMission-friendly plan (example):\n";
    std::cout << "  • 10 min/day: read 5–10 ayat with tartil.\n";
    std::cout << "  • 3 min/day: review 3 previously-read ayat.\n";
    std::cout << "  • 1x/week: longer session + listen to recitation.\n";
    std::cout << "  • Keep notes on: vocabulary, themes, and du'a you can make from ayat.\n\n";
}

int main() {
    // UPDATED: use quran-uthmani.txt
    const std::string quran_path = "quran-uthmani.txt";
    const std::string progress_path = "progress.db";
    const std::string bookmarks_path = "bookmarks.db";

    try {
        auto q = load_quran(quran_path);
        auto prog = load_progress(progress_path);
        auto bms  = load_bookmarks(bookmarks_path);

        std::cout << "Qur'an Trainer (offline)\n";
        std::cout << "Loaded " << q.size() << " ayat from " << quran_path << "\n";
        std::cout << "Last position: [" << prog.last_surah << ":" << prog.last_ayah << "]\n";
        print_mission_friendly_plan();

        while (true) {
            std::cout << "\nMenu:\n";
            std::cout << "  1) Daily reading session\n";
            std::cout << "  2) Review session (spaced repetition)\n";
            std::cout << "  3) Quiz (identify reference)\n";
            std::cout << "  4) Go to Surah/Ayah\n";
            std::cout << "  5) Search\n";
            std::cout << "  6) List bookmarks\n";
            std::cout << "  7) Tajweed quick tips\n";
            std::cout << "  0) Save & exit\n";

            int choice = read_int("Choose: ", 0, 7);
            if (choice == 0) break;

            switch (choice) {
                case 1: {
                    int verses = read_int("How many ayat this session? (1-50): ", 1, 50);
                    run_reading_session(q, prog, verses);
                    break;
                }
                case 2: {
                    int n = read_int("How many review items? (1-20): ", 1, 20);
                    run_review(q, prog, static_cast<size_t>(n));
                    break;
                }
                case 3: {
                    int n = read_int("How many questions? (1-20): ", 1, 20);
                    run_quiz(q, prog, n);
                    break;
                }
                case 4:
                    goto_and_read(q, prog, bms);
                    break;
                case 5:
                    search_menu(q, prog, bms);
                    break;
                case 6:
                    list_bookmarks(q, bms);
                    break;
                case 7:
                    print_tajweed_quick_tips();
                    break;
                default:
                    break;
            }
        }

        save_progress(progress_path, prog);
        save_bookmarks(bookmarks_path, bms);

        std::cout << "Saved progress to " << progress_path
                  << " and bookmarks to " << bookmarks_path << ".\n";
        std::cout << "Assalamu 'alaykum.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        // UPDATED message:
        std::cerr << "Make sure you have quran-uthmani.txt in the same folder as the program.\n";
        return 1;
    }
}
