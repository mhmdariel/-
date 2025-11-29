// QNET.main.cpp ﷲ
#include "QuranicDevelopmentFramework.h"
#include <iostream>

using namespace QuranicFramework;

int main() {
    std::cout << "🕌 Quranic Personal Development System 🕌\n";
    std::cout << "========================================\n";
    std::cout << "بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ\n\n";
    
    QuranicDevelopmentEngine engine;
    QuranicAudioMeditation meditation;
    
    int choice;
    do {
        std::cout << "\n📚 Main Menu:\n";
        std::cout << "1. Display Quranic Principles\n";
        std::cout << "2. Generate Development Plan\n";
        std::cout << "3. Character Analysis\n";
        std::cout << "4. Progress Report\n";
        std::cout << "5. Meditation Session\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;
        
        switch(choice) {
            case 1:
                engine.displayQuranicPrinciples();
                break;
            case 2:
                engine.generatePersonalDevelopmentPlan();
                break;
            case 3:
                engine.analyzeCharacterDevelopment();
                break;
            case 4:
                engine.generateSpiritualProgressReport();
                break;
            case 5:
                meditation.generateMeditationSession();
                break;
            case 0:
                std::cout << "\n🎉 May your development be blessed and accepted! 🎉\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    } while(choice != 0);
    
    return 0;
}
