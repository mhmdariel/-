#!/usr/bin/env python3
# quran_fullstack_sonifier.py
# ------------------------------------------------------------
# Programmed specifically for: quran-uthmani.txt (Tanzil Uthmani)
# Expected file format per line:
#   sura|ayah|text
# Example:
#   1|1|بِسْمِ ٱللَّهِ ٱلرَّحْمَٰنِ ٱلرَّحِيمِ
#
# Outputs:
# - WAV render (default): quran_sonified.wav
# - Optional real-time audio stream
# - Optional MIDI notes + optional MIDI clock (hardware sync)
#
# INSTALL:
#   pip install numpy soundfile scipy
# OPTIONAL:
#   pip install sounddevice
#   pip install mido python-rtmidi
# ------------------------------------------------------------

from __future__ import annotations

import argparse
import json
import math
import re
import time
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

import numpy as np
import soundfile as sf
from scipy.signal import butter, lfilter

try:
    import sounddevice as sd  # type: ignore
except Exception:
    sd = None  # type: ignore

try:
    import mido  # type: ignore
except Exception:
    mido = None  # type: ignore


# =========================
# Tanzil Uthmani handling
# =========================

FATHA = "\u064E"
DAMMA = "\u064F"
KASRA = "\u0650"
SUKUN = "\u0652"
SHADDA = "\u0651"
TANWIN_FATH = "\u064B"
TANWIN_DAMM = "\u064C"
TANWIN_KASR = "\u064D"
MADDAH = "\u0653"
DAGGER_ALIF = "\u0670"
SMALL_WAW = "\u06E5"
SMALL_YA = "\u06E6"
TATWEEL = "\u0640"

ALIF = "\u0627"
WAW = "\u0648"
YA = "\u064A"

AR_LETTER_RE = re.compile(r"[\u0621-\u063A\u0641-\u064A\u0671-\u06D3]")

# Tanzil Uthmani Qur’anic annotation marks
QURANIC_MARKS_SET = set(chr(cp) for cp in range(0x06D6, 0x06ED + 1))

ARABIC_MARKS = {
    FATHA, DAMMA, KASRA, SUKUN, SHADDA,
    TANWIN_FATH, TANWIN_DAMM, TANWIN_KASR,
    MADDAH, DAGGER_ALIF, SMALL_WAW, SMALL_YA
} | QURANIC_MARKS_SET

# Per-waqf sign pause logic (Tanzil Uthmani)
WAQF_PAUSE_MAP_S: Dict[str, float] = {
    "\u06D6": 0.10,  # ۖ
    "\u06D7": 0.12,  # ۗ
    "\u06D8": 0.14,  # ۘ
    "\u06D9": 0.18,  # ۙ
    "\u06DA": 0.22,  # ۚ
    "\u06DB": 0.28,  # ۛ
    "\u06DC": 0.16,  # ۜ
}

# Tajwid groups (simplified)
QALQALAH = set("قطبجد")
NOON = "ن"
MEEM = "م"
EMPHATICS = set("صضطظقغخ")

# quran-uthmani.txt format: sura|ayah|text
TANZIL_LINE_RE = re.compile(r"^\s*(\d+)\|(\d+)\|(.*)\s*$")


@dataclass
class Token:
    letter: str
    marks: List[str]
    raw: str


@dataclass
class Ayah:
    surah: int
    ayah: int
    text: str


def read_quran_uthmani(path: str) -> List[Ayah]:
    """Strict reader for quran-uthmani.txt (must be sura|ayah|text lines)."""
    ayahs: List[Ayah] = []
    with open(path, "r", encoding="utf-8") as f:
        for ln, line in enumerate(f, start=1):
            line = line.rstrip("\n")
            if not line.strip():
                continue
            m = TANZIL_LINE_RE.match(line)
            if not m:
                raise ValueError(
                    f"Line {ln} not in 'sura|ayah|text' format. "
                    f"Example: 1|1|بِسْمِ ٱللَّهِ ..."
                )
            ayahs.append(Ayah(int(m.group(1)), int(m.group(2)), m.group(3)))
    if not ayahs:
        raise ValueError("No ayahs found. Is quran-uthmani.txt empty?")
    return ayahs


