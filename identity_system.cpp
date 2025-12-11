#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h> // For SHA-256 hashing
#include <regex>

// Base Identity Component
class IdentityComponent {
protected:
    std::string componentId;
    std::string componentType;
    time_t creationTime;
    
public:
    IdentityComponent(const std::string& type) 
        : componentType(type), creationTime(time(nullptr)) {
        generateComponentId();
    }
    
    virtual ~IdentityComponent() = default;
    
    void generateComponentId() {
        std::stringstream ss;
        ss << "ID_" << componentType << "_" << std::hex << creationTime;
        componentId = ss.str();
    }
    
    virtual void displayInfo() const = 0;
    virtual std::string getData() const = 0;
    
    std::string getId() const { return componentId; }
    std::string getType() const { return componentType; }
    time_t getCreationTime() const { return creationTime; }
};

// Biometric Component
class BiometricData : public IdentityComponent {
private:
    std::string fingerprintHash;
    std::string facialRecognitionData;
    std::string irisScanHash;
    
public:
    BiometricData(const std::string& fingerprint, 
                  const std::string& facialData,
                  const std::string& irisData)
        : IdentityComponent("BIOMETRIC"),
          fingerprintHash(hashData(fingerprint)),
          facialRecognitionData(facialData),
          irisScanHash(hashData(irisData)) {}
    
    static std::string hashData(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)data.c_str(), data.length(), hash);
        
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
    
    void displayInfo() const override {
        std::cout << "=== Biometric Data ===\n";
        std::cout << "Component ID: " << componentId << "\n";
        std::cout << "Fingerprint Hash: " << fingerprintHash.substr(0, 16) << "...\n";
        std::cout << "Facial Data: " << facialRecognitionData << "\n";
        std::cout << "Iris Scan Hash: " << irisScanHash.substr(0, 16) << "...\n";
        std::cout << "Created: " << std::ctime(&creationTime);
    }
    
    std::string getData() const override {
        return fingerprintHash + ":" + facialRecognitionData + ":" + irisScanHash;
    }
    
    bool verifyFingerprint(const std::string& fingerprint) const {
        return fingerprintHash == hashData(fingerprint);
    }
};

// Digital Identity Component
class DigitalIdentity : public IdentityComponent {
private:
    std::string username;
    std::string email;
    std::string digitalSignature;
    std::vector<std::string> linkedAccounts;
    
public:
    DigitalIdentity(const std::string& user, const std::string& mail)
        : IdentityComponent("DIGITAL"), username(user), email(mail) {
        generateDigitalSignature();
    }
    
    void generateDigitalSignature() {
        std::string data = username + email + std::to_string(creationTime);
        digitalSignature = BiometricData::hashData(data);
    }
    
    void addLinkedAccount(const std::string& account) {
        linkedAccounts.push_back(account);
    }
    
    void displayInfo() const override {
        std::cout << "=== Digital Identity ===\n";
        std::cout << "Component ID: " << componentId << "\n";
        std::cout << "Username: " << username << "\n";
        std::cout << "Email: " << email << "\n";
        std::cout << "Digital Signature: " << digitalSignature.substr(0, 16) << "...\n";
        std::cout << "Linked Accounts: " << linkedAccounts.size() << "\n";
        for(const auto& account : linkedAccounts) {
            std::cout << "  - " << account << "\n";
        }
    }
    
    std::string getData() const override {
        std::string accounts;
        for(const auto& acc : linkedAccounts) {
            accounts += acc + ";";
        }
        return username + ":" + email + ":" + digitalSignature + ":" + accounts;
    }
    
    bool verifyEmail(const std::string& checkEmail) const {
        std::regex emailPattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
        return std::regex_match(checkEmail, emailPattern) && email == checkEmail;
    }
};

