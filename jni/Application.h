
#include <jni.h>

#include "libpng/png.h"

#include "common.h"
#include "GraphicsDriver.h"
#include "InputHandler.h"

extern "C"
{
#include "rewind.h"
}

#include "genplusgx/types.h"

#define TURBO_FRAMES_TO_EMULATE 10

#define GENPLUS_RENDER_TEXTURE_WIDTH  320.0
#define GENPLUS_RENDER_TEXTURE_HEIGHT 224.0
#define SCREEN_RENDER_TEXTURE_WIDTH  512.0
#define SCREEN_RENDER_TEXTURE_HEIGHT 512.0
#define SCREEN_RENDER_BYTE_BY_PIXEL  2

enum Buttons
{
     BUTTON_INDEX_A,
     BUTTON_INDEX_B,
     BUTTON_INDEX_C,
     BUTTON_INDEX_X,
     BUTTON_INDEX_Y,
     BUTTON_INDEX_Z,
     BUTTON_INDEX_START,
     BUTTON_INDEX_LEFT,
     BUTTON_INDEX_UP,
     BUTTON_INDEX_RIGHT,
     BUTTON_INDEX_DOWN,
     BUTTON_INDEX_COUNT
};


class Application
{
public:
     Application();
     ~Application();

     int init(JNIEnv *env, const char* apkPath);
     int initGraphics();
     int initAudioBuffers(const int sizeInSamples);
     void setAudioSampleRate(int rate);
     void destroy();

     int setPaths(const char* externalStorageDir, const char* romDir,
         		 	 	 const char* stateDir, const char* sramDir,
         		 	 	 const char* cheatsDir);

     void processInput();

     int loadROM(const char* filename);
     void closeROM();
     void resetROM();

     void saveSRam(const char* romName);
     void loadSRam(const char* romName);
     void saveState(const int i);
     void loadState(const int i);
     void selectState(const int i);

     void step(const JNIEnv *env);
     void draw(const JNIEnv *env);

     void setAudioEnabled(const bool b);
     size_t readAudio(jshort* in, const size_t samples);
     size_t getAudioBufferSize() { return _audioBufferSize; }
     size_t getCurrentEmuSamples() { return _ssize; }
     unsigned char* getAudioBuffer();

     void setFrameSkip(int i)
     {
          if (i < 0) i = 0;
          if (i > 9) i = 9;

          _frameSkip = i;
     }

     void setRewind(bool b);

     bool isRomLoaded() { return _romLoaded; }
     bool isInitialized() { return _initialized; }
     bool isAudioInit() { return _audioInitialized; }

     void setGenPlusVideoChanged();

     GraphicsDriver Graphics;
     InputHandler Input;
private:
     bool _initialized;
     bool _romLoaded;
     bool _fceuInitialized;
     bool _audioInitialized;

     double _timeStart;
     double _timeEnd;
     double _timeDelta;

     int _frameSkip;
     int _frameSkipCount;

     char* _apkPath;
     char* _stateDir;
     char* _sramDir;
     char* _currentRom;

     // video
     int _viewPortW;
     int _viewPortH;

     // audio
     int32 _ssize;
     int _sampleRate;
     size_t _audioBufferSize;

     int makePath(const char* dirName, char* outBuffer);
};