def tokenize_uthmani(text: str) -> List[Token]:
    text = text.replace(TATWEEL, "")
    tokens: List[Token] = []
    i = 0

    while i < len(text):
        ch = text[i]

        if AR_LETTER_RE.match(ch):
            marks: List[str] = []
            raw = ch
            j = i + 1
            while j < len(text) and text[j] in ARABIC_MARKS:
                marks.append(text[j])
                raw += text[j]
                j += 1
            tokens.append(Token(ch, marks, raw))
            i = j
            continue

        # If a Qur'anic mark appears alone, attach to previous token when possible
        if ch in ARABIC_MARKS and tokens and tokens[-1].letter != " ":
            tokens[-1].marks.append(ch)
            tokens[-1].raw += ch
            i += 1
            continue

        # whitespace/punctuation => rest
        if ch.isspace() or ch in "،.؛:!?\"'()[]{}«»":
            tokens.append(Token(" ", [], ch))
            i += 1
            continue

        # ignore other symbols
        i += 1

    return tokens


# =========================
# Musical scales / Maqām presets
# =========================

def semitone_ratio(n: float) -> float:
    return 2 ** (n / 12.0)

def build_scale_from_semitones(base_hz: float, semis: List[float], octaves: int = 2) -> List[float]:
    out: List[float] = []
    for o in range(octaves):
        for s in semis:
            out.append(base_hz * semitone_ratio(s + 12 * o))
    out.append(base_hz * semitone_ratio(12 * octaves))
    return out

# Approximate maqām-like presets in 12-TET (not quarter-tone accurate).
MAQAM_PRESETS: Dict[str, List[float]] = {
    "rast":  [0, 2, 4, 5, 7, 9, 11],
    "bayati":[0, 2, 3, 5, 7, 8, 10],
    "hijaz": [0, 1, 4, 5, 7, 8, 11],
    "saba":  [0, 1, 3, 5, 6, 8, 10],
    "nahawand":[0, 2, 3, 5, 7, 8, 10],
}

def make_letter_mapping(scale: List[float]) -> Dict[str, float]:
    letters = list("ءأإآؤئابتثجحخدذرزسشصضطظعغفقكلمنهةويىٱ")
    return {L: scale[i % len(scale)] for i, L in enumerate(letters)}


# =========================
# DSP utilities
# =========================

def butter_lowpass(cutoff: float, sr: int, order: int = 4):
    nyq = 0.5 * sr
    normal = min(0.99, max(1e-6, cutoff / nyq))
    return butter(order, normal, btype="low", analog=False)

def lowpass(x: np.ndarray, cutoff: float, sr: int) -> np.ndarray:
    b, a = butter_lowpass(cutoff, sr, 4)
    return lfilter(b, a, x).astype(np.float32)

def osc_sine(freq: float, dur_s: float, sr: int) -> np.ndarray:
    n = int(max(0.0, dur_s) * sr)
    if n <= 0:
        return np.zeros(0, dtype=np.float32)
    t = np.arange(n, dtype=np.float32) / sr
    return np.sin(2 * np.pi * freq * t).astype(np.float32)

def adsr(n: int, sr: int, a=0.01, d=0.06, s=0.70, r=0.10) -> np.ndarray:
    if n <= 0:
        return np.zeros(0, dtype=np.float32)
    A = int(a * sr)
    D = int(d * sr)
    R = int(r * sr)
    S = max(0, n - (A + D + R))
    env = np.zeros(n, dtype=np.float32)
    if A > 0:
        env[:A] = np.linspace(0, 1, A, endpoint=False)
    if D > 0:
        env[A:A+D] = np.linspace(1, s, D, endpoint=False)
    if S > 0:
        env[A+D:A+D+S] = s
    start = A + D + S
    if start < n:
        env[start:] = np.linspace(env[start-1] if start > 0 else s, 0, n - start)
    return env

