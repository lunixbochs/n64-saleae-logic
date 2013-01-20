#ifndef N64_ANALYZER_RESULTS
#define N64_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class N64Analyzer;
class N64AnalyzerSettings;

class N64AnalyzerResults : public AnalyzerResults
{
public:
	N64AnalyzerResults( N64Analyzer* analyzer, N64AnalyzerSettings* settings );
	virtual ~N64AnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	N64AnalyzerSettings* mSettings;
	N64Analyzer* mAnalyzer;
};

#endif //N64_ANALYZER_RESULTS
