// ever_grow_reals.cpp
// Generates arbitrarily large finite decimal representations (strings) forever.
// NOT intended for any organization or violent activity — purely mathematical.
// Compile: g++ -std=c++17 ever_grow_reals.cpp -O2 -o ever_grow_reals
// Run: ./ever_grow_reals    (stop with Ctrl+C)

#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <limits>
#include <csignal>
#include <atomic>

std::atomic<bool> keep_running(true);

void signal_handler(int) {
    keep_running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Seed random engine with high-quality seed
    std::random_device rd;
    // Fallback in case random_device is deterministic on some platforms
    std::uint64_t seed = (static_cast<std::uint64_t>(rd()) << 32) ^ std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> digit_dist(0, 9);

    // Start parameters: initial digits after decimal, growth step
    std::size_t digits = 512;          // starting number of digits per cycle
    const std::size_t growth_step = 128; // increase digits by this many each cycle
    std::size_t cycle = 0;

    // For memory safety we will limit per-cycle allocation to something reasonable,
    // but the program is designed to run indefinitely and slowly increase digits.
    const std::size_t MAX_PER_CYCLE = 20'000'000; // 20 million digits limit per cycle (adjust as needed)

    std::cout << "Starting endless high-precision decimal generator.\n"
              << "Press Ctrl+C to stop.\n\n";

    while (keep_running) {
        ++cycle;
        if (digits > MAX_PER_CYCLE) {
            // If digits gets too big for one cycle, slow down growth so we don't OOM.
            digits = MAX_PER_CYCLE;
        }

        // Generate an integer part (random large integer with some digits)
        // We'll generate a random integer with 1..12 digits to avoid extremely long integer parts.
        int int_digits = 1 + (rng() % 12);
        std::string integer_part;
        integer_part.reserve(int_digits);
        for (int i = 0; i < int_digits; ++i) {
            int d = digit_dist(rng);
            // avoid leading zero
            if (i == 0 && d == 0) d = 1 + (rng() % 9);
            integer_part.push_back(char('0' + d));
        }

        // Generate fractional part with `digits` digits
        std::string frac_part;
        frac_part.reserve(digits);
        for (std::size_t i = 0; i < digits; ++i) {
            int d = digit_dist(rng);
            frac_part.push_back(char('0' + d));
        }

        // Optionally, add a random exponent to produce more variety (but still finite)
        int exponent = static_cast<int>(rng() % 2000) - 1000; // range [-1000,999]

        // Print a header for the cycle
        std::cout << "=== cycle " << cycle << " | digits=" << digits << " | exponent=" << exponent << " ===\n";
        // Print the decimal representation: integer.fraction e exponent
        std::cout << integer_part << '.' << frac_part << "e" << (exponent >= 0 ? "+" : "") << exponent << '\n' << std::flush;

        // Growth schedule: modestly increase `digits` each cycle
        // This keeps the program running for a long time without immediate exhaustion.
        if (digits + growth_step > digits) {
            digits += growth_step;
        }

        // Sleep a tiny bit to avoid busy-loop hogging of cores (adjust or remove if you want max throughput).
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "\nGraceful shutdown requested. Exiting after cycle " << cycle << ".\n";
    return 0;
}
