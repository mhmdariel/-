#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <random>

// Islamic Studies Educational Module
class QuranicVerse {
private:
    std::string surahName;
    int surahNumber;
    int verseNumber;
    std::string arabicText;
    std::string translation;
    std::string context;
    std::string themes;

public:
    QuranicVerse(const std::string& surah, int surahNum, int verseNum, 
                 const std::string& arabic, const std::string& trans,
                 const std::string& ctx, const std::string& thms)
        : surahName(surah), surahNumber(surahNum), verseNumber(verseNum),
          arabicText(arabic), translation(trans), context(ctx), themes(thms) {}

    void displayVerse() const {
        std::cout << "\n════════════════════════════════════════════\n";
        std::cout << "📖 Quranic Reference: " << surahName << " (" << surahNumber 
                  << ":" << verseNumber << ")\n";
        std::cout << "════════════════════════════════════════════\n";
        std::cout << "Arabic: " << arabicText << "\n";
        std::cout << "Translation: " << translation << "\n";
        std::cout << "\nContext: " << context << "\n";
        std::cout << "Themes: " << themes << "\n";
        std::cout << "════════════════════════════════════════════\n";
    }

    std::string getReference() const {
        return surahName + " " + std::to_string(surahNumber) + 
               ":" + std::to_string(verseNumber);
    }

    const std::string& getThemes() const { return themes; }
    const std::string& getSurahName() const { return surahName; }
};

class HadithStudy {
private:
    std::string collection;
    std::string narrator;
    std::string arabicText;
    std::string translation;
    std::string grading;
    std::string lessons;

public:
    HadithStudy(const std::string& col, const std::string& narr,
                const std::string& arabic, const std::string& trans,
                const std::string& grade, const std::string& lesson)
        : collection(col), narrator(narr), arabicText(arabic),
          translation(trans), grading(grade), lessons(lesson) {}

    void displayHadith() const {
        std::cout << "\n────────────────────────────────────────────\n";
        std::cout << "📚 Hadith Study\n";
        std::cout << "Collection: " << collection << "\n";
        std::cout << "Narrator: " << narrator << "\n";
        std::cout << "Grading: " << grading << "\n";
        std::cout << "\nArabic: " << arabicText << "\n";
        std::cout << "Translation: " << translation << "\n";
        std::cout << "\nKey Lessons: " << lessons << "\n";
        std::cout << "────────────────────────────────────────────\n";
    }
};

class IslamicConcept {
private:
    std::string conceptName;
    std::string definition;
    std::string quranicBasis;
    std::string practicalApplication;
    std::vector<std::string> relatedVerses;

public:
    IslamicConcept(const std::string& name, const std::string& def,
                   const std::string& basis, const std::string& application)
        : conceptName(name), definition(def), quranicBasis(basis),
          practicalApplication(application) {}

    void addRelatedVerse(const std::string& verse) {
        relatedVerses.push_back(verse);
    }

    void displayConcept() const {
        std::cout << "\n✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦\n";
        std::cout << "Concept: " << conceptName << "\n";
        std::cout << "✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦\n";
        std::cout << "\nDefinition: " << definition << "\n";
        std::cout << "\nQuranic Basis: " << quranicBasis << "\n";
        std::cout << "\nPractical Application: " << practicalApplication << "\n";
        
        if (!relatedVerses.empty()) {
            std::cout << "\nRelated Quranic Verses:\n";
            for (const auto& verse : relatedVerses) {
                std::cout << "  • " << verse << "\n";
            }
        }
        std::cout << "✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦✦\n";
    }
};

