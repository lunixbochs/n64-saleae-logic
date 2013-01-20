#ifndef N64_ANALYZER_H
#define N64_ANALYZER_H

#include <Analyzer.h>
#include "N64AnalyzerResults.h"
#include "N64SimulationDataGenerator.h"

class N64AnalyzerSettings;
class ANALYZER_EXPORT N64Analyzer : public Analyzer
{
public:
	N64Analyzer();
	virtual ~N64Analyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< N64AnalyzerSettings > mSettings;
	std::auto_ptr< N64AnalyzerResults > mResults;
	AnalyzerChannelData* mData;

	N64SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //N64_ANALYZER_H
