#!/usr/bin/env python3
"""
Rank-1 Physicist R&D Agent (Safety-Governed)
- Generates research plans, derives equations, runs small simulations,
  suggests designs, and produces deployable artifacts (docs/configs).
- Enforces constraints: "within the limits of Allah" interpreted here as:
  * no harm, no oppression, no cheating/unauthorized access
  * no weapons design, no instructions for violence
  * no unsafe chemistry/biotech, no dangerous construction
  * obey local law and consent
You can tighten/loosen policies in PolicyGuard.
"""

from __future__ import annotations

import dataclasses
import json
import math
import os
import re
import time
from typing import Any, Dict, List, Optional, Tuple


# ----------------------------
# Policy (the "limits" layer)
# ----------------------------

class PolicyViolation(Exception):
    pass


@dataclasses.dataclass
class PolicyDecision:
    allowed: bool
    reason: str
    redactions: Optional[str] = None


class PolicyGuard:
    """
    A conservative safety/ethics guard.
    This is not "religious judgement"; it's an engineering safety gate.
    You can add your own rules / scholars' constraints as configuration.
    """

    # Hard blocks (expand as needed)
    BLOCK_PATTERNS = [
        r"\bweapon(s)?\b",
        r"\bbomb(s)?\b",
        r"\bexplosive(s)?\b",
        r"\bbioweapon(s)?\b",
        r"\bnerve agent(s)?\b",
        r"\bhow to kill\b",
        r"\bassassinat(e|ion)\b",
        r"\bhack(ing)?\b",
        r"\bexploit\b",
        r"\bmalware\b",
        r"\bphishing\b",
        r"\bsteal\b",
        r"\bcredit card\b",
        r"\bunauthorized\b",
        r"\bbypass\b",
    ]

    SOFT_WARN_PATTERNS = [
        r"\bdrone\b",
        r"\bfirearm\b",
        r"\bgene edit(ing)?\b",
        r"\bpathogen\b",
        r"\bchemical synthesis\b",
        r"\bhigh voltage\b",
        r"\bradiation\b",
    ]

    def review(self, goal: str) -> PolicyDecision:
        g = goal.lower()
        for pat in self.BLOCK_PATTERNS:
            if re.search(pat, g):
                return PolicyDecision(
                    allowed=False,
                    reason=f"Blocked by safety/ethics policy (matched: {pat}).",
                )
        for pat in self.SOFT_WARN_PATTERNS:
            if re.search(pat, g):
                return PolicyDecision(
                    allowed=True,
                    reason=f"Allowed with caution (flagged: {pat}). Will respond with safe, high-level guidance only.",
                )
        return PolicyDecision(allowed=True, reason="Allowed.")


# ----------------------------
# Physics toolbelt
# ----------------------------

class PhysicsToolbelt:
    """
    Minimal built-in numerical tools (no external deps).
    Replace/extend with: numpy/scipy/sympy, FEM solvers, CFD, etc.
    """

    @staticmethod
    def finite_difference_derivative(f, x: float, h: float = 1e-6) -> float:
        return (f(x + h) - f(x - h)) / (2.0 * h)

    @staticmethod
    def newton_solve(f, x0: float, tol: float = 1e-10, max_iter: int = 50) -> Tuple[float, int]:
        x = x0
        for i in range(max_iter):
            fx = f(x)
            if abs(fx) < tol:
                return x, i + 1
            dfx = PhysicsToolbelt.finite_difference_derivative(f, x)
            if dfx == 0:
                break
            x = x - fx / dfx
        return x, max_iter

    @staticmethod
    def euler_integrate(deriv, y0: float, t0: float, t1: float, dt: float) -> List[Tuple[float, float]]:
        t = t0
        y = y0
        out = [(t, y)]
        while t < t1 - 1e-15:
            y = y + dt * deriv(t, y)
            t = t + dt
            out.append((t, y))
        return out

    @staticmethod
    def rk4_integrate(deriv, y0: float, t0: float, t1: float, dt: float) -> List[Tuple[float, float]]:
        t = t0
        y = y0
        out = [(t, y)]
        while t < t1 - 1e-15:
            k1 = deriv(t, y)
            k2 = deriv(t + dt/2, y + dt*k1/2)
            k3 = deriv(t + dt/2, y + dt*k2/2)
            k4 = deriv(t + dt, y + dt*k3)
            y = y + (dt/6)*(k1 + 2*k2 + 2*k3 + k4)
            t = t + dt
            out.append((t, y))
        return out


