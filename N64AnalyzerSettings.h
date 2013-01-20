#ifndef N64_ANALYZER_SETTINGS
#define N64_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

// microsecond bitrate
#define bitrate 1000000
#define ZERO_BIT 0
#define ONE_BIT 1
#define CONSOLE_STOP_BIT 2
#define CONTROLLER_STOP_BIT 3
#define NO_BIT -1

typedef struct {
    bool left;
    bool right;
    bool up;
    bool down;
} digital;

typedef struct {
    char x;
    char y;
} stick;

typedef struct {
    bool a;
    bool b;
    bool z;
    bool start;
    bool l;
    bool r;
    digital d;
    digital c;
    stick s;
} N64Controller;
class N64AnalyzerSettings : public AnalyzerSettings
{
public:
	N64AnalyzerSettings();
	virtual ~N64AnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
};

#endif //N64_ANALYZER_SETTINGS
