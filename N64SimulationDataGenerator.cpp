#include "N64SimulationDataGenerator.h"
#include "N64AnalyzerSettings.h"

#include <AnalyzerHelpers.h>

N64SimulationDataGenerator::N64SimulationDataGenerator()
{
}

N64SimulationDataGenerator::~N64SimulationDataGenerator()
{
}

void N64SimulationDataGenerator::Initialize( U32 simulation_sample_rate, N64AnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSimulationData.SetChannel( mSettings->mInputChannel );
	mSimulationData.SetSampleRate( simulation_sample_rate );
	mSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 N64SimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreatePacket();
	}

	*simulation_channel = &mSimulationData;
	return 1;
}

void N64SimulationDataGenerator::CreateByte(unsigned char byte)
{

}

void N64SimulationDataGenerator::CreateBit(int bit)
{
	U32 samples_per_bit = mSimulationSampleRateHz / bitrate;

	mSimulationData.TransitionIfNeeded( BIT_LOW );
	switch (bit) {
		case ZERO_BIT:
			mSimulationData.Advance( samples_per_bit * 3);
			mSimulationData.TransitionIfNeeded( BIT_HIGH );
			mSimulationData.Advance( samples_per_bit );
			break;
		case ONE_BIT:
			mSimulationData.Advance( samples_per_bit );
			mSimulationData.TransitionIfNeeded( BIT_HIGH );
			mSimulationData.Advance( samples_per_bit * 3);
			break;
		case CONSOLE_STOP_BIT:
			mSimulationData.Advance( samples_per_bit );
			mSimulationData.TransitionIfNeeded( BIT_HIGH );
			mSimulationData.Advance( samples_per_bit * 2);
			mSimulationData.TransitionIfNeeded( BIT_LOW );
			mSimulationData.Advance( samples_per_bit);
			break;
		case CONTROLLER_STOP_BIT:
			mSimulationData.Advance( samples_per_bit * 2);
			mSimulationData.TransitionIfNeeded( BIT_HIGH );
			mSimulationData.Advance( samples_per_bit );
			break;
	}
}

void N64SimulationDataGenerator::CreatePacket()
{
	U32 samples_per_bit = mSimulationSampleRateHz / bitrate;

	// simulated console data
	mSimulationData.Advance( samples_per_bit * 10 );

	U8 byte = 0xCC;
	U8 mask = 0x1 << 7;
	for( U32 i=0; i<8; i++ )
	{
		if( ( byte & mask ) != 0 )
			CreateBit(ONE_BIT);
		else
			CreateBit(ZERO_BIT);

		mask >>= 1;
	}

	CreateBit(CONSOLE_STOP_BIT);

	// simulated controller response
	mSimulationData.Advance( samples_per_bit * 10 );

	mask = 0x1 << 7;
	for( U32 i=0; i<8; i++ )
	{
		if( ( byte & mask ) != 0 )
			CreateBit(ONE_BIT);
		else
			CreateBit(ZERO_BIT);

		mask >>= 1;
	}

	CreateBit(CONTROLLER_STOP_BIT);
}
