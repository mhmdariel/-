// quran_reader.cpp
// Respectful Qur'anic reader/printer with optional TTS hook.
// Usage:
//   Compile: g++ -std=c++17 -O2 -o quran_reader quran_reader.cpp
//   Run:     ./quran_reader quran.txt           (prints to stdout)
//   Speak:   ./quran_reader quran.txt --speak  (prints and attempts TTS per line)
//
// Note: Provide a UTF-8 encoded Qur'an file (e.g., an Uthmani-text file).
//       This program does not embed the Qur'an itself to avoid copying large sacred text here.

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
  #include <windows.h>
#endif

// A small helper to run an external TTS command for a single line.
// This implementation uses system() to call a command-line TTS program.
// You should replace or adapt it for your platform (SAPI on Windows, espeak-ng or gtts-cli on Linux/macOS).
static void speakLine(const std::string &utf8line) {
    if (utf8line.empty()) return;

    // Example attempts (comment/uncomment according to your environment).
    // 1) Linux: espeak-ng (may need proper Arabic voice tag). Example voice tags vary by installation.
    //    std::string cmd = "espeak-ng -v ar \"" + utf8line + "\" 2>/dev/null";
    // 2) Linux: use gtts-cli (python) and mpg123 (requires piping): slower but often clearer:
    //    std::string cmd = "python3 -c \"from gtts import gTTS; t=gTTS('" + utf8line + "', lang='ar'); t.save('/tmp/qtemp.mp3')\" && mpg123 -q /tmp/qtemp.mp3";
    // 3) macOS: say (may not support Arabic properly):
    //    std::string cmd = "say \"" + utf8line + "\"";
    // 4) Windows: you would implement SAPI instead of system() calls.
    //
    // By default, we try a harmless, no-op placeholder so the program is portable.
    // Uncomment/replace one of the above implementations for actual TTS.

    // --- Safe default: do nothing (user must enable a TTS command below) ---
    // return;

    // Example (uncomment if espeak-ng is installed and supports Arabic on your system):
    // std::string cmd = "espeak-ng -v ar \"" + utf8line + "\" 2>/dev/null";
    // std::system(cmd.c_str());
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <quran-file.txt> [--speak]\n";
        return 1;
    }

    std::string filename = argv[1];
    bool doSpeak = false;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--speak" || a == "-s") doSpeak = true;
    }

    // Print respectful Qur'anic invocations first
    // (Using well-known Qur'anic short phrases and the Shahāda)
    std::cout << u8"بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ\n"; // Bismillah
    std::cout << u8"ٱلْحَمْدُ لِلَّهِ رَبِّ ٱلْعَٰلَمِينَ\n"; // Al-Fātiḥa opening praise
    std::cout << u8"أَشْهَدُ أَنْ لَا إِلَٰهَ إِلَّا ٱللَّٰهُ وَأَشْهَدُ أَنَّ مُحَمَّدًا رَسُولُ ٱللَّهِ\n\n"; // Shahāda
    std::cout << "----- Begin Qur'an content from file: " << filename << " -----\n\n";

    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "Error opening file: " << filename << "\n";
        return 2;
    }

    // Read line-by-line as UTF-8, print each line.
    // If spoken requested, call speakLine() on each non-empty line.
    std::string line;
    while (std::getline(in, line)) {
        // Trim trailing carriage return if present (Windows CRLF)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Print the line exactly as in file (so Uthmani orthography remains).
        std::cout << line << '\n';

        if (doSpeak) {
            // Attempt to speak the line (user must enable/adjust speakLine implementation)
            speakLine(line);
        }
    }

    std::cout << "\n----- End of file -----\n";
    std::cout << u8"سُبْحَانَ اللَّهِ وَبِحَمْدِهِ، سُبْحَانَ اللَّهِ الْعَظِيمِ\n"; // a short dhikr
    in.close();
    return 0;
}
