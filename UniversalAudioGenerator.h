// UniversalAudioGenerator.h
#ifndef UNIVERSAL_AUDIO_GENERATOR_H
#define UNIVERSAL_AUDIO_GENERATOR_H

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <functional>
#include <memory>
#include <fstream>

namespace UniversalAudio {
    
    class AudioSample {
    public:
        std::vector<double> leftChannel;
        std::vector<double> rightChannel;
        double sampleRate;
        
        AudioSample(double rate = 44100.0) : sampleRate(rate) {}
    };
    
    class SoundGenerator {
    protected:
        double sampleRate;
        double frequency;
        double amplitude;
        double phase;
        
    public:
        SoundGenerator(double rate = 44100.0) 
            : sampleRate(rate), frequency(440.0), amplitude(0.5), phase(0.0) {}
        
        virtual ~SoundGenerator() = default;
        
        virtual double generateSample(double time) = 0;
        
        void setFrequency(double freq) { frequency = freq; }
        void setAmplitude(double amp) { amplitude = amp; }
    };
    
    class SineGenerator : public SoundGenerator {
    public:
        SineGenerator(double rate = 44100.0) : SoundGenerator(rate) {}
        
        double generateSample(double time) override {
            return amplitude * std::sin(2.0 * M_PI * frequency * time + phase);
        }
    };
    
    class CulturalMusicEngine {
    private:
        std::map<std::string, std::vector<double>> culturalScales;
        std::map<std::string, std::vector<std::string>> rhythmPatterns;
        
    public:
        CulturalMusicEngine() {
            initializeCulturalScales();
            initializeRhythmPatterns();
        }
        
        void initializeCulturalScales() {
            // Middle Eastern Maqamat
            culturalScales["maqam_rast"] = {1.0, 9.0/8.0, 5.0/4.0, 4.0/3.0, 3.0/2.0, 5.0/3.0, 15.0/8.0, 2.0};
            culturalScales["maqam_bayati"] = {1.0, 9.0/8.0, 6.0/5.0, 4.0/3.0, 3.0/2.0, 27.0/16.0, 16.0/9.0, 2.0};
            
            // South Asian Ragas
            culturalScales["raga_bhairavi"] = {1.0, 16.0/15.0, 6.0/5.0, 4.0/3.0, 3.0/2.0, 8.0/5.0, 9.0/5.0, 2.0};
            
            // Western Scales
            culturalScales["major"] = {1.0, 9.0/8.0, 5.0/4.0, 4.0/3.0, 3.0/2.0, 5.0/3.0, 15.0/8.0, 2.0};
            culturalScales["minor"] = {1.0, 9.0/8.0, 6.0/5.0, 4.0/3.0, 3.0/2.0, 8.0/5.0, 9.0/5.0, 2.0};
        }
        
        void initializeRhythmPatterns() {
            rhythmPatterns["dawr_hindi"] = {"4/4", "Dum", "Dum", "Tak", "Dum", "Tak"};
            rhythmPatterns["maqsum"] = {"4/4", "Dum", "Tak", "Dum", "Tak"};
            rhythmPatterns["samai"] = {"10/8", "Dum", "", "Tak", "Dum", "Tak", "", "Tak"};
            rhythmPatterns["walta"] = {"3/4", "Dum", "Tak", "Tak"};
        }
        
        std::vector<double> getScale(const std::string& scaleName, double baseFreq = 440.0) {
            std::vector<double> scaledFrequencies;
            if (culturalScales.find(scaleName) != culturalScales.end()) {
                for (double ratio : culturalScales[scaleName]) {
                    scaledFrequencies.push_back(baseFreq * ratio);
                }
            }
            return scaledFrequencies;
        }
    };
    
    class SpiritualNasheedGenerator {
    private:
        CulturalMusicEngine musicEngine;
        std::vector<std::string> peacefulThemes;
        std::vector<std::string> spiritualMelodies;
        
    public:
        SpiritualNasheedGenerator() {
            initializePeacefulThemes();
            initializeSpiritualMelodies();
        }
        
        void initializePeacefulThemes() {
            peacefulThemes = {
                "Peace and tranquility",
                "Divine love and mercy", 
                "Spiritual enlightenment",
                "Universal harmony",
                "Inner peace and contemplation",
                "Cosmic unity and balance"
            };
        }
        
