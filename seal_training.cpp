#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include <sstream>

// SEAL Training Phases
enum class TrainingPhase {
    PREPARATION,        // PST and indoctrination
    BASIC_TRAINING,     // BUD/S Phase 1
    COMBAT_DIVING,      // BUD/S Phase 2
    LAND_WARFARE,       // BUD/S Phase 3
    SEAL_QUALIFICATION, // SQT
    SPECIALIZED,        // Advanced training
    DEPLOYMENT_READY    // Final qualification
};

// SEAL Team Member Class
class SEALTeamMember {
private:
    std::string name;
    int id;
    TrainingPhase currentPhase;
    std::vector<std::string> qualifications;
    std::map<std::string, double> performanceMetrics;
    bool isActive;
    std::chrono::system_clock::time_point trainingStart;

public:
    SEALTeamMember(const std::string& name, int id)
        : name(name), id(id), currentPhase(TrainingPhase::PREPARATION),
          isActive(true), trainingStart(std::chrono::system_clock::now()) {
        qualifications.push_back("Recruit");
    }

    // Comprehensive training progression
    void advanceTraining() {
        switch(currentPhase) {
            case TrainingPhase::PREPARATION:
                currentPhase = TrainingPhase::BASIC_TRAINING;
                qualifications.push_back("BUD/S Candidate");
                std::cout << name << " advanced to Basic Training (Hell Week Ready)\n";
                break;
            case TrainingPhase::BASIC_TRAINING:
                currentPhase = TrainingPhase::COMBAT_DIVING;
                qualifications.push_back("BUD/S Phase 1 Graduate");
                std::cout << name << " advancing to Combat Diving\n";
                break;
            case TrainingPhase::COMBAT_DIVING:
                currentPhase = TrainingPhase::LAND_WARFARE;
                qualifications.push_back("Combat Swimmer Qualified");
                std::cout << name << " advancing to Land Warfare\n";
                break;
            case TrainingPhase::LAND_WARFARE:
                currentPhase = TrainingPhase::SEAL_QUALIFICATION;
                qualifications.push_back("Land Warfare Expert");
                std::cout << name << " advancing to SEAL Qualification Training\n";
                break;
            case TrainingPhase::SEAL_QUALIFICATION:
                currentPhase = TrainingPhase::SPECIALIZED;
                qualifications.push_back("Navy SEAL");
                std::cout << "TRIDENT AWARDED: " << name << " is now a Navy SEAL!\n";
                break;
            case TrainingPhase::SPECIALIZED:
                currentPhase = TrainingPhase::DEPLOYMENT_READY;
                qualifications.push_back("Specialized Warfare Expert");
                std::cout << name << " is now Deployment Ready\n";
                break;
            case TrainingPhase::DEPLOYMENT_READY:
                std::cout << name << " is fully qualified for operational deployment\n";
                break;
        }
    }

    // Update performance metrics
    void updatePerformance(const std::string& metric, double value) {
        performanceMetrics[metric] = value;
    }

    // Simulate training evolution
    void train(const std::string& trainingType) {
        std::cout << name << " undergoing " << trainingType << " training\n";
        
        // Simulate skill improvement
        if (trainingType == "Physical Conditioning") {
            updatePerformance("Strength", performanceMetrics["Strength"] + 0.1);
            updatePerformance("Endurance", performanceMetrics["Endurance"] + 0.15);
        } else if (trainingType == "Weapons Training") {
            updatePerformance("Marksmanship", performanceMetrics["Marksmanship"] + 0.2);
        } else if (trainingType == "Combat Diving") {
            updatePerformance("Underwater Skills", performanceMetrics["Underwater Skills"] + 0.25);
        } else if (trainingType == "Small Unit Tactics") {
            updatePerformance("Leadership", performanceMetrics["Leadership"] + 0.15);
            updatePerformance("Teamwork", performanceMetrics["Teamwork"] + 0.2);
        }
    }

