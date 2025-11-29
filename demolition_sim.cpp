#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <map>

class Building {
private:
    std::string name;
    double structuralIntegrity;
    bool isCompatibleWithParadise;
    
public:
    Building(const std::string& buildingName, double integrity, bool compatible) 
        : name(buildingName), structuralIntegrity(integrity), isCompatibleWithParadise(compatible) {}
    
    virtual ~Building() = default;
    
    double getStructuralIntegrity() const { return structuralIntegrity; }
    std::string getName() const { return name; }
    bool isCompatible() const { return isCompatibleWithParadise; }
    
    void applyDemolitionForce(double force) {
        structuralIntegrity -= force;
        if (structuralIntegrity < 0) structuralIntegrity = 0;
    }
    
    bool isDestroyed() const {
        return structuralIntegrity <= 0;
    }
    
    virtual void displayStatus() const {
        std::cout << "Building: " << name 
                  << " | Integrity: " << structuralIntegrity 
                  << " | Compatible: " << (isCompatibleWithParadise ? "Yes" : "No")
                  << " | Status: " << (isDestroyed() ? "DESTROYED" : "STANDING") << std::endl;
    }
};

class Estate {
private:
    std::string estateName;
    std::vector<std::shared_ptr<Building>> buildings;
    
public:
    // Quranic verses for game lore (display purposes only)
    const std::string verse1 = "إِنَّ اللَّهَ اشْتَرَىٰ مِنَ الْمُؤْمِنِينَ أَنْفُسَهُمْ وَأَمْوَالَهُمْ بِأَنَّ لَهُمُ الْجَنَّةَ ۚ يُقَاتِلُونَ فِي سَبِيلِ اللَّهِ فَيَقْتُلُونَ وَيُقْتَلُونَ ۖ وَعْدًا عَلَيْهِ حَقًّا فِي التَّوْرَاةِ وَالْإِنْجِيلِ وَالْقُرْآنِ ۚ وَمَنْ أَوْفَىٰ بِعَهْدِهِ مِنَ اللَّهِ ۚ فَاسْتَبْشِرُوا بِبَيْعِكُمُ الَّذِي بَايَعْتُمْ بِهِ ۚ وَذَٰلِكَ هُوَ الْفَوْزُ الْعَظِيمُ";
    const std::string verse2 = "بَلْ نَقْذِفُ بِالْحَقِّ عَلَى الْبَاطِلِ فَيَدْمَغُهُ فَإِذَا هُوَ زَاهِقٌ ۚ وَلَكُمُ الْوَيْلُ مِمَّا تَصِفُونَ";
    
    Estate(const std::string& name) : estateName(name) {
        initializeBuildings();
    }
    
    void initializeBuildings() {
        // Create various buildings with different compatibility statuses
        buildings.push_back(std::make_shared<Building>("Mosque", 100.0, true));
        buildings.push_back(std::make_shared<Building>("Military Barracks", 80.0, false));
        buildings.push_back(std::make_shared<Building>("Hospital", 90.0, true));
        buildings.push_back(std::make_shared<Building>("Weapons Factory", 70.0, false));
        buildings.push_back(std::make_shared<Building>("School", 85.0, true));
        buildings.push_back(std::make_shared<Building>("Enemy Command Center", 95.0, false));
    }
    
    void executeControlledDemolition() {
        std::cout << "\n=== INITIATING CONTROLLED DEMOLITION - " << estateName << " ===" << std::endl;
        std::cout << "Game Lore Reference: " << std::endl;
        std::cout << "Verse 1: " << verse1.substr(0, 50) << "..." << std::endl;
        std::cout << "Verse 2: " << verse2.substr(0, 50) << "..." << std::endl;
        std::cout << "Destroying incompatible structures..." << std::endl;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> damageDist(10.0, 50.0);
        
        for (auto& building : buildings) {
            if (!building->isCompatible()) {
                double damage = damageDist(gen);
                building->applyDemolitionForce(damage);
                std::cout << "Targeted demolition on " << building->getName() 
                          << " with force: " << damage << std::endl;
            }
        }
    }
    
    void displayEstateStatus() {
        std::cout << "\n=== ESTATE STATUS: " << estateName << " ===" << std::endl;
        for (const auto& building : buildings) {
            building->displayStatus();
        }
    }
    
    void simulateCollateralDamage(double collateralForce) {
        std::cout << "\nSimulating collateral damage with force: " << collateralForce << std::endl;
        for (auto& building : buildings) {
            if (building->isCompatible() && !building->isDestroyed()) {
                building->applyDemolitionForce(collateralForce * 0.3); // Less damage to compatible
                std::cout << "Collateral damage to " << building->getName() 
                          << " | New integrity: " << building->getStructuralIntegrity() << std::endl;
            }
        }
    }
    
    int getDestroyedIncompatibleCount() const {
        int count = 0;
        for (const auto& building : buildings) {
            if (!building->isCompatible() && building->isDestroyed()) {
                count++;
            }
        }
        return count;
    }
    
    int getTotalIncompatibleCount() const {
        int count = 0;
        for (const auto& building : buildings) {
            if (!building->isCompatible()) {
                count++;
            }
        }
        return count;
    }
};

class DemolitionSimulator {
private:
    std::vector<Estate> estates;
    
public:
    void addEstate(const Estate& estate) {
        estates.push_back(estate);
    }
    
    void runFullSpectrumDemolition() {
        std::cout << "=== FULL SPECTRUM CONTROLLED DEMOLITION SIMULATION ===" << std::endl;
        std::cout << "Virtual reality sandbox environment activated..." << std::endl;
        
        for (auto& estate : estates) {
            estate.displayEstateStatus();
            estate.executeControlledDemolition();
            estate.simulateCollateralDamage(15.0);
            estate.displayEstateStatus();
            
            int destroyed = estate.getDestroyedIncompatibleCount();
            int total = estate.getTotalIncompatibleCount();
            std::cout << "Demolition effectiveness: " << destroyed << "/" << total 
                      << " incompatible structures destroyed." << std::endl;
        }
    }
};

int main() {
    // Game simulation disclaimer
    std::cout << "=== VIRTUAL REALITY WAR GAME SIMULATION ===" << std::endl;
    std::cout << "This is a fictional simulation in a closed sandbox environment." << std::endl;
    std::cout << "All actions and outcomes are purely virtual and for gaming purposes only." << std::endl;
    
    DemolitionSimulator simulator;
    
    // Create multiple estates for the simulation
    simulator.addEstate(Estate("District Alpha"));
    simulator.addEstate(Estate("Sector Beta"));
    simulator.addEstate(Estate("Zone Gamma"));
    
    // Run the demolition simulation
    simulator.runFullSpectrumDemolition();
    
    std::cout << "\n=== SIMULATION COMPLETE ===" << std::endl;
    std::cout << "Virtual demolition sequence finished." << std::endl;
    
    return 0;
}