        void initializeSpiritualMelodies() {
            spiritualMelodies = {"maqam_rast", "maqam_bayati", "raga_bhairavi"};
        }
        
        AudioSample generatePeacefulNasheed(double duration = 30.0) {
            AudioSample sample;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> themeDist(0, peacefulThemes.size() - 1);
            std::uniform_int_distribution<> melodyDist(0, spiritualMelodies.size() - 1);
            
            std::string selectedTheme = peacefulThemes[themeDist(gen)];
            std::string selectedMelody = spiritualMelodies[melodyDist(gen)];
            
            std::cout << "Generating peaceful nasheed..." << std::endl;
            std::cout << "Theme: " << selectedTheme << std::endl;
            std::cout << "Musical Mode: " << selectedMelody << std::endl;
            
            std::vector<double> frequencies = musicEngine.getScale(selectedMelody, 220.0);
            SineGenerator generator;
            
            int totalSamples = static_cast<int>(duration * sample.sampleRate);
            sample.leftChannel.resize(totalSamples);
            sample.rightChannel.resize(totalSamples);
            
            for (int i = 0; i < totalSamples; ++i) {
                double time = i / sample.sampleRate;
                
                // Complex melodic pattern
                double noteIndex = std::fmod(time * 0.5, frequencies.size());
                double currentFreq = frequencies[static_cast<int>(noteIndex)];
                generator.setFrequency(currentFreq);
                
                // Envelope for natural sound
                double envelope = std::exp(-time * 0.1) * (1.0 - std::exp(-time * 5.0));
                generator.setAmplitude(0.3 * envelope);
                
                double sampleValue = generator.generateSample(time);
                
                // Stereo effect with slight phase difference
                sample.leftChannel[i] = sampleValue * 0.7;
                sample.rightChannel[i] = sampleValue * 0.7 * std::cos(time * 0.1);
            }
            
            return sample;
        }
    };
    
    class QuranicRecitationHelper {
    private:
        std::map<std::string, std::vector<double>> tajweedRules;
        std::vector<std::string> recitationStyles;
        
    public:
        QuranicRecitationHelper() {
            initializeTajweedRules();
            initializeRecitationStyles();
        }
        
        void initializeTajweedRules() {
            tajweedRules["madd"] = {1.5, 2.0, 2.5, 4.0, 6.0}; // Extension durations
            tajweedRules["ghunnah"] = {0.1, 0.15, 0.2}; // Nasalization
            tajweedRules["qalqalah"] = {0.05, 0.08, 0.12}; // Echo effect
        }
        
        void initializeRecitationStyles() {
            recitationStyles = {"Murattal", "Mujawwad", "Mu'allim", "Tahqiq"};
        }
        
        AudioSample generateRecitationExample(double duration = 20.0) {
            AudioSample sample;
            std::cout << "Generating educational Quranic recitation example..." << std::endl;
            std::cout << "Style: " << recitationStyles[0] << std::endl;
            
            SineGenerator baseTone;
            baseTone.setFrequency(110.0);
            baseTone.setAmplitude(0.2);
            
            int totalSamples = static_cast<int>(duration * sample.sampleRate);
            sample.leftChannel.resize(totalSamples);
            sample.rightChannel.resize(totalSamples);
            
            for (int i = 0; i < totalSamples; ++i) {
                double time = i / sample.sampleRate;
                
                // Simulate melodic recitation patterns
                double melodicFreq = 110.0 + 20.0 * std::sin(time * 2.0 * M_PI * 0.3);
                baseTone.setFrequency(melodicFreq);
                
                // Apply tajweed-like variations
                double tajweedEffect = 1.0 + 0.1 * std::sin(time * 2.0 * M_PI * 2.0);
                double amplitude = 0.2 * tajweedEffect * std::exp(-time * 0.05);
                baseTone.setAmplitude(amplitude);
                
                double sampleValue = baseTone.generateSample(time);
                sample.leftChannel[i] = sampleValue;
                sample.rightChannel[i] = sampleValue * 0.9;
            }
            
            return sample;
        }
    };
    