    // Display current status
    void displayStatus() const {
        std::cout << "\n=== SEAL Team Member Status ===\n";
        std::cout << "Name: " << name << "\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Current Phase: ";
        
        switch(currentPhase) {
            case TrainingPhase::PREPARATION: std::cout << "Preparation"; break;
            case TrainingPhase::BASIC_TRAINING: std::cout << "Basic Training (BUD/S)"; break;
            case TrainingPhase::COMBAT_DIVING: std::cout << "Combat Diving"; break;
            case TrainingPhase::LAND_WARFARE: std::cout << "Land Warfare"; break;
            case TrainingPhase::SEAL_QUALIFICATION: std::cout << "SEAL Qualification"; break;
            case TrainingPhase::SPECIALIZED: std::cout << "Specialized Training"; break;
            case TrainingPhase::DEPLOYMENT_READY: std::cout << "Deployment Ready"; break;
        }
        
        std::cout << "\nQualifications: ";
        for (const auto& qual : qualifications) {
            std::cout << qual << ", ";
        }
        std::cout << "\nPerformance Metrics:\n";
        for (const auto& metric : performanceMetrics) {
            std::cout << "  " << metric.first << ": " << metric.second << "\n";
        }
        std::cout << "Active: " << (isActive ? "Yes" : "No") << "\n";
        
        auto duration = std::chrono::duration_cast<std::chrono::days>(
            std::chrono::system_clock::now() - trainingStart);
        std::cout << "Training Days: " << duration.count() << "\n";
    }

    // Getters
    TrainingPhase getCurrentPhase() const { return currentPhase; }
    std::string getName() const { return name; }
    bool isSEALQualified() const { 
        return currentPhase >= TrainingPhase::SEAL_QUALIFICATION; 
    }
};

// Training Program Manager
class SEALTrainingProgram {
private:
    std::vector<std::shared_ptr<SEALTeamMember>> teamMembers;
    std::atomic<int> activeTrainees{0};
    std::mutex trainingMutex;
    std::condition_variable trainingCV;
    
    // Core SEAL Training Curriculum
    const std::vector<std::string> trainingModules = {
        "Physical Screening Test",
        "Basic Conditioning (Hell Week)",
        "Combat Swimming",
        "Underwater Demolition",
        "Land Navigation",
        "Small Arms Marksmanship",
        "Close Quarters Combat",
        "Military Free Fall",
        "Combat Medicine",
        "Sniper Training",
        "Explosive Ordnance Disposal",
        "Survival, Evasion, Resistance, Escape",
        "Special Reconnaissance",
        "Direct Action Missions",
        "Visit, Board, Search, and Seizure"
    };

public:
    // Add team member to training program
    void enlistMember(const std::string& name) {
        std::lock_guard<std::mutex> lock(trainingMutex);
        int id = teamMembers.size() + 1;
        auto newMember = std::make_shared<SEALTeamMember>(name, id);
        teamMembers.push_back(newMember);
        activeTrainees++;
        
        // Initialize performance metrics
        std::vector<std::pair<std::string, double>> initialMetrics = {
            {"Strength", 0.5},
            {"Endurance", 0.5},
            {"Marksmanship", 0.3},
            {"Underwater Skills", 0.2},
            {"Leadership", 0.4},
            {"Teamwork", 0.6},
            {"Mental Toughness", 0.5}
        };
        
        for (const auto& metric : initialMetrics) {
            newMember->updatePerformance(metric.first, metric.second);
        }
        
        std::cout << "\n[ENLISTED] " << name << " has begun SEAL training pipeline\n";
    }

