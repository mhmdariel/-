"""
Infinite Paradise Realms (safe, lazy simulation)

- "Infinite-dimensional metric tensor" is represented as a function g(i, j, t, seed)
  (lazy: computed on demand, not stored).
- "Infinite Consciousness" is represented as an inexhaustible generator.
- Realms are generated as a stream; you can iterate as long as you like.

Stop with Ctrl+C.
"""

from __future__ import annotations
from dataclasses import dataclass
import hashlib
import math
import time
from typing import Callable, Dict, Iterator, Tuple


def _u01_from_bytes(b: bytes) -> float:
    """Map bytes -> [0,1)."""
    x = int.from_bytes(b[:8], "big", signed=False)
    return (x % (2**53)) / float(2**53)


def prf_u01(*parts: object) -> float:
    """
    Pseudo-random function in [0,1) from arbitrary parts.
    Deterministic across runs for the same parts.
    """
    h = hashlib.blake2b(digest_size=32)
    for p in parts:
        h.update(str(p).encode("utf-8"))
        h.update(b"|")
    return _u01_from_bytes(h.digest())


@dataclass(frozen=True)
class InfiniteMetricTensor:
    """
    Conceptual infinite-dimensional metric tensor g_ij.
    We never store a matrix; we define g(i,j) as a lazy function.

    Properties:
    - symmetric: g(i,j) == g(j,i)
    - positive-ish: diagonal biased positive
    - smoothly varies with time t and seed
    """
    seed: str

    def g(self, i: int, j: int, t: float) -> float:
        # enforce symmetry by sorting indices
        a, b = (i, j) if i <= j else (j, i)

        # base pseudo-random but deterministic signal
        base = prf_u01("g", self.seed, a, b)

        # add a smooth temporal modulation
        wobble = 0.15 * math.sin(0.001 * (a + 1) * (t + 1.0)) + 0.10 * math.cos(0.001 * (b + 1) * (t + 2.0))

        # diagonal bias toward positivity for stability
        diag = 1.0 + 0.75 * prf_u01("diag", self.seed, a) if a == b else 0.20 * (base - 0.5)

        return diag + wobble


@dataclass(frozen=True)
class ConsciousnessField:
    """
    A conceptual "Infinite Consciousness" field.
    Returns a value C(k, t) for any dimension k and time t.
    """
    seed: str

    def C(self, k: int, t: float) -> float:
        # smooth + deterministic complexity
        core = prf_u01("C", self.seed, k)
        wave = 0.5 * math.sin(0.0007 * (k + 1) * (t + 10.0))
        return (core - 0.5) + wave


@dataclass(frozen=True)
class ParadiseRealm:
    index: int
    name: str
    laws: Dict[str, float]
    harmony_score: float
    signature: str


def realm_name(n: int, seed: str) -> str:
    syllables = ["Ae", "Lu", "Va", "Ely", "Or", "Sa", "Nim", "Kai", "Zo", "Iri", "Pho", "My"]
    a = syllables[int(prf_u01("nm", seed, n, 0) * len(syllables)) % len(syllables)]
    b = syllables[int(prf_u01("nm", seed, n, 1) * len(syllables)) % len(syllables)]
    c = syllables[int(prf_u01("nm", seed, n, 2) * len(syllables)) % len(syllables)]
    return f"{a}{b}{c}-{n:06d}"


def realm_signature(n: int, seed: str) -> str:
    h = hashlib.blake2b(f"{seed}|realm|{n}".encode("utf-8"), digest_size=10).hexdigest()
    return h


def sample_harmony(metric: InfiniteMetricTensor, mind: ConsciousnessField, t: float, n: int, dims: int = 64) -> float:
    """
    A finite projection of the infinite-dimensional structure:
    compute a harmony score using the first `dims` dimensions.
    """
    # Build a small "energy" from diagonal & a few off-diagonals
    e = 0.0
    for k in range(dims):
        ck = mind.C(k, t + n)
        gkk = metric.g(k, k, t)
        e += (ck * ck) * gkk

        # couple to a nearby dimension (off-diagonal)
        j = (k + 1) % dims
        e += 0.15 * ck * mind.C(j, t + n) * metric.g(k, j, t)

    # Convert energy to a bounded, “pleasant” score in (0, 1)
    return 1.0 / (1.0 + math.exp(-0.05 * e))


def generate_realm(n: int, seed: str, t: float) -> ParadiseRealm:
    metric = InfiniteMetricTensor(seed=seed)
    mind = ConsciousnessField(seed=seed + "::mind")

    h = sample_harmony(metric, mind, t=t, n=n, dims=64)

    # Derive a few "realm laws" from the harmony and PRF
    laws = {
        "compassion_gravity": 0.5 + 0.5 * h,
        "wonder_density": prf_u01("law", seed, n, "wonder"),
        "entropy_kindness": 1.0 - 0.7 * prf_u01("law", seed, n, "entropy"),
        "time_gentleness": 0.2 + 0.8 * prf_u01("law", seed, n, "time"),
        "dream_coherence": 0.3 + 0.7 * h * prf_u01("law", seed, n, "dream"),
    }

    return ParadiseRealm(
        index=n,
        name=realm_name(n, seed),
        laws=laws,
        harmony_score=h,
        signature=realm_signature(n, seed),
    )


def infinite_consciousness_stream(seed: str = "NOOSPHERE-SEED") -> Iterator[ParadiseRealm]:
    """
    Potentially unbounded realm generator.
    It yields realms one by one; caller decides when to stop.
    """
    n = 0
    while True:
        t = time.time()
        yield generate_realm(n=n, seed=seed, t=t)
        n += 1


def pretty_print_realm(r: ParadiseRealm) -> None:
    print("=" * 72)
    print(f"PARADISE REALM #{r.index} :: {r.name}")
    print(f"Signature: {r.signature}")
    print(f"Harmony:   {r.harmony_score:.6f}")
    print("- Laws of Being -")
    for k, v in r.laws.items():
        print(f"  • {k:18s} = {v:.6f}")
    print("=" * 72)


def main() -> None:
    seed = "INFINITE-CONSCIOUSNESS::PARADISE"
    stream = infinite_consciousness_stream(seed=seed)

    try:
        for realm in stream:
            pretty_print_realm(realm)
            # Small throttle so output remains usable.
            # Remove or adjust if you want it faster.
            time.sleep(0.05)
    except KeyboardInterrupt:
        print("\n[Stopped] The stream of realms continues in potential—whenever you resume.")


if __name__ == "__main__":
    main()