class IslamicStudiesModule {
private:
    std::vector<QuranicVerse> verses;
    std::vector<HadithStudy> hadiths;
    std::vector<IslamicConcept> concepts;
    std::map<std::string, std::vector<int>> themeIndex;
    std::map<std::string, std::vector<int>> surahIndex;

public:
    IslamicStudiesModule() {
        initializeEducationalContent();
    }

private:
    void initializeEducationalContent() {
        // Sample educational content - In a real application, this would come from a database
        // Note: Actual Quranic text would need proper licensing and verification
        
        // Mercy and Compassion
        verses.emplace_back(
            "Al-Fatihah", 1, 1,
            "بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ",
            "In the name of Allah, the Entirely Merciful, the Especially Merciful",
            "Opening verse of the Quran establishing God's attributes of mercy",
            "Mercy, Compassion, Divine Attributes"
        );
        
        verses.emplace_back(
            "Al-Baqarah", 2, 163,
            "وَإِلَٰهُكُمْ إِلَٰهٌ وَاحِدٌ ۖ لَّا إِلَٰهَ إِلَّا هُوَ الرَّحْمَٰنُ الرَّحِيمُ",
            "And your god is one God. There is no deity except Him, the Entirely Merciful, the Especially Merciful",
            "Emphasis on monotheism and divine mercy",
            "Tawhid, Mercy, Unity of God"
        );
        
        // Justice and Equity
        verses.emplace_back(
            "An-Nisa", 4, 135,
            "يَا أَيُّهَا الَّذِينَ آمَنُوا كُونُوا قَوَّامِينَ بِالْقِسْطِ شُهَدَاءَ لِلَّهِ",
            "O you who have believed, be persistently standing firm in justice, witnesses for Allah",
            "Command to uphold justice even against personal interests",
            "Justice, Integrity, Testimony"
        );
        
        // Knowledge and Wisdom
        verses.emplace_back(
            "Al-Alaq", 96, 1,
            "اقْرَأْ بِاسْمِ رَبِّكَ الَّذِي خَلَقَ",
            "Read in the name of your Lord who created",
            "First revelation emphasizing the importance of knowledge",
            "Knowledge, Education, Revelation"
        );
        
        // Peace and Reconciliation
        verses.emplace_back(
            "Al-Hujurat", 49, 13,
            "يَا أَيُّهَا النَّاسُ إِنَّا خَلَقْنَاكُم مِّن ذَكَرٍ وَأُنثَىٰ وَجَعَلْنَاكُمْ شُعُوبًا وَقَبَائِلَ لِتَعَارَفُوا",
            "O mankind, indeed We have created you from male and female and made you peoples and tribes that you may know one another",
            "Promotion of mutual understanding among diverse peoples",
            "Unity, Diversity, Human Relations"
        );
        
        // Initialize Hadith studies
        hadiths.emplace_back(
            "Sahih al-Bukhari",
            "Abu Huraira",
            "الرَّاحِمُونَ يَرْحَمُهُمْ الرَّحْمَنُ",
            "Those who are merciful will be shown mercy by the Most Merciful",
            "Sahih (Authentic)",
            "Importance of showing mercy to others as a reflection of divine mercy"
        );
        
        hadiths.emplace_back(
            "Sahih Muslim",
            "Abdullah ibn Amr",
            "لاَ يُؤْمِنُ أَحَدُكُمْ حَتَّى يُحِبَّ لأَخِيهِ مَا يُحِبُّ لِنَفْسِهِ",
            "None of you truly believes until he loves for his brother what he loves for himself",
            "Sahih (Authentic)",
            "Golden rule of Islam, emphasizing empathy and mutual care"
        );
        
        // Initialize Islamic Concepts
        concepts.emplace_back(
            "Tawhid",
            "The fundamental Islamic concept of monotheism - the oneness and unity of God",
            "Al-Ikhlas 112:1-4",
            "Affects every aspect of a Muslim's life, worship, and worldview"
        );
        concepts.back().addRelatedVerse("Al-Baqarah 2:163");
        concepts.back().addRelatedVerse("Al-Ikhlas 112:1-4");
        
        concepts.emplace_back(
            "Rahma",
            "Divine mercy and compassion, one of God's primary attributes",
            "Al-Fatihah 1:1-3",
            "Should be reflected in human interactions and governance"
        );
        concepts.back().addRelatedVerse("Al-Fatihah 1:1");
        concepts.back().addRelatedVerse("Al-Baqarah 2:163");
        
        concepts.emplace_back(
            "Adl",
            "Justice and equity, a cornerstone of Islamic ethics",
            "An-Nisa 4:135",
            "Mandates fair treatment in all affairs, personal and societal"
        );
        concepts.back().addRelatedVerse("An-Nisa 4:135");
        concepts.back().addRelatedVerse("Al-Ma'idah 5:8");
        
        // Build indices for search functionality
        buildIndices();
    }
    
