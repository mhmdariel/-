import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from itertools import product

# ----------------------------
# Infinite-Dimensional Realm
# ----------------------------
class Realm:
    def __init__(self, name, shape):
        self.name = name
        self.shape = shape
        self.dimensions = len(shape)
        self.states = np.zeros(shape)
        self.creatures = {}  # position -> creature
    
    def add_creature(self, creature, position):
        if len(position) != self.dimensions:
            raise ValueError("Position must match realm dimensions")
        self.creatures[position] = creature
    
    def apply_laws(self, wisdom_db):
        for pos, creature in self.creatures.items():
            action = creature.choose_action()
            consequence = wisdom_db.determine_outcome(action)
            self.states[pos] += consequence
            creature.receive(consequence)
    
    def project_slice(self, dim=0, index=0):
        slicer = [slice(None)] * self.dimensions
        slicer[dim] = index
        return self.states[tuple(slicer)]
    
    def get_creatures_on_slice(self, dim=0, index=0):
        """Return list of (x,y) positions and names of creatures on this slice"""
        positions = []
        for pos, creature in self.creatures.items():
            if pos[dim] == index:
                # Remove the sliced dimension to map to 2D projection
                proj_pos = list(pos)
                proj_pos.pop(dim)
                positions.append((tuple(proj_pos), creature.name))
        return positions

# ----------------------------
# Creatures
# ----------------------------
class Creature:
    def __init__(self, name):
        self.name = name
        self.state = 0
    
    def choose_action(self):
        return np.random.randint(-1, 2)
    
    def receive(self, consequence):
        self.state += consequence

# ----------------------------
# Infinite Wisdom
# ----------------------------
class InfiniteKnowledge:
    def determine_outcome(self, action):
        return np.sign(action) * np.random.rand()

# ----------------------------
# Interactive Multi-Slice Dashboard
# ----------------------------
class InteractiveCosmicDashboard:
    def __init__(self, realms_shapes, slice_dims_indices):
        self.realms = [Realm(f"Realm {i+1}", shape) for i, shape in enumerate(realms_shapes)]
        self.wisdom_db = InfiniteKnowledge()
        self.slice_dims_indices = slice_dims_indices
    
    def add_creature(self, creature, realm_index=0, position=None):
        self.realms[realm_index].add_creature(creature, position)
    
    def run_dashboard(self, steps=50):
        total_slices = sum(len(slices) for slices in self.slice_dims_indices)
        fig, axes = plt.subplots(1, total_slices, figsize=(5*total_slices,5))
        if total_slices == 1:
            axes = [axes]
        
        ims = []
        scatters = []
        idx = 0
        for realm_idx, realm in enumerate(self.realms):
            for dim, slice_index in self.slice_dims_indices[realm_idx]:
                proj = realm.project_slice(dim, slice_index)
                im = axes[idx].imshow(proj, cmap='viridis', vmin=-1, vmax=1)
                axes[idx].set_title(f"{realm.name} dim{dim}={slice_index}")
                fig.colorbar(im, ax=axes[idx], fraction=0.046, pad=0.04)
                
                # Initialize scatter for creatures
                creatures = realm.get_creatures_on_slice(dim, slice_index)
                if creatures:
                    xs, ys, labels = zip(*[(p[1], p[0], n) for p,n in creatures])
                    scat = axes[idx].scatter(xs, ys, c='red', s=50)
                else:
                    scat = axes[idx].scatter([], [], c='red', s=50)
                ims.append(im)
                scatters.append((scat, realm, dim, slice_index))
                idx += 1
        
        def update(frame):
            for (scat, realm, dim, slice_index), im in zip(scatters, ims):
                realm.apply_laws(self.wisdom_db)
                proj = realm.project_slice(dim, slice_index)
                im.set_array(proj)
                
                # Update creature positions on slice
                creatures = realm.get_creatures_on_slice(dim, slice_index)
                if creatures:
                    xs, ys, labels = zip(*[(p[1], p[0], n) for p,n in creatures])
                    scat.set_offsets(np.c_[xs, ys])
                else:
                    scat.set_offsets(np.empty((0,2)))
            fig.suptitle(f"Interactive Cosmic Dashboard - Step {frame+1}", fontsize=16)
            return ims + [s[0] for s in scatters]
        
        ani = animation.FuncAnimation(fig, update, frames=steps, interval=500, blit=True)
        plt.show()
    
    def creature_report(self):
        report = {}
        for realm in self.realms:
            report[realm.name] = {c.name: c.state for c in realm.creatures.values()}
        return report

# ----------------------------
# Example Usage
# ----------------------------
if __name__ == "__main__":
    engine = InteractiveCosmicDashboard(
        realms_shapes=[(5,5,5,5), (6,6,6,6)],
        slice_dims_indices=[
            [(0,0),(1,2)],
            [(0,1),(2,3)]
        ]
    )
    
    engine.add_creature(Creature("Human A"), realm_index=0, position=(1,2,3,0))
    engine.add_creature(Creature("Human B"), realm_index=0, position=(2,1,0,4))
    engine.add_creature(Creature("Angel X"), realm_index=1, position=(3,3,2,1))
    engine.add_creature(Creature("Angel Y"), realm_index=1, position=(1,4,0,5))
    
    # Run interactive dashboard
    engine.run_dashboard(steps=30)
    
    # Final creature states
    report = engine.creature_report()
    print("\n=== Creature States ===")
    for realm, states in report.items():
        print(f"{realm}:")
        for name, state in states.items():
            print(f"  {name}: {state:.3f}")