def click(dur_s: float, sr: int, strength: float = 0.35) -> np.ndarray:
    n = int(max(0.0, dur_s) * sr)
    if n <= 0:
        return np.zeros(0, dtype=np.float32)
    m = min(n, max(16, int(0.008 * sr)))
    decay = np.exp(-np.linspace(0, 6, m)).astype(np.float32)
    x = np.zeros(n, dtype=np.float32)
    x[:m] = strength * decay
    return x

def normalize(x: np.ndarray, peak: float = 0.98) -> np.ndarray:
    m = float(np.max(np.abs(x))) if x.size else 0.0
    if m < 1e-9:
        return x.astype(np.float32)
    return (x * (peak / m)).astype(np.float32)


# =========================
# Tajwid timing (simplified)
# =========================

@dataclass
class TajwidParams:
    base_unit_s: float
    madd_counts: float = 4.0
    shadda_mult: float = 1.6
    sukun_mult: float = 0.9
    tanwin_extra_s: float = 0.05
    space_rest_s: float = 0.10

def estimate_duration(tok: Token, next_tok: Optional[Token], tp: TajwidParams) -> Tuple[float, float]:
    """
    Returns (sounded_duration, tail_silence)
    - tail_silence used for waqf signs (different pauses per sign).
    """
    if tok.letter == " ":
        return (0.0, tp.space_rest_s)

    dur = tp.base_unit_s
    tail = 0.0
    marks = set(tok.marks)

    if SHADDA in marks:
        dur *= tp.shadda_mult
    if SUKUN in marks:
        dur *= tp.sukun_mult

    if (TANWIN_FATH in marks) or (TANWIN_DAMM in marks) or (TANWIN_KASR in marks):
        dur += tp.tanwin_extra_s

    # Dagger alif => lengthening
    if DAGGER_ALIF in marks:
        dur = max(dur, tp.base_unit_s * tp.madd_counts)

    # Simplified madd: vowel + elongation letter next
    if next_tok and next_tok.letter in (ALIF, WAW, YA, "ٱ"):
        if (FATHA in marks) or (DAMMA in marks) or (KASRA in marks):
            dur = max(dur, tp.base_unit_s * tp.madd_counts)

    # Per-waqf-sign tail rest
    for mk in tok.marks:
        if mk in WAQF_PAUSE_MAP_S:
            tail = max(tail, WAQF_PAUSE_MAP_S[mk])

    return (float(dur), float(tail))


# =========================
# Isochronic tone + sync
# =========================

@dataclass
class IsoParams:
    pulse_hz: float = 8.0
    carrier_hz: float = 180.0
    depth: float = 0.88
    smooth_ms: float = 10.0
    gain: float = 0.22

def isochronic_track(total_dur_s: float, sr: int, ip: IsoParams) -> np.ndarray:
    n = int(total_dur_s * sr)
    if n <= 0:
        return np.zeros(0, dtype=np.float32)
    t = np.arange(n, dtype=np.float32) / sr
    carrier = np.sin(2 * np.pi * ip.carrier_hz * t).astype(np.float32)
    gate = (np.sin(2 * np.pi * ip.pulse_hz * t) > 0).astype(np.float32)

    w = max(1, int((ip.smooth_ms / 1000.0) * sr))
    if w > 1:
        kernel = np.ones(w, dtype=np.float32) / w
        gate = np.convolve(gate, kernel, mode="same").astype(np.float32)

    gate = (1.0 - ip.depth) + ip.depth * gate
    x = ip.gain * carrier * gate
    x = lowpass(x, cutoff=min(8000.0, ip.carrier_hz * 12), sr=sr)
    return x.astype(np.float32)

def bpm_to_midi_clock_interval_s(bpm: float) -> float:
    return (60.0 / max(1e-6, bpm)) / 24.0


# =========================
# Surah-aware tempo curve
# =========================

def surah_bpm(surah: int, base_bpm: float, spread: float = 18.0) -> float:
    if surah <= 0:
        return base_bpm
    u = (surah - 1) / 113.0
    return float(base_bpm + (0.5 - u) * spread)


# =========================
# Rendering audio (wav/stream)
# =========================