    void buildIndices() {
        for (int i = 0; i < verses.size(); ++i) {
            std::string themes = verses[i].getThemes();
            std::istringstream iss(themes);
            std::string theme;
            while (iss >> theme) {
                if (theme.back() == ',') theme.pop_back();
                themeIndex[theme].push_back(i);
            }
            surahIndex[verses[i].getSurahName()].push_back(i);
        }
    }

public:
    void displayDailyVerse() {
        std::cout << "\n🕌 DAILY QURANIC REFLECTION 🕌\n";
        std::cout << "Date: " << getCurrentDate() << "\n";
        
        // Get a different verse each day
        auto now = std::chrono::system_clock::now();
        auto days = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count() / 24;
        int index = days % verses.size();
        
        verses[index].displayVerse();
        
        std::cout << "\n📝 Reflection Prompt:\n";
        std::cout << "How can you apply the principles of ";
        
        // Extract themes for reflection
        std::string themes = verses[index].getThemes();
        std::istringstream iss(themes);
        std::string theme;
        std::vector<std::string> themeList;
        while (iss >> theme) {
            if (theme.back() == ',') theme.pop_back();
            themeList.push_back(theme);
        }
        
        for (size_t i = 0; i < themeList.size(); ++i) {
            if (i > 0) {
                if (i == themeList.size() - 1) std::cout << " and ";
                else std::cout << ", ";
            }
            std::cout << themeList[i];
        }
        std::cout << " in your daily life today?\n";
    }
    
    void studyByTheme(const std::string& theme) {
        std::cout << "\n🔍 STUDYING THEME: " << theme << "\n";
        
        if (themeIndex.find(theme) != themeIndex.end()) {
            std::cout << "Found " << themeIndex[theme].size() << " related verses:\n";
            for (int idx : themeIndex[theme]) {
                verses[idx].displayVerse();
            }
        } else {
            std::cout << "No verses found for theme: " << theme << "\n";
            std::cout << "Available themes include: Mercy, Justice, Knowledge, Unity, Compassion\n";
        }
    }
    
    void studySurah(const std::string& surahName) {
        std::cout << "\n📚 STUDYING SURAH: " << surahName << "\n";
        
        if (surahIndex.find(surahName) != surahIndex.end()) {
            for (int idx : surahIndex[surahName]) {
                verses[idx].displayVerse();
            }
        } else {
            std::cout << "No verses found for Surah: " << surahName << "\n";
        }
    }
    
    void exploreIslamicConcepts() {
        std::cout << "\n💡 EXPLORING ISLAMIC CONCEPTS 💡\n";
        for (const auto& concept : concepts) {
            concept.displayConcept();
        }
    }
    
    void hadithStudies() {
        std::cout << "\n📖 HADITH STUDIES 📖\n";
        std::cout << "Studying Prophetic Traditions\n";
        
        for (const auto& hadith : hadiths) {
            hadith.displayHadith();
        }
    }
    
    void interactiveLearningSession() {
        std::cout << "\n🎓 INTERACTIVE LEARNING SESSION 🎓\n";
        std::cout << "Answer these reflection questions based on Islamic teachings:\n\n";
        
        std::vector<std::pair<std::string, std::vector<std::string>>> questions = {
            {
                "How does the concept of 'Rahma' (Divine Mercy) influence interpersonal relationships?",
                {"Showing forgiveness", "Practicing empathy", "Helping those in need", "Being patient with others"}
            },
            {
                "What are practical ways to implement 'Adl' (Justice) in daily life?",
                {"Being fair in speech", "Equal treatment of people", "Honest transactions", "Standing against oppression"}
            },
            {
                "How does 'Tawhid' (Oneness of God) shape a Muslim's worldview?",
                {"Unity of creation", "Purpose in life", "Accountability", "Trust in divine wisdom"}
            }
        };
        
        for (const auto& [question, points] : questions) {
            std::cout << "\nQ: " << question << "\n";
            std::cout << "Consider these points:\n";
            for (const auto& point : points) {
                std::cout << "  • " << point << "\n";
            }
            std::cout << "\n[Press Enter to continue...]";
            std::cin.ignore();
        }
    }

private:
    std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
        return ss.str();
    }
};

class EducationalProgressTracker {
private:
    struct StudySession {
        std::string date;
        std::string topic;
        int duration; // in minutes
        std::string reflections;
    };
    
