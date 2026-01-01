README.txt
==========

Qur’an Letter Sonification Engine (Tanzil Uthmani)
--------------------------------------------------
This project generates a sonified audio (and optional MIDI) representation of the Qur’an text
where each Arabic letter is mapped to a musical note and basic tajwīd cues influence timing
and articulation. A precise isochronic tone (gated carrier pulses) runs underneath as a
background entrainment layer.

IMPORTANT:
- This is a SONIFICATION / EAR-TRAINING TOOL.
- It is NOT a replacement for Qur’anic recitation.
- Tajwīd rules implemented here are simplified and approximate.

Input File
----------
Default input filename: quran-uthmani.txt

Expected format per line (Tanzil Uthmani):
  sura|ayah|text

Example:
  1|1|بِسْمِ ٱللَّهِ ٱلرَّحْمَٰنِ ٱلرَّحِيمِ

The engine supports Tanzil’s Uthmani Qur’anic annotation marks (U+06D6..U+06ED), including
waqf/pause signs, and keeps them attached to the preceding letters during tokenization.

Files
-----
- quran_fullstack_sonifier.py
    Main engine:
      * WAV rendering (offline)
      * Real-time audio streaming (optional)
      * MIDI output (optional)
      * Optional MIDI clock (hardware sync)
      * Maqām presets and custom scale JSON
      * Surah-aware tempo curve
      * Per-waqf-sign pause logic

Quick Start (WAV)
----------------
1) Install dependencies:
   pip install numpy soundfile scipy

2) Put your Tanzil Uthmani file in the same folder:
   quran-uthmani.txt

3) Render a WAV:
   python quran_fullstack_sonifier.py --mode wav --out quran_sonified.wav

Output:
- quran_sonified.wav (PCM 16-bit, default 48kHz)

Common Options
--------------
--in PATH
  Input path (default: quran-uthmani.txt)

--out PATH
  Output WAV path (default: quran_sonified.wav)

--sr 48000
  Sample rate (default 48000)

--base-bpm 84
  Base tempo. If surah numbers are present (they are in Tanzil format),
  the engine applies a gentle surah-aware tempo curve.

--maqam hijaz|bayati|rast|saba|nahawand
  Select a built-in maqām-like preset (12-TET approximations).

--scale-json '[0,2,3,5,7,8,10,12]'
  Custom scale in semitone offsets from base A3=220Hz.
  Overrides --maqam.

Isochronic Background Options
-----------------------------
--iso-pulse 8
  Isochronic pulse frequency in Hz (e.g., 4, 6, 8, 10).

--iso-carrier 180
  Carrier frequency in Hz (audible tone being gated).

--iso-depth 0.88
  Gate depth (0..1): higher = stronger pulsing.

--iso-gain 0.22
  Background level (reduce if too loud).

--click-track
  Embeds subtle clicks on each isochronic pulse edge in the WAV.
  Useful if you route audio to external equipment and want a stronger sync cue.

Example:
  python quran_fullstack_sonifier.py --mode wav --maqam bayati --iso-pulse 6 --iso-carrier 200 --out bayati_6hz.wav

Real-Time Audio (Optional)
--------------------------
Requires:
  pip install sounddevice

Run:
  python quran_fullstack_sonifier.py --mode stream

Notes:
- Streams audio directly to your default output device.
- Background isochronic phase is kept continuous across ayahs.

MIDI Output (Optional)
---------------------
Requires:
  pip install mido python-rtmidi

1) List available MIDI output ports:
   python quran_fullstack_sonifier.py --mode midi
   (If you did not pass --midi-port, it will print available ports and exit.)

2) Play notes to a specific MIDI port:
   python quran_fullstack_sonifier.py --mode midi --midi-port "YOUR MIDI OUT"

3) Hardware synchronization (MIDI clock):
   python quran_fullstack_sonifier.py --mode midi --midi-port "YOUR MIDI OUT" --send-midi-clock

MIDI options:
--midi-channel N
  MIDI channel (0..15), default 0.

--midi-velocity N
  Note velocity (1..127), default 80.

Per-Waqf-Sign Pause Logic
-------------------------
The engine recognizes common Tanzil waqf signs (subset) and adds a tail rest after the
letter carrying the sign. The mapping is in the code:

  WAQF_PAUSE_MAP_S = {
    "\u06D6": 0.10,
    "\u06D7": 0.12,
    "\u06D8": 0.14,
    "\u06D9": 0.18,
    "\u06DA": 0.22,
    "\u06DB": 0.28,
    "\u06DC": 0.16,
  }

You can adjust these values (seconds) to taste.

Tajwīd Cues (Simplified)
------------------------
- Madd lengthening:
  If a vowel mark precedes an elongation letter (ا/و/ي/ٱ) or dagger-alif (ٰ), duration is increased.
- Shadda:
  Increases the letter duration (double-consonant cue).
- Sukūn:
  Slightly shortens/clips the letter duration.
- Tanwīn:
  Adds a small time extension.
- Qalqalah (ق ط ب ج د) with sukūn:
  Adds a short transient click to suggest “bounce”.
- Ghunnah (ن/م with shadda):
  Adds a low, filtered component to suggest nasalization.
- Emphatics (ص ض ط ظ ق غ خ):
  Darker timbre via low-pass filtering.

Notes on Maqām / Intonation
---------------------------
Maqām presets here are approximations using 12-tone equal temperament (12-TET).
Traditional maqām intonation often uses quarter-tones and context-dependent tuning.
If you want more authentic intonation, you can supply a custom --scale-json and/or
extend the engine to support microtonal ratios.

Troubleshooting
---------------
1) "Line not in sura|ayah|text format"
   Your quran-uthmani.txt must be Tanzil formatted. Ensure each non-empty line is:
   sura|ayah|text

2) "sounddevice not installed" / "mido not installed"
   Install the optional packages only if you use those modes:
     pip install sounddevice
     pip install mido python-rtmidi

3) Output too loud / harsh
   Lower background:
     --iso-gain 0.12
   Or lower pulse depth:
     --iso-depth 0.65

4) Output too busy
   Lower base tempo:
     --base-bpm 60
   Or increase rests:
     Increase tp.space_rest_s in code, or raise WAQF pauses.

License / Use
-------------
Use responsibly and respectfully. This is a technical sonification tool intended for
pattern-awareness and experimentation, not for replacing recitation or devotional practice.