def render_voice_for_ayah(
    ayah: Ayah,
    sr: int,
    letter_map: Dict[str, float],
    bpm: float,
) -> np.ndarray:
    beat_s = 60.0 / max(1e-6, bpm)
    tp = TajwidParams(base_unit_s=beat_s / 3.0)

    tokens = tokenize_uthmani(ayah.text)
    chunks: List[np.ndarray] = []

    for i, tok in enumerate(tokens):
        nxt = tokens[i + 1] if i + 1 < len(tokens) else None
        sounded, tail = estimate_duration(tok, nxt, tp)

        if tok.letter == " ":
            chunks.append(np.zeros(int(tail * sr), dtype=np.float32))
            continue

        freq = letter_map.get(tok.letter, 220.0)

        x = osc_sine(freq, sounded, sr)
        x *= adsr(len(x), sr)

        marks = set(tok.marks)

        # Qalqalah cue: click/bounce when sukun present
        if tok.letter in QALQALAH and SUKUN in marks:
            x += click(min(0.03, sounded), sr, strength=0.26)

        # Ghunnah cue: noon/meem with shadda
        if tok.letter in (NOON, MEEM) and SHADDA in marks:
            nasal = osc_sine(freq * 0.5, sounded, sr) * 0.18
            nasal = lowpass(nasal, 600.0, sr)
            x += nasal

        # Emphatics: darker
        if tok.letter in EMPHATICS:
            x = lowpass(x, 1400.0, sr)

        # Append tail rest for waqf
        if tail > 0:
            x = np.concatenate([x, np.zeros(int(tail * sr), dtype=np.float32)])

        chunks.append((0.62 * x).astype(np.float32))

    return np.concatenate(chunks) if chunks else np.zeros(0, dtype=np.float32)


def build_scale_and_map(maqam: str, scale_json: Optional[str]) -> Dict[str, float]:
    base_hz = 220.0
    if scale_json:
        semis = json.loads(scale_json)
        if not isinstance(semis, list) or not all(isinstance(x, (int, float)) for x in semis):
            raise ValueError("--scale-json must be a JSON list of numbers (semitones).")
        scale = build_scale_from_semitones(base_hz, [float(x) for x in semis], octaves=2)
    else:
        semis = MAQAM_PRESETS.get(maqam.lower())
        if semis is None:
            raise ValueError(f"Unknown maqam '{maqam}'. Choose from: {', '.join(MAQAM_PRESETS)}")
        scale = build_scale_from_semitones(base_hz, semis, octaves=2)
    return make_letter_mapping(scale)


def render_wav_from_quran_uthmani(
    in_path: str = "quran-uthmani.txt",
    out_path: str = "quran_sonified.wav",
    sr: int = 48000,
    base_bpm: float = 84.0,
    maqam: str = "hijaz",
    scale_json: Optional[str] = None,
    iso: Optional[IsoParams] = None,
    click_track: bool = False
) -> None:
    ayahs = read_quran_uthmani(in_path)
    iso = iso or IsoParams()

    letter_map = build_scale_and_map(maqam, scale_json)

    parts: List[np.ndarray] = []
    for ay in ayahs:
        bpm = surah_bpm(ay.surah, base_bpm)
        parts.append(render_voice_for_ayah(ay, sr, letter_map, bpm))
        parts.append(np.zeros(int(0.05 * sr), dtype=np.float32))

    voice = np.concatenate(parts) if parts else np.zeros(0, dtype=np.float32)
    total_dur_s = len(voice) / sr

    bg = isochronic_track(total_dur_s, sr, iso)
    mix = voice + bg

    if click_track:
        pulse = max(0.5, float(iso.pulse_hz))
        interval = 1.0 / pulse
        click_len = int(0.01 * sr)
        for k in range(int(total_dur_s / interval)):
            idx = int(k * interval * sr)
            if idx + click_len < len(mix):
                mix[idx:idx+click_len] += click(0.01, sr, strength=0.12)

    mix = normalize(mix, 0.98)
    sf.write(out_path, mix, sr, subtype="PCM_16")