    std::vector<StudySession> sessions;
    std::map<std::string, int> topicStudyTime;
    
public:
    void recordSession(const std::string& topic, int duration, const std::string& reflections) {
        StudySession session;
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M");
        session.date = ss.str();
        session.topic = topic;
        session.duration = duration;
        session.reflections = reflections;
        
        sessions.push_back(session);
        topicStudyTime[topic] += duration;
    }
    
    void displayProgressReport() {
        std::cout << "\n📊 EDUCATIONAL PROGRESS REPORT 📊\n";
        std::cout << "Total Study Sessions: " << sessions.size() << "\n";
        
        int totalMinutes = 0;
        for (const auto& [topic, minutes] : topicStudyTime) {
            totalMinutes += minutes;
        }
        
        std::cout << "Total Study Time: " << totalMinutes << " minutes\n";
        std::cout << "\nTime by Topic:\n";
        for (const auto& [topic, minutes] : topicStudyTime) {
            std::cout << "  " << topic << ": " << minutes << " minutes\n";
        }
        
        if (!sessions.empty()) {
            std::cout << "\nRecent Reflections:\n";
            int count = 0;
            for (auto it = sessions.rbegin(); it != sessions.rend() && count < 3; ++it, ++count) {
                std::cout << "  " << it->date << " - " << it->topic << "\n";
                std::cout << "  Reflection: " << it->reflections.substr(0, 100) << "...\n";
            }
        }
    }
};

// Main Islamic Studies Application
int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                ISLAMIC STUDIES EDUCATIONAL MODULE            ║\n";
    std::cout << "║          Academic Study of Quranic Principles and Ethics     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    IslamicStudiesModule islamicStudies;
    EducationalProgressTracker progressTracker;
    
    int choice;
    bool continueLearning = true;
    
    while (continueLearning) {
        std::cout << "\n════════════════════════════════════════════════════\n";
        std::cout << "              LEARNING MENU\n";
        std::cout << "════════════════════════════════════════════════════\n";
        std::cout << "1. Daily Quranic Reflection\n";
        std::cout << "2. Study by Theme (Mercy, Justice, Knowledge, etc.)\n";
        std::cout << "3. Study Islamic Concepts\n";
        std::cout << "4. Hadith Studies\n";
        std::cout << "5. Interactive Learning Session\n";
        std::cout << "6. View Progress Report\n";
        std::cout << "7. Exit\n";
        std::cout << "════════════════════════════════════════════════════\n";
        std::cout << "Enter your choice (1-7): ";
        std::cin >> choice;
        std::cin.ignore(); // Clear newline
        
        switch(choice) {
            case 1: {
                islamicStudies.displayDailyVerse();
                std::string reflection;
                std::cout << "\nEnter your personal reflection (or press Enter to skip): ";
                std::getline(std::cin, reflection);
                if (!reflection.empty()) {
                    progressTracker.recordSession("Daily Reflection", 15, reflection);
                }
                break;
            }
            case 2: {
                std::cout << "\nEnter theme to study: ";
                std::string theme;
                std::getline(std::cin, theme);
                islamicStudies.studyByTheme(theme);
                progressTracker.recordSession("Theme Study: " + theme, 30, "Studied " + theme + " in Quranic context");
                break;
            }
            case 3:
                islamicStudies.exploreIslamicConcepts();
                progressTracker.recordSession("Islamic Concepts", 45, "Explored core Islamic theological concepts");
                break;
            case 4:
                islamicStudies.hadithStudies();
                progressTracker.recordSession("Hadith Studies", 40, "Studied Prophetic traditions");
                break;
            case 5:
                islamicStudies.interactiveLearningSession();
                progressTracker.recordSession("Interactive Learning", 60, "Participated in reflective learning session");
                break;
            case 6:
                progressTracker.displayProgressReport();
                break;
            case 7:
                continueLearning = false;
                break;
            default:
                std::cout << "Invalid choice. Please enter 1-7.\n";
        }
        
        if (continueLearning) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
        }
    }
    
    std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║            EDUCATIONAL SESSION COMPLETE                      ║\n";
    std::cout << "║  Knowledge is light - may your learning continue to grow     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    // Final progress report
    progressTracker.displayProgressReport();
    
    std::cout << "\nThank you for engaging in Islamic studies education.\n";
    std::cout << "Remember: This is an educational tool for academic study.\n";
    std::cout << "For authoritative religious guidance, consult qualified scholars.\n";
    
    return 0;
}