// Government ID Component
class GovernmentID : public IdentityComponent {
private:
    std::string nationalId;
    std::string passportNumber;
    std::string drivingLicense;
    std::string country;
    
public:
    GovernmentID(const std::string& nid, const std::string& passport, 
                 const std::string& license, const std::string& cntry)
        : IdentityComponent("GOVERNMENT"), 
          nationalId(nid), 
          passportNumber(passport),
          drivingLicense(license),
          country(cntry) {}
    
    void displayInfo() const override {
        std::cout << "=== Government ID ===\n";
        std::cout << "Component ID: " << componentId << "\n";
        std::cout << "Country: " << country << "\n";
        std::cout << "National ID: " << nationalId << "\n";
        std::cout << "Passport: " << passportNumber << "\n";
        std::cout << "Driver's License: " << drivingLicense << "\n";
    }
    
    std::string getData() const override {
        return country + ":" + nationalId + ":" + passportNumber + ":" + drivingLicense;
    }
    
    bool validateNationalId() const {
        return nationalId.length() >= 8 && nationalId.length() <= 15;
    }
};

// Main Identity System
class DirectIdentitySystem {
private:
    std::string masterIdentityCode;
    std::vector<std::shared_ptr<IdentityComponent>> components;
    std::string systemHash;
    
public:
    DirectIdentitySystem() {
        generateMasterCode();
    }
    
    void generateMasterCode() {
        std::stringstream ss;
        ss << "ID_SYSTEM_" << time(nullptr) << "_" << std::rand();
        masterIdentityCode = ss.str();
        systemHash = BiometricData::hashData(masterIdentityCode);
    }
    
    void addComponent(std::shared_ptr<IdentityComponent> component) {
        components.push_back(component);
        updateSystemHash();
    }
    
    void updateSystemHash() {
        std::string combinedData;
        for(const auto& comp : components) {
            combinedData += comp->getData();
        }
        combinedData += masterIdentityCode;
        systemHash = BiometricData::hashData(combinedData);
    }
    
    void displayFullIdentity() const {
        std::cout << "\n========== DIRECT IDENTITY CODE SYSTEM ==========\n";
        std::cout << "Master Identity Code: " << masterIdentityCode << "\n";
        std::cout << "System Hash: " << systemHash.substr(0, 32) << "...\n";
        std::cout << "Total Components: " << components.size() << "\n";
        std::cout << "================================================\n\n";
        
        for(const auto& component : components) {
            component->displayInfo();
            std::cout << "\n";
        }
        
        std::cout << "================================================\n";
    }
    
    bool verifyIdentity() const {
        if(components.empty()) return false;
        
        std::cout << "\n=== Identity Verification Process ===\n";
        
        // Check each component
        for(const auto& comp : components) {
            std::cout << "Verifying " << comp->getType() << " component... ";
            
            if(comp->getType() == "BIOMETRIC") {
                auto bio = std::dynamic_pointer_cast<BiometricData>(comp);
                if(bio && !bio->getData().empty()) {
                    std::cout << "✓ Biometric data present\n";
                } else {
                    std::cout << "✗ Biometric data missing\n";
                    return false;
                }
            }
            else if(comp->getType() == "DIGITAL") {
                auto dig = std::dynamic_pointer_cast<DigitalIdentity>(comp);
                if(dig && dig->verifyEmail("user@example.com")) {
                    std::cout << "✓ Digital identity valid\n";
                } else {
                    std::cout << "✗ Digital identity invalid\n";
                    return false;
                }
            }
            else if(comp->getType() == "GOVERNMENT") {
                auto gov = std::dynamic_pointer_cast<GovernmentID>(comp);
                if(gov && gov->validateNationalId()) {
                    std::cout << "✓ Government ID valid\n";
                } else {
                    std::cout << "✗ Government ID invalid\n";
                    return false;
                }
            }
        }
        
        std::cout << "✓ All identity components verified successfully!\n";
        return true;
    }
    