def stream_from_quran_uthmani(
    in_path: str = "quran-uthmani.txt",
    sr: int = 48000,
    base_bpm: float = 84.0,
    maqam: str = "hijaz",
    scale_json: Optional[str] = None,
    iso: Optional[IsoParams] = None
) -> None:
    if sd is None:
        raise RuntimeError("sounddevice not installed. Run: pip install sounddevice")

    ayahs = read_quran_uthmani(in_path)
    iso = iso or IsoParams()

    letter_map = build_scale_and_map(maqam, scale_json)

    t0 = 0.0
    def bg_chunk(dur_s: float) -> np.ndarray:
        nonlocal t0
        n = int(dur_s * sr)
        if n <= 0:
            return np.zeros(0, dtype=np.float32)
        t = (np.arange(n, dtype=np.float32) / sr) + t0
        carrier = np.sin(2 * np.pi * iso.carrier_hz * t).astype(np.float32)
        gate = (np.sin(2 * np.pi * iso.pulse_hz * t) > 0).astype(np.float32)
        w = max(1, int((iso.smooth_ms / 1000.0) * sr))
        if w > 1:
            kernel = np.ones(w, dtype=np.float32) / w
            gate = np.convolve(gate, kernel, mode="same").astype(np.float32)
        gate = (1.0 - iso.depth) + iso.depth * gate
        x = iso.gain * carrier * gate
        x = lowpass(x, cutoff=min(8000.0, iso.carrier_hz * 12), sr=sr)
        t0 += dur_s
        return x.astype(np.float32)

    with sd.OutputStream(samplerate=sr, channels=1, dtype="float32") as stream:
        for ay in ayahs:
            bpm = surah_bpm(ay.surah, base_bpm)
            voice = render_voice_for_ayah(ay, sr, letter_map, bpm)
            dur_s = len(voice) / sr
            bg = bg_chunk(dur_s)
            chunk = normalize(voice + bg, 0.9)
            stream.write(chunk.reshape(-1, 1))
            gap = np.zeros(int(0.03 * sr), dtype=np.float32)
            stream.write(gap.reshape(-1, 1))


# =========================
# MIDI output + MIDI clock sync
# =========================

def freq_to_midi_note(freq: float) -> int:
    if freq <= 0:
        return 60
    note = 69 + 12 * math.log2(freq / 440.0)
    return int(max(0, min(127, round(note))))

def midi_from_quran_uthmani(
    in_path: str,
    midi_port_name: str,
    base_bpm: float = 84.0,
    maqam: str = "hijaz",
    scale_json: Optional[str] = None,
    channel: int = 0,
    velocity: int = 80,
    send_midi_clock: bool = False
) -> None:
    if mido is None:
        raise RuntimeError("mido not installed. Run: pip install mido python-rtmidi")

    ayahs = read_quran_uthmani(in_path)
    letter_map = build_scale_and_map(maqam, scale_json)

    out = mido.open_output(midi_port_name)

    def wait_with_clock(dur_s: float, bpm: float):
        if not send_midi_clock:
            time.sleep(dur_s)
            return
        interval = bpm_to_midi_clock_interval_s(bpm)
        ticks = max(0, int(dur_s / interval))
        out.send(mido.Message("start"))
        t_next = time.perf_counter()
        for _ in range(ticks):
            out.send(mido.Message("clock"))
            t_next += interval
            now = time.perf_counter()
            sleep = t_next - now
            if sleep > 0:
                time.sleep(sleep)

    for ay in ayahs:
        bpm = surah_bpm(ay.surah, base_bpm)
        beat_s = 60.0 / max(1e-6, bpm)
        tp = TajwidParams(base_unit_s=beat_s / 3.0)

        tokens = tokenize_uthmani(ay.text)

        for i, tok in enumerate(tokens):
            nxt = tokens[i + 1] if i + 1 < len(tokens) else None
            sounded, tail = estimate_duration(tok, nxt, tp)

            if tok.letter == " ":
                wait_with_clock(tail, bpm)
                continue

            freq = letter_map.get(tok.letter, 220.0)
            note = freq_to_midi_note(freq)

            marks = set(tok.marks)
            vel = max(1, min(127, velocity))
            dur = sounded

            if tok.letter in (NOON, MEEM) and SHADDA in marks:
                vel = max(20, int(vel * 0.75))
                dur *= 1.10

            out.send(mido.Message("note_on", channel=channel, note=note, velocity=vel))
            wait_with_clock(dur, bpm)
            out.send(mido.Message("note_off", channel=channel, note=note, velocity=0))

            if tok.letter in QALQALAH and SUKUN in marks:
                out.send(mido.Message("note_on", channel=channel, note=note, velocity=max(20, int(vel * 0.6))))
                wait_with_clock(min(0.06, dur * 0.25), bpm)
                out.send(mido.Message("note_off", channel=channel, note=note, velocity=0))

            if tail > 0:
                wait_with_clock(tail, bpm)

        wait_with_clock(0.08, bpm)


