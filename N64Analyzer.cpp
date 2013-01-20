#include "N64Analyzer.h"
#include "N64AnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <math.h>

N64Analyzer::N64Analyzer()
:	Analyzer(),  
	mSettings( new N64AnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

N64Analyzer::~N64Analyzer()
{
	KillThread();
}

void N64Analyzer::WorkerThread()
{
	mResults.reset( new N64AnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );

	mSampleRateHz = GetSampleRate();
	double samples_per_bit = mSampleRateHz / bitrate;
	U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( bitrate ) );
	mData = GetAnalyzerChannelData( mSettings->mInputChannel );

	if( mData->GetBitState() == BIT_LOW )
		mData->AdvanceToNextEdge();

	int state = CONTROLLER_STOP_BIT;
	bool idle = false;
	for (;;) {
		U64 start, middle, end, first, second;
		int bit = NO_BIT;

		mData->AdvanceToNextEdge();
		start = mData->GetSampleNumber();
		middle = 0;

		// idle + console stop doesn't give us the first half of the signal
		// so we step back to align everything
		if (idle && state == CONSOLE_STOP_BIT) {
			end = mData->GetSampleOfNextEdge();
			first = nearbyint((end - start) / samples_per_bit);
			if (first == 1 || first == 3) {
				middle = start;
				idle = false;

				if (first == 1) {
					start -= samples_per_bit * 3;
				} else if (first == 3) {
					start -= samples_per_bit * 1;
				}
			}
		}

		if (!middle) {
			mData->AdvanceToNextEdge();
			middle = mData->GetSampleNumber();	
		}
		first = nearbyint((middle - start) / samples_per_bit);
		end = std::min(mData->GetSampleOfNextEdge(), (U64)(middle + (4 - first) * samples_per_bit));
		second = nearbyint((end - middle) / samples_per_bit);

		if (bit == NO_BIT) {
			if (first == 1 && second == 3) {
				bit = ONE_BIT;
			} else if (first == 3 && second == 1) {
				bit = ZERO_BIT;
			} else if (first == 1 && second == 2) {
				idle = true;
				state = bit = CONSOLE_STOP_BIT;
				// consume an edge because we're on the wrong side of the cycle
				mData->AdvanceToNextEdge();

			} else if (first == 2 && (second == 1 || second == 2)) {
				if (second == 2)
					end -= samples_per_bit;

				idle = true;
				state = bit = CONTROLLER_STOP_BIT;
			}
		}

		if (bit == NO_BIT)
			continue;

		// add a marker
		U64 halfway = start + (end - start) / 2.0;
		mResults->AddMarker( halfway, AnalyzerResults::Dot, mSettings->mInputChannel );

		Frame frame;
		frame.mData1 = bit;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = start;
		frame.mEndingSampleInclusive = end;

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
	return;

	for( ; ; )
	{
		U8 data = 0;
		U8 mask = 1 << 7;
		
		mData->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		U64 starting_sample = mData->GetSampleNumber();

		mData->Advance( samples_to_first_center_of_first_data_bit );

		for( U32 i=0; i<8; i++ )
		{
			//let's put a dot exactly where we sample this bit:
			mResults->AddMarker( mData->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mInputChannel );

			if( mData->GetBitState() == BIT_HIGH )
				data |= mask;

			mData->Advance( samples_per_bit );

			mask = mask >> 1;
		}


		//we have a byte to save. 
		Frame frame;
		frame.mData1 = data;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = mData->GetSampleNumber();

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
}

bool N64Analyzer::NeedsRerun()
{
	return false;
}

U32 N64Analyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 N64Analyzer::GetMinimumSampleRateHz()
{
	return bitrate * 4;
}

const char* N64Analyzer::GetAnalyzerName() const
{
	return "N64 Controller";
}

const char* GetAnalyzerName()
{
	return "N64 Controller";
}

Analyzer* CreateAnalyzer()
{
	return new N64Analyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