    void generateIdentityReport() const {
        std::cout << "\n=== IDENTITY SYSTEM REPORT ===\n";
        std::cout << "Generated: " << getCurrentTimestamp() << "\n";
        std::cout << "Master Code: " << masterIdentityCode << "\n";
        std::cout << "System Integrity Hash: " << systemHash << "\n";
        std::cout << "Component Count: " << components.size() << "\n";
        
        std::map<std::string, int> typeCount;
        for(const auto& comp : components) {
            typeCount[comp->getType()]++;
        }
        
        std::cout << "\nComponent Breakdown:\n";
        for(const auto& [type, count] : typeCount) {
            std::cout << "  " << type << ": " << count << " component(s)\n";
        }
        
        std::cout << "\n=== END REPORT ===\n";
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string getSystemHash() const { return systemHash; }
    std::string getMasterCode() const { return masterIdentityCode; }
};

// Utility Functions
class IdentityUtilities {
public:
    static void printHeader() {
        std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║              DIRECT IDENTITY CODE SYSTEM                     ║
║             Unified Identity Management Platform             ║
╚══════════════════════════════════════════════════════════════╝
        )" << '\n';
    }
    
    static void printMenu() {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. Display Full Identity\n";
        std::cout << "2. Verify Identity\n";
        std::cout << "3. Generate Report\n";
        std::cout << "4. Add Test Components\n";
        std::cout << "5. Show System Info\n";
        std::cout << "6. Exit\n";
        std::cout << "Choice: ";
    }
};

// Main Application
int main() {
    IdentityUtilities::printHeader();
    
    // Create the main identity system
    DirectIdentitySystem identitySystem;
    
    // Create some default identity components
    auto biometric = std::make_shared<BiometricData>(
        "user_fingerprint_data_12345",
        "facial_vector_xyz789",
        "iris_pattern_abc456"
    );
    
    auto digitalId = std::make_shared<DigitalIdentity>("john_doe", "user@example.com");
    digitalId->addLinkedAccount("GitHub: johndoe");
    digitalId->addLinkedAccount("LinkedIn: john-doe");
    
    auto govId = std::make_shared<GovernmentID>(
        "NAT123456789",
        "P12345678",
        "DL987654321",
        "United States"
    );
    
    // Add components to the system
    identitySystem.addComponent(biometric);
    identitySystem.addComponent(digitalId);
    identitySystem.addComponent(govId);
    
    int choice = 0;
    
    while(choice != 6) {
        IdentityUtilities::printMenu();
        std::cin >> choice;
        
        switch(choice) {
            case 1:
                identitySystem.displayFullIdentity();
                break;
                
            case 2:
                identitySystem.verifyIdentity();
                break;
                
            case 3:
                identitySystem.generateIdentityReport();
                break;
                
            case 4: {
                // Add additional test components
                auto extraBio = std::make_shared<BiometricData>(
                    "backup_fingerprint_987",
                    "secondary_facial_data",
                    "backup_iris_scan"
                );
                
                auto extraDigital = std::make_shared<DigitalIdentity>("jane_doe", "jane@example.com");
                extraDigital->addLinkedAccount("Twitter: @janedoe");
                
                identitySystem.addComponent(extraBio);
                identitySystem.addComponent(extraDigital);
                
                std::cout << "\n✓ Added test components to identity system.\n";
                break;
            }
                
            case 5:
                std::cout << "\n=== SYSTEM INFORMATION ===\n";
                std::cout << "Master Code: " << identitySystem.getMasterCode() << "\n";
                std::cout << "System Hash: " << identitySystem.getSystemHash() << "\n";
                std::cout << "Timestamp: " << identitySystem.getCurrentTimestamp() << "\n";
                break;
                
            case 6:
                std::cout << "\nExiting Direct Identity Code System...\n";
                break;
                
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
                break;
        }
    }
    
    std::cout << "\nThank you for using the Direct Identity Code System!\n";
    
    return 0;
}