# =========================
# CLI (defaults to quran-uthmani.txt)
# =========================

def main():
    ap = argparse.ArgumentParser(description="Tanzil Uthmani Qur'an sonifier for quran-uthmani.txt")
    ap.add_argument("--in", dest="in_path", default="quran-uthmani.txt", help="Input path (default: quran-uthmani.txt)")
    ap.add_argument("--mode", choices=["wav", "stream", "midi"], default="wav")
    ap.add_argument("--out", default="quran_sonified.wav", help="Output WAV (mode=wav)")
    ap.add_argument("--sr", type=int, default=48000)
    ap.add_argument("--base-bpm", type=float, default=84.0)
    ap.add_argument("--maqam", default="hijaz", help=f"Preset: {', '.join(MAQAM_PRESETS.keys())}")
    ap.add_argument("--scale-json", default=None, help="Custom semitone list JSON, overrides maqam")
    ap.add_argument("--iso-pulse", type=float, default=8.0)
    ap.add_argument("--iso-carrier", type=float, default=180.0)
    ap.add_argument("--iso-depth", type=float, default=0.88)
    ap.add_argument("--iso-gain", type=float, default=0.22)
    ap.add_argument("--click-track", action="store_true", help="Embed subtle pulse clicks in WAV")
    ap.add_argument("--midi-port", default="", help="MIDI output port (mode=midi)")
    ap.add_argument("--midi-channel", type=int, default=0)
    ap.add_argument("--midi-velocity", type=int, default=80)
    ap.add_argument("--send-midi-clock", action="store_true", help="Send MIDI clock for hardware sync (mode=midi)")
    args = ap.parse_args()

    iso = IsoParams(
        pulse_hz=args.iso_pulse,
        carrier_hz=args.iso_carrier,
        depth=args.iso_depth,
        smooth_ms=10.0,
        gain=args.iso_gain
    )

    if args.mode == "wav":
        render_wav_from_quran_uthmani(
            in_path=args.in_path,
            out_path=args.out,
            sr=args.sr,
            base_bpm=args.base_bpm,
            maqam=args.maqam,
            scale_json=args.scale_json,
            iso=iso,
            click_track=args.click_track
        )
        print(f"Wrote: {args.out}")

    elif args.mode == "stream":
        stream_from_quran_uthmani(
            in_path=args.in_path,
            sr=args.sr,
            base_bpm=args.base_bpm,
            maqam=args.maqam,
            scale_json=args.scale_json,
            iso=iso
        )

    elif args.mode == "midi":
        if not args.midi_port:
            if mido is None:
                raise RuntimeError("mido not installed. Run: pip install mido python-rtmidi")
            print("Available MIDI output ports:")
            for name in mido.get_output_names():
                print(" -", name)
            raise SystemExit("Provide --midi-port with one of the listed ports.")
        midi_from_quran_uthmani(
            in_path=args.in_path,
            midi_port_name=args.midi_port,
            base_bpm=args.base_bpm,
            maqam=args.maqam,
            scale_json=args.scale_json,
            channel=args.midi_channel,
            velocity=max(1, min(127, args.midi_velocity)),
            send_midi_clock=args.send_midi_clock
        )

if __name__ == "__main__":
    main()
