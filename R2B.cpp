#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>

const double carrierFrequency = 1000000;
const double modSignalFrequency0 = 1000;
const double modSignalFrequency1 = 2000;
const double samplingRate = 5000000;
const double bitDuration = 0.01;
const double modulationIndex = 0.5;

std::vector<double> generateAMBit(double carrierFreq, double modFreq, double samplingRate, double bitDuration, double modulationIndex) {
    int sampleCount = static_cast<int>(samplingRate * bitDuration);
    std::vector<double> signal(sampleCount);
    for(int i = 0; i < sampleCount; ++i) {
        double t = i / samplingRate;
        double modulatingSignal = sin(2 * M_PI * modFreq * t);
        double carrierSignal = cos(2 * M_PI * carrierFreq * t);
        signal[i] = (1 + modulationIndex * modulatingSignal) * carrierSignal;
    }
    return signal;
}

// Function to generate an AM signal for a given letter (returns the modulating signal for clarity)
std::vector<double> generateAMSignalForLetter(char letter, double carrierFreq, double modFreq0, double modFreq1, double samplingRate, double bitDuration, double modulationIndex) {
     std::bitset<8> bits(static_cast<unsigned long long>(letter));
    std::vector<double> signal;
    
    // Store the modulating signal for later comparison
    std::vector<double> modulatingSignal;

    for (size_t i = 0; i < bits.size(); ++i) {
        double modFreq = bits.test(i) ? modFreq1 : modFreq0;
        auto bitSignal = generateAMBit(carrierFreq, modFreq, samplingRate, bitDuration, modulationIndex);
        
        // Extract and store the modulating signal before adding to the carrier
        for (double sample : bitSignal) {
            modulatingSignal.push_back(sample / carrierSignal - 1 / modulationIndex);
        }

        signal.insert(signal.end(), bitSignal.begin(), bitSignal.end());
    }

    return modulatingSignal; // Return modulating signal instead of the modulated signal
}

char demodulateAMSignal(const std::vector<double>& signal, double samplingRate, double bitDuration, double modFreq0, double modFreq1) {
    std::bitset<8> bits;
    int sampleCount = static_cast<int>(samplingRate * bitDuration);

    for(size_t i = 0; i < signal.size(); i += sampleCount) {
        double avg = 0;
        for(int j = 0; j < sampleCount; ++j) {
            avg += signal[i + j];
        }
        avg /= sampleCount;

        // Determine the bit based on the average frequency
        bits[i / sampleCount] = (avg > (modFreq0 + modFreq1) / 2);
    }

    return static_cast<char>(bits.to_ulong());
}

int main() {
    char letter = 'A';
    std::vector<double> modulatingSignal = generateAMSignalForLetter(letter, carrierFrequency, modSignalFrequency0, modSignalFrequency1, samplingRate, bitDuration, modulationIndex);

    std::cout << "Modulating Signal for letter '" << letter << "' (first 10 samples):" << std::endl;
    for(int i = 0; i < 10 && i < modulatingSignal.size(); ++i) {
        std::cout << modulatingSignal[i] << " ";
    }
    std::cout << std::endl;

    char recoveredLetter = demodulateAMSignal(modulatingSignal, samplingRate, bitDuration, modSignalFrequency0, modSignalFrequency1);
    std::cout << "Recovered letter: " << recoveredLetter << std::endl;

    return 0;
}