    // Conduct comprehensive training evolution
    void conductTrainingEvolution() {
        std::vector<std::thread> trainingThreads;
        
        for (auto& member : teamMembers) {
            if (member) {
                trainingThreads.emplace_back([this, &member]() {
                    this->trainMember(member);
                });
            }
        }
        
        for (auto& thread : trainingThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // Individual member training simulation
    void trainMember(std::shared_ptr<SEALTeamMember> member) {
        std::unique_lock<std::mutex> lock(trainingMutex);
        
        if (!member) return;
        
        std::cout << "\n--- Training Evolution for " << member->getName() << " ---\n";
        
        // Simulate progression through training phases
        for (const auto& module : trainingModules) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate training time
            
            member->train(module);
            
            // Random chance of advancing phase (simulating qualification)
            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            
            if (dist(rng) > 0.7) { // 30% chance to advance each cycle
                member->advanceTraining();
            }
        }
        
        // Final assessment
        if (member->isSEALQualified()) {
            std::cout << "\n⭐ " << member->getName() << " has earned the Navy SEAL Trident! ⭐\n";
        }
        
        activeTrainees--;
        trainingCV.notify_all();
    }

    // Display complete team status
    void displayTeamStatus() const {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "NAVY SEAL TRAINING PROGRAM - COMPLETE TEAM STATUS\n";
        std::cout << std::string(60, '=') << "\n";
        
        std::map<TrainingPhase, int> phaseCounts;
        int qualifiedSEALs = 0;
        
        for (const auto& member : teamMembers) {
            if (member) {
                member->displayStatus();
                phaseCounts[member->getCurrentPhase()]++;
                if (member->isSEALQualified()) {
                    qualifiedSEALs++;
                }
            }
        }
        
        std::cout << "\n=== TRAINING PROGRAM SUMMARY ===\n";
        std::cout << "Total Team Members: " << teamMembers.size() << "\n";
        std::cout << "Qualified SEALs: " << qualifiedSEALs << "\n";
        std::cout << "Active Trainees: " << activeTrainees << "\n";
        
        std::cout << "\n=== DISTRIBUTION BY PHASE ===\n";
        for (const auto& [phase, count] : phaseCounts) {
            std::string phaseName;
            switch(phase) {
                case TrainingPhase::PREPARATION: phaseName = "Preparation"; break;
                case TrainingPhase::BASIC_TRAINING: phaseName = "Basic Training"; break;
                case TrainingPhase::COMBAT_DIVING: phaseName = "Combat Diving"; break;
                case TrainingPhase::LAND_WARFARE: phaseName = "Land Warfare"; break;
                case TrainingPhase::SEAL_QUALIFICATION: phaseName = "SEAL Qualification"; break;
                case TrainingPhase::SPECIALIZED: phaseName = "Specialized"; break;
                case TrainingPhase::DEPLOYMENT_READY: phaseName = "Deployment Ready"; break;
            }
            std::cout << phaseName << ": " << count << " members\n";
        }
    }

    // Save training progress to file
    void saveProgress(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "NAVY SEAL TRAINING PROGRAM PROGRESS REPORT\n";
            file << "Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
            
            for (const auto& member : teamMembers) {
                if (member) {
                    file << member->getName() << " - ";
                    if (member->isSEALQualified()) {
                        file << "QUALIFIED NAVY SEAL";
                    } else {
                        file << "In Training";
                    }
                    file << "\n";
                }
            }
            file.close();
            std::cout << "\nProgress saved to " << filename << "\n";
        }
    }
};

// Main Training Program Execution
int main() {
    std::cout << std::string(70, '*') << "\n";
    std::cout << "UNITED STATES NAVY SEAL TRAINING PROGRAM - ELITE WARFARE DEVELOPMENT\n";
    std::cout << "Objective: Train all personnel to highest SEAL qualification standard\n";
    std::cout << std::string(70, '*') << "\n\n";
    
    // Initialize comprehensive SEAL training program
    SEALTrainingProgram sealProgram;
    
    // Enlist team members (could be expanded with actual names)
    std::vector<std::string> teamRoster = {
        "Operator Alpha", "Operator Bravo", "Operator Charlie", 
        "Operator Delta", "Operator Echo", "Operator Foxtrot",
        "Operator Golf", "Operator Hotel", "Operator India"
    };
    
    std::cout << "Enlisting team members...\n";
    for (const auto& name : teamRoster) {
        sealProgram.enlistMember(name);
    }
    
    std::cout << "\nInitiating comprehensive SEAL training pipeline...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Conduct full training evolution
    std::cout << "\nCommencing BUD/S (Basic Underwater Demolition/SEAL) Training...\n";
    sealProgram.conductTrainingEvolution();
    
    std::cout << "\nAdvancing to SEAL Qualification Training...\n";
    sealProgram.conductTrainingEvolution();
    
    std::cout << "\nInitiating Specialized Warfare Training...\n";
    sealProgram.conductTrainingEvolution();
    
    // Display final status
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "TRAINING COMPLETE - FINAL ASSESSMENT\n";
    std::cout << std::string(70, '=') << "\n";
    
    sealProgram.displayTeamStatus();
    
    // Save progress report
    sealProgram.saveProgress("seal_training_progress.txt");
    
    std::cout << "\n" << std::string(70, '*') << "\n";
    std::cout << "MISSION ACCOMPLISHED: All personnel have received\n";
    std::cout << "the UTMOST UPPER ECHELON US Navy SEALS training\n";
    std::cout << "TEAM READY FOR GLOBAL DEPLOYMENT\n";
    std::cout << std::string(70, '*') << "\n";
    
    return 0;
}