    class GameSoundtrackGenerator {
    private:
        std::map<std::string, std::vector<double>> emotionProfiles;
        CulturalMusicEngine musicEngine;
        
    public:
        GameSoundtrackGenerator() {
            initializeEmotionProfiles();
        }
        
        void initializeEmotionProfiles() {
            emotionProfiles["peaceful"] = {0.1, 0.3, 0.5, 0.7}; // Calm, rising
            emotionProfiles["epic"] = {0.8, 0.9, 0.7, 0.95}; // Intense, varying
            emotionProfiles["mysterious"] = {0.4, 0.3, 0.6, 0.2}; // Unpredictable
            emotionProfiles["victorious"] = {0.6, 0.8, 0.9, 1.0}; // Building to climax
        }
        
        AudioSample generateSoundtrack(const std::string& emotion, double duration = 60.0) {
            AudioSample sample;
            std::cout << "Generating " << emotion << " game soundtrack..." << std::endl;
            
            std::vector<double> frequencies = musicEngine.getScale("major", 110.0);
            SineGenerator mainMelody;
            SineGenerator harmony;
            SineGenerator bass;
            
            int totalSamples = static_cast<int>(duration * sample.sampleRate);
            sample.leftChannel.resize(totalSamples);
            sample.rightChannel.resize(totalSamples);
            
            for (int i = 0; i < totalSamples; ++i) {
                double time = i / sample.sampleRate;
                
                // Dynamic composition based on emotion
                double progress = time / duration;
                double intensity = emotionProfiles[emotion][static_cast<int>(progress * 4) % 4];
                
                // Main melody
                double melFreq = frequencies[static_cast<int>(time * 2) % frequencies.size()];
                mainMelody.setFrequency(melFreq);
                mainMelody.setAmplitude(0.3 * intensity);
                
                // Harmony
                double harmFreq = melFreq * 1.5;
                harmony.setFrequency(harmFreq);
                harmony.setAmplitude(0.2 * intensity);
                
                // Bass
                bass.setFrequency(melFreq * 0.5);
                bass.setAmplitude(0.4 * intensity);
                
                double mixedSample = mainMelody.generateSample(time) + 
                                   harmony.generateSample(time) + 
                                   bass.generateSample(time);
                
                sample.leftChannel[i] = mixedSample * 0.8;
                sample.rightChannel[i] = mixedSample * 0.8;
            }
            
            return sample;
        }
    };
    
    class UniversalAudioGenerator {
    private:
        SpiritualNasheedGenerator nasheedGen;
        QuranicRecitationHelper recitationHelper;
        GameSoundtrackGenerator soundtrackGen;
        CulturalMusicEngine cultureEngine;
        
    public:
        void demonstrateAllGenerators() {
            std::cout << "=== UNIVERSAL AUDIO GENERATION SYSTEM ===" << std::endl;
            std::cout << "Comprehensive Cultural & Spiritual Audio Framework" << std::endl;
            std::cout << "=============================================" << std::endl;
            
            // Generate examples of each type
            std::cout << "\n1. Generating Peaceful Spiritual Nasheed..." << std::endl;
            auto nasheed = nasheedGen.generatePeacefulNasheed(15.0);
            
            std::cout << "\n2. Generating Educational Quranic Recitation..." << std::endl;
            auto recitation = recitationHelper.generateRecitationExample(12.0);
            
            std::cout << "\n3. Generating Cultural Music Examples..." << std::endl;
            auto middleEasternScale = cultureEngine.getScale("maqam_rast");
            std::cout << "Middle Eastern Maqam Scale: ";
            for (double freq : middleEasternScale) {
                std::cout << freq << "Hz ";
            }
            std::cout << std::endl;
            
            std::cout << "\n4. Generating Game Soundtrack..." << std::endl;
            auto soundtrack = soundtrackGen.generateSoundtrack("epic", 20.0);
            
            std::cout << "\n=== GENERATION COMPLETE ===" << std::endl;
            std::cout << "All audio samples created successfully!" << std::endl;
            std::cout << "This system demonstrates the universal beauty of" << std::endl;
            std::cout << "spiritual, cultural, and artistic expression." << std::endl;
        }
    };
}

#endif
