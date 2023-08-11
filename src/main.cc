#include "./rpapplication.h"
//
//
//
//
// // ---------------------------------------------------------------------------------------
//
// #include <iostream>
// #include <cmath>
// #include <cassert>
// #include <cstddef>
// #include <portaudiocpp/PortAudioCpp.hxx>
//
// // ---------------------------------------------------------------------------------------
//
// // Some constants:
// const int NUM_SECONDS = 5;
// const double SAMPLE_RATE = 44100.0;
// const int FRAMES_PER_BUFFER = 64;
// const int TABLE_SIZE = 200;
//
// // ---------------------------------------------------------------------------------------
//
// // SineGenerator class:
// class SineGenerator
// {
// public:
//     SineGenerator(int tableSize) : tableSize_(tableSize), leftPhase_(0), rightPhase_(0)
//     {
//         const double PI = 3.14159265;
//         table_ = new float[tableSize];
//         for (int i = 0; i < tableSize; ++i)
//         {
//             table_[i] = 0.125f * (float)sin(((double)i/(double)tableSize)*PI*2.);
//         }
//     }
//
//     ~SineGenerator()
//     {
//         delete[] table_;
//     }
//
//     int generate(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
//         const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
//     {
//         assert(outputBuffer != NULL);
//
//         float **out = static_cast<float **>(outputBuffer);
//
//         for (unsigned int i = 0; i < framesPerBuffer; ++i)
//         {
//             out[0][i] = table_[leftPhase_];
//             //out[1][i] = table_[rightPhase_];
//
//             leftPhase_ += 1;
//             if (leftPhase_ >= tableSize_)
//                 leftPhase_ -= tableSize_;
//
//             rightPhase_ += 3;
//             if (rightPhase_ >= tableSize_)
//                 rightPhase_ -= tableSize_;
//         }
//
//         return paContinue;
//     }
//
// private:
//     float *table_;
//     int tableSize_;
//     int leftPhase_;
//     int rightPhase_;
// };
//
// // ---------------------------------------------------------------------------------------
//
// // main:
// int mmain(int, char *[]);
// int mmain(int, char *[])
// {
//     try
//     {
//         // Create a SineGenerator object:
//         SineGenerator sineGenerator(TABLE_SIZE);
//
//         std::cout << "Setting up PortAudio..." << std::endl;
//
//         // Set up the System:
//         portaudio::AutoSystem autoSys;
//         portaudio::System &sys = portaudio::System::instance();
//
//         // Set up the parameters required to open a (Callback)Stream:
//         portaudio::DirectionSpecificStreamParameters outParams(sys.defaultOutputDevice(), 1, portaudio::FLOAT32, false, sys.defaultOutputDevice().defaultLowOutputLatency(), NULL);
//         portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);
//
//         std::cout << "Opening stereo output stream..." << std::endl;
//
//         // Create (and open) a new Stream, using the SineGenerator::generate function as a callback:
//         portaudio::MemFunCallbackStream<SineGenerator> stream(params, sineGenerator, &SineGenerator::generate);
//
//         std::cout << "Starting playback for " << NUM_SECONDS << " seconds." << std::endl;
//
//         // Start the Stream (audio playback starts):
//         stream.start();
//
//         // Wait for 5 seconds:
//         sys.sleep(NUM_SECONDS * 1000);
//
//         std::cout << "Closing stream..." <<std::endl;
//
//         // Stop the Stream (not strictly needed as termintating the System will also stop all open Streams):
//         stream.stop();
//
//         // Close the Stream (not strictly needed as terminating the System will also close all open Streams):
//         stream.close();
//
//         // Terminate the System (not strictly needed as the AutoSystem will also take care of this when it
//         // goes out of scope):
//         sys.terminate();
//
//         std::cout << "Test finished." << std::endl;
//     }
//     catch (const portaudio::PaException &e)
//     {
//         std::cout << "A PortAudio error occurred: " << e.paErrorText() << std::endl;
//     }
//     catch (const portaudio::PaCppException &e)
//     {
//         std::cout << "A PortAudioCpp error occurred: " << e.what() << std::endl;
//     }
//     catch (const std::exception &e)
//     {
//         std::cout << "A generic exception occurred: " << e.what() << std::endl;
//     }
//     catch (...)
//     {
//         std::cout << "An unknown exception occurred." << std::endl;
//     }
//
//     return 0;
// }
//
//
// void printSupportedStandardSampleRates(
//         const portaudio::DirectionSpecificStreamParameters &inputParameters,
//         const portaudio::DirectionSpecificStreamParameters &outputParameters)
// {
//     static double STANDARD_SAMPLE_RATES[] = {
//         8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
//         44100.0, 48000.0, 88200.0, 96000.0, -1 }; // negative terminated list
//
//     int printCount = 0;
//
//     for (int i = 0; STANDARD_SAMPLE_RATES[i] > 0; ++i)
//     {
//         portaudio::StreamParameters tmp = portaudio::StreamParameters(inputParameters, outputParameters, STANDARD_SAMPLE_RATES[i], 0, paNoFlag);
//
//         if (tmp.isSupported())
//         {
//             if (printCount == 0)
//             {
//                 std::cout << "    " << STANDARD_SAMPLE_RATES[i]; // 8.2
//                 printCount = 1;
//             }
//             else if (printCount == 4)
//             {
//                 std::cout << "," << std::endl;
//                 std::cout << "    " << STANDARD_SAMPLE_RATES[i]; // 8.2
//                 printCount = 1;
//             }
//             else
//             {
//                 std::cout << ", " << STANDARD_SAMPLE_RATES[i]; // 8.2
//                 ++printCount;
//             }
//         }
//     }
//
//     if (printCount == 0)
//         std::cout << "None" << std::endl;
//     else
//         std::cout << std::endl;
// }
//
// // ---------------------------------------------------------------------------------------
//
// int main_devices(int, char*[]);
// int main_devices(int, char*[])
//
// {
//     try
//     {
//         portaudio::AutoSystem autoSys;
// 
//         portaudio::System &sys = portaudio::System::instance();
// 
//         std::cout << "PortAudio version number = " << sys.version() << std::endl;
//         std::cout << "PortAudio version text = '" << sys.versionText() << "'" << std::endl;
// 
//         int numDevices = sys.deviceCount();
//         std::cout << "Number of devices = " << numDevices << std::endl;
// 
//         for (portaudio::System::DeviceIterator i = sys.devicesBegin(); i != sys.devicesEnd(); ++i)
//         {
//             std::cout << "--------------------------------------- device #" << (*i).index() << std::endl;
// 
//             // Mark global and API specific default devices:
//             bool defaultDisplayed = false;
// 
//             if ((*i).isSystemDefaultInputDevice())
//             {
//                 std::cout << "[ Default Input";
//                 defaultDisplayed = true;
//             }
//             else if ((*i).isHostApiDefaultInputDevice())
//             {
//                 std::cout << "[ Default " << (*i).hostApi().name() << " Input";
//                 defaultDisplayed = true;
//             }
// 
//             if ((*i).isSystemDefaultOutputDevice())
//             {
//                 std::cout << (defaultDisplayed ? "," : "[");
//                 std::cout << " Default Output";
//                 defaultDisplayed = true;
//             }
//             else if ((*i).isHostApiDefaultOutputDevice())
//             {
//                 std::cout << (defaultDisplayed ? "," : "[");
//                 std::cout << " Default " << (*i).hostApi().name() << " Output";
//                 defaultDisplayed = true;
//             }
// 
//             if (defaultDisplayed)
//                 std::cout << " ]" << std::endl;
// 
//             // Print device info:
//             std::cout << "Name                        = " << (*i).name() << std::endl;
//             std::cout << "Host API                    = " << (*i).hostApi().name() << std::endl;
//             std::cout << "Max inputs = " << (*i).maxInputChannels() << ", Max outputs = " << (*i).maxOutputChannels() << std::endl;
//             continue;
// 
//             std::cout << "Default low input latency   = " << (*i).defaultLowInputLatency() << std::endl;   // 8.3
//             std::cout << "Default low output latency  = " << (*i).defaultLowOutputLatency() << std::endl;  // 8.3
//             std::cout << "Default high input latency  = " << (*i).defaultHighInputLatency() << std::endl;  // 8.3
//             std::cout << "Default high output latency = " << (*i).defaultHighOutputLatency() << std::endl; // 8.3
// 
// #ifdef WIN32
//             // ASIO specific latency information:
//             if ((*i).hostApi().typeId() == paASIO)
//             {
//                 portaudio::AsioDeviceAdapter asioDevice((*i));
// 
//                 std::cout << "ASIO minimum buffer size    = " << asioDevice.minBufferSize() << std::endl;
//                 std::cout << "ASIO maximum buffer size    = " << asioDevice.maxBufferSize() << std::endl;
//                 std::cout << "ASIO preferred buffer size  = " << asioDevice.preferredBufferSize() << std::endl;
// 
//                 if (asioDevice.granularity() == -1)
//                     std::cout << "ASIO buffer granularity     = power of 2" << std::endl;
//                 else
//                     std::cout << "ASIO buffer granularity     = " << asioDevice.granularity() << std::endl;
//             }
// #endif // WIN32
// 
//             std::cout << "Default sample rate         = " << (*i).defaultSampleRate() << std::endl; // 8.2
// 
//             // Poll for standard sample rates:
//             portaudio::DirectionSpecificStreamParameters inputParameters((*i), (*i).maxInputChannels(), portaudio::INT16, true, 0.0, NULL);
//             portaudio::DirectionSpecificStreamParameters outputParameters((*i), (*i).maxOutputChannels(), portaudio::INT16, true, 0.0, NULL);
// 
//             if (inputParameters.numChannels() > 0)
//             {
//                 std::cout << "Supported standard sample rates" << std::endl;
//                 std::cout << " for half-duplex 16 bit " << inputParameters.numChannels() << " channel input = " << std::endl;
//                 printSupportedStandardSampleRates(inputParameters, portaudio::DirectionSpecificStreamParameters::null());
//             }
// 
//             if (outputParameters.numChannels() > 0)
//             {
//                 std::cout << "Supported standard sample rates" << std::endl;
//                 std::cout << " for half-duplex 16 bit " << outputParameters.numChannels() << " channel output = " << std::endl;
//                 printSupportedStandardSampleRates(portaudio::DirectionSpecificStreamParameters::null(), outputParameters);
//             }
// 
//             if (inputParameters.numChannels() > 0 && outputParameters.numChannels() > 0)
//             {
//                 std::cout << "Supported standard sample rates" << std::endl;
//                 std::cout << " for full-duplex 16 bit " << inputParameters.numChannels() << " channel input, " << outputParameters.numChannels() << " channel output = " << std::endl;
//                 printSupportedStandardSampleRates(inputParameters, outputParameters);
//             }
//         }
// 
//         std::cout << "----------------------------------------------" << std::endl;
//     }
//     catch (const portaudio::PaException &e)
//     {
//         std::cout << "A PortAudio error occurred: " << e.paErrorText() << std::endl;
//     }
//     catch (const portaudio::PaCppException &e)
//     {
//         std::cout << "A PortAudioCpp error occurred: " << e.what() << std::endl;
//     }
//     catch (const std::exception &e)
//     {
//         std::cout << "A generic exception occurred: " << e.what() << std::endl;
//     }
//     catch (...)
//     {
//         std::cout << "An unknown exception occurred." << std::endl;
//     }
// 
//     return 0;
// }
int main(int argc, char *argv[])
{
    // main_devices(argc,argv);
    // printf("------------------------------------\n");
    // mmain(argc,argv);
    auto application = RPApplication::create();
    return application->run(argc, argv);
}