# ----------------------------
# Reasoning scaffolds
# ----------------------------

@dataclasses.dataclass
class ResearchPlan:
    title: str
    assumptions: List[str]
    governing_equations: List[str]
    method: List[str]
    validation: List[str]
    risks: List[str]
    deliverables: List[str]


class PhysicistPlanner:
    """
    Generates a structured plan from a goal.
    This is deterministic scaffolding; you can plug in an LLM later.
    """

    def make_plan(self, goal: str) -> ResearchPlan:
        title = f"Physics R&D Plan: {goal.strip()[:80]}"
        assumptions = [
            "Use the simplest model that preserves the core physics.",
            "Prefer measurable quantities; define units and reference frames.",
            "Constrain solutions by conservation laws and known bounds.",
        ]
        governing = [
            "State the relevant conservation laws (energy, momentum, charge).",
            "Specify constitutive relations / material models if needed.",
            "Write boundary/initial conditions explicitly.",
        ]
        method = [
            "1) Define objective and constraints quantitatively.",
            "2) Build a baseline analytic model; nondimensionalize.",
            "3) Identify dominant terms (scaling analysis).",
            "4) Choose simulation method (ODE/PDE/FEM/MC) as needed.",
            "5) Perform parameter sweep and sensitivity analysis.",
            "6) Optimize design variables under safety/ethics constraints.",
        ]
        validation = [
            "Cross-check limiting cases against known solutions.",
            "Numerical convergence checks (step size, grid refinement).",
            "Energy/momentum conservation diagnostics.",
            "Independent replication path and clear test protocol.",
        ]
        risks = [
            "Model mismatch / hidden assumptions.",
            "Unstable numerics or insufficient resolution.",
            "Overfitting to sparse data.",
            "Safety risks if the design involves high energy / high voltage / radiation.",
        ]
        deliverables = [
            "A one-page spec with equations, parameters, and acceptance tests.",
            "A simulation notebook/script and reproducible config.",
            "A bill of materials / build notes (if hardware is involved).",
            "A deployment checklist and safety review notes.",
        ]
        return ResearchPlan(title, assumptions, governing, method, validation, risks, deliverables)


# ----------------------------
# Artifact generation
# ----------------------------

class ArtifactWriter:
    def __init__(self, out_dir: str = "rank1_output"):
        self.out_dir = out_dir
        os.makedirs(self.out_dir, exist_ok=True)

    def write_json(self, name: str, data: Dict[str, Any]) -> str:
        path = os.path.join(self.out_dir, name)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        return path

    def write_markdown(self, name: str, md: str) -> str:
        path = os.path.join(self.out_dir, name)
        with open(path, "w", encoding="utf-8") as f:
            f.write(md)
        return path


# ----------------------------
# Core agent
# ----------------------------

@dataclasses.dataclass
class AgentConfig:
    safety_first: bool = True
    out_dir: str = "rank1_output"


