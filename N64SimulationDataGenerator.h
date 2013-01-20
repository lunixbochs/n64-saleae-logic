#ifndef N64_SIMULATION_DATA_GENERATOR
#define N64_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
#include "N64AnalyzerSettings.h"

class N64SimulationDataGenerator
{
public:
	N64SimulationDataGenerator();
	~N64SimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, N64AnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	N64AnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateByte(unsigned char byte);
	void CreateBit(int bit);
	void CreatePacket();
	N64Controller controller;

	SimulationChannelDescriptor mSimulationData;

};
#endif //N64_SIMULATION_DATA_GENERATOR
