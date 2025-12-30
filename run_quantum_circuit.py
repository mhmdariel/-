# Save as run_quantum_circuit.py
from qiskit import QuantumCircuit, transpile
from qiskit_aer import AerSimulator
from qiskit.visualization import plot_histogram
import matplotlib.pyplot as plt

# Load QASM file
circuit = QuantumCircuit.from_qasm_file("quantum_theorem.qasm")

# Use Aer simulator
simulator = AerSimulator()

# Transpile for simulator
compiled_circuit = transpile(circuit, simulator)

# Execute simulation
job = simulator.run(compiled_circuit, shots=1024)
result = job.result()
counts = result.get_counts(circuit)

# Display results
print("Measurement Results:")
print(f"Total shots: {sum(counts.values())}")
print(f"Unique states: {len(counts)}")

# Display most common states
sorted_counts = sorted(counts.items(), key=lambda x: x[1], reverse=True)
print("\nTop 10 Theorem States:")
for i, (state, count) in enumerate(sorted_counts[:10]):
    print(f"{i+1}. {state}: {count} occurrences")

# Convert to theorem interpretations
def interpret_state(quantum_state):
    """Interpret quantum state as theorem components"""
    # Example: Split 15-bit state into components
    theorem_part = quantum_state[:8]
    complexity_part = quantum_state[8:12]
    truth_part = quantum_state[12:14]
    unity_part = quantum_state[14]
    
    return {
        'theorem': theorem_part,
        'complexity': int(complexity_part, 2),
        'truth_value': truth_part,
        'unity': unity_part == '1'
    }

print("\n=== Theorem Interpretations ===")
for state, count in sorted_counts[:5]:
    interpretation = interpret_state(state)
    print(f"\nState: {state}")
    print(f"  Theorem encoding: {interpretation['theorem']}")
    print(f"  Complexity level: {interpretation['complexity']}")
    print(f"  Truth verification: {interpretation['truth_value']}")
    print(f"  Unity detected: {interpretation['unity']}")
    print(f"  Probability: {(count/1024)*100:.2f}%")

# Plot results
plot_histogram(counts, title="Quantum Theorem Generator Results")
plt.show()