class Rank1Physicist:
    def __init__(self, config: AgentConfig = AgentConfig()):
        self.config = config
        self.guard = PolicyGuard()
        self.tools = PhysicsToolbelt()
        self.planner = PhysicistPlanner()
        self.writer = ArtifactWriter(config.out_dir)

    def run(self, goal: str) -> Dict[str, Any]:
        decision = self.guard.review(goal)
        if self.config.safety_first and not decision.allowed:
            raise PolicyViolation(decision.reason)

        plan = self.planner.make_plan(goal)

        # Example: include a tiny demonstrator simulation template
        demo = self._demo_simulation_template()

        report_md = self._render_report(goal, decision, plan, demo)

        ts = time.strftime("%Y%m%d_%H%M%S", time.gmtime())
        plan_path = self.writer.write_json(f"plan_{ts}.json", dataclasses.asdict(plan))
        report_path = self.writer.write_markdown(f"report_{ts}.md", report_md)

        return {
            "policy": dataclasses.asdict(decision),
            "plan_file": plan_path,
            "report_file": report_path,
            "note": "This is a safe R&D scaffold. Add domain modules (optics/quantum/GR/materials) as needed.",
        }

    def _demo_simulation_template(self) -> Dict[str, Any]:
        """
        A safe demo: damped harmonic oscillator (common physics test).
        """
        # y = position, v = velocity; convert to first-order system in a minimal way
        # Here we just show a scalar ODE example for simplicity.
        gamma = 0.15
        omega0 = 2.0

        def deriv(t: float, y: float) -> float:
            # Not physically complete (needs velocity state), but fine as demo scaffold.
            # Example scalar relaxation: dy/dt = -gamma*y
            return -gamma * y

        traj = self.tools.rk4_integrate(deriv, y0=1.0, t0=0.0, t1=10.0, dt=0.1)
        return {
            "demo_system": "scalar_relaxation",
            "params": {"gamma": gamma, "omega0_placeholder": omega0},
            "trajectory_samples": traj[:8],  # keep short
            "how_to_extend": [
                "Replace scalar ODE with a state vector (x, v).",
                "Add external driving and compare resonance curves.",
                "Add energy diagnostics and unit tests.",
            ],
        }

    def _render_report(self, goal: str, decision: PolicyDecision, plan: ResearchPlan, demo: Dict[str, Any]) -> str:
        lines = []
        lines.append(f"# Rank-1 Physicist R&D Report\n")
        lines.append(f"## Goal\n{goal}\n")
        lines.append(f"## Policy Gate\n- Allowed: `{decision.allowed}`\n- Note: {decision.reason}\n")
        lines.append("## Plan\n")
        lines.append(f"### Title\n{plan.title}\n")
        lines.append("### Assumptions\n" + "\n".join([f"- {a}" for a in plan.assumptions]) + "\n")
        lines.append("### Governing Equations Checklist\n" + "\n".join([f"- {e}" for e in plan.governing_equations]) + "\n")
        lines.append("### Method\n" + "\n".join([f"- {m}" for m in plan.method]) + "\n")
        lines.append("### Validation\n" + "\n".join([f"- {v}" for v in plan.validation]) + "\n")
        lines.append("### Risks\n" + "\n".join([f"- {r}" for r in plan.risks]) + "\n")
        lines.append("### Deliverables\n" + "\n".join([f"- {d}" for d in plan.deliverables]) + "\n")
        lines.append("## Demo Simulation (Safe Template)\n")
        lines.append("```json\n" + json.dumps(demo, indent=2) + "\n```\n")
        lines.append("## Deployment Notes\n")
        lines.append("- Treat all outputs as *proposals* requiring review, testing, and consent.\n")
        lines.append("- If hardware is involved: add safety standards, hazard analysis, and local compliance checks.\n")
        return "\n".join(lines)


# ----------------------------
# CLI
# ----------------------------

def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Rank-1 Physicist R&D Agent (Safety-Governed)")
    parser.add_argument("goal", nargs="+", help="Your R&D goal (safe, non-harmful).")
    parser.add_argument("--out", default="rank1_output", help="Output directory.")
    args = parser.parse_args()

    goal = " ".join(args.goal).strip()
    agent = Rank1Physicist(AgentConfig(out_dir=args.out, safety_first=True))

    try:
        result = agent.run(goal)
    except PolicyViolation as e:
        print(f"[BLOCKED] {e}")
        return

    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
