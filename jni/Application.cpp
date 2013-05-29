
extern "C"
{

#include "genplusgx/shared.h"

}

#include "unistd.h"

#include "Application.h"

#include "libzip/zip.h"



#define SB_SIZE 2048*2
unsigned char soundbuffer[SB_SIZE];


Application::Application():
		Input(BUTTON_INDEX_COUNT, 1)
{
     _initialized = false;
     _romLoaded = false;
     _fceuInitialized = false;
     _audioInitialized = false;
     _ssize = 0;
     _sampleRate = 22050;

     // paths
     _apkPath = (char*)malloc(MAX_PATH * sizeof(char));
     _stateDir = (char*)malloc(MAX_PATH * sizeof(char));
     _sramDir = (char*)malloc(MAX_PATH * sizeof(char));
     _currentRom = (char*)malloc(MAX_PATH * sizeof(char));
}


Application::~Application()
{
	LOGD("Application::Destructor()");

     destroy();
}


void Application::destroy()
{
     LOGD("Start Application.destroy()");

     // save sram
     saveSRam(_sramDir);

     // shutdown genplus
     system_shutdown();

     // set to false
     _romLoaded = false;

     LOGD("Finished Application.destroy()");
}


void Application::setRewind(bool b)
{
}


static void load_bios(void)
{
	// clear BIOS detection flag
	/*config.tmss &= ~2;

	// open BIOS file
	FILE *fp = fopen(Settings.BIOS.c_str(), "rb");
	if (fp == NULL) return;

	// read file
	fread(bios_rom, 1, 0x800, fp);
	fclose(fp);

	// check ROM file
	if (!strncmp((char *)(bios_rom + 0x120),"GENESIS OS", 10))
		config.tmss |= 2; // valid BIOS detected*/
}


int Application::setPaths(const char* externalStorageDir, const char* romDir,
	 	 	 	 	 	 	 const char* stateDir, const char* sramDir,
	 	 	 	 	 	 	 const char* cheatsDir)
{
	LOGD("EXTERNAL_STORAGE_DIR: %s", externalStorageDir);
	LOGD("ROM_DIR: %s", romDir);
	LOGD("STATE_DIR: %s", stateDir);
	LOGD("SRAM_DIR: %s", sramDir);
	LOGD("CHEATS_DIR: %s", cheatsDir);

	if (externalStorageDir == NULL || strlen(externalStorageDir) >= MAX_PATH)
	{
		return NATIVE_ERROR;
	}

	if (stateDir == NULL || strlen(stateDir) >= MAX_PATH)
	{
		return NATIVE_ERROR;
	}

	strcpy(_stateDir, stateDir);

	if (sramDir == NULL || strlen(sramDir) >= MAX_PATH)
	{
		return NATIVE_ERROR;
	}

	strcpy(_sramDir, sramDir);
}


int Application::init(JNIEnv *env, const char * apkAbsolutePath)
{
     LOGD("APK_PATH: %s", apkAbsolutePath);

     // TODO: setup genesis directories

     // copy dirs for storage
     if (apkAbsolutePath == NULL || strlen(apkAbsolutePath) >= MAX_PATH)
     {
    	 return NATIVE_ERROR;
     }

     strcpy(_apkPath, apkAbsolutePath);

     if (!_fceuInitialized)
     {
    	 // allocate cart
    	 cart.rom = (unsigned char*)memalign(32, MAXROMSIZE);

    	 // load bios
    	 //load_bios();

    	 /* allocate global work bitmap */
    	 memset (&bitmap, 0, sizeof (bitmap));
    	 bitmap.width  = GENPLUS_RENDER_TEXTURE_WIDTH;
    	 bitmap.height = GENPLUS_RENDER_TEXTURE_HEIGHT;
    	 bitmap.depth  = 16;
    	 bitmap.granularity = SCREEN_RENDER_BYTE_BY_PIXEL;
    	 bitmap.pitch = bitmap.width * bitmap.granularity;
    	 bitmap.viewport.w = GENPLUS_RENDER_TEXTURE_WIDTH;
    	 bitmap.viewport.h = GENPLUS_RENDER_TEXTURE_HEIGHT;
    	 bitmap.viewport.x = 0;
    	 bitmap.viewport.y = 0;
    	 bitmap.remap = 1;
    	 bitmap.data = (unsigned char*)malloc(bitmap.width*bitmap.height*bitmap.granularity);

    	 // version TAG
    	 strncpy(config.version,CONFIG_VERSION,16);

    	 // sound options
    	 config.psg_preamp     = 150;
    	 config.fm_preamp      = 100;
    	 config.hq_fm          = 1;
    	 config.psgBoostNoise  = 0;
    	 config.filter         = 1;
    	 config.lp_range       = 50;
    	 config.low_freq       = 880;
    	 config.high_freq      = 5000;
    	 config.lg             = 1.0;
    	 config.mg             = 1.0;
    	 config.hg             = 1.0;
    	 config.rolloff        = 0.995;
    	 config.dac_bits       = 14;

    	 // system options
    	 config.region_detect  = 0;
    	 config.force_dtack    = 0;
    	 config.addr_error     = 0;
    	 config.tmss           = 0;
    	 config.lock_on        = 0;//Settings.ExtraCart;
    	 config.romtype        = 0;
    	 config.hot_swap       = 0;

    	 // video options
    	 config.xshift   = 0;
    	 config.yshift   = 0;
    	 config.xscale   = 0;
    	 config.yscale   = 0;
    	 config.aspect   = 0; // 1 for 4:3, 2 for 16:9
    	 config.overscan = 0;

    	 _fceuInitialized = true;
     }

     _initialized = true;

     return NATIVE_OK;
}


int Application::initAudioBuffers(const int sizeInSamples)
{
     LOGD("initAudioBuffers(%d)", sizeInSamples);

     _audioBufferSize = sizeInSamples;
     _audioInitialized = true;

     return NATIVE_OK;
}


unsigned char* Application::getAudioBuffer()
{
	return soundbuffer;
}


void Application::setGenPlusVideoChanged()
{
	bitmap.viewport.changed |= 1;
}


int Application::initGraphics()
{
     Graphics.Init(SCREEN_RENDER_TEXTURE_WIDTH, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);

     Graphics.InitEmuShader(NULL, NULL);
     Graphics.Clear();

     LOGI("Loading APK %s", _apkPath);
     zip* APKArchive = zip_open(_apkPath, 0, NULL);
     if (APKArchive == NULL)
     {
          LOGE("Error loading APK");
          return NATIVE_ERROR;
     }

     int dPadTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/DirectionalPad.png", &dPadTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int startTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/StartButton.png", &startTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int selectTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/SelectButton.png", &selectTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int cTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonC.png", &cTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int bTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonB.png", &bTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int aTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonA.png", &aTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int xTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonX.png", &xTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int yTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonY.png", &yTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

	  int zTex = 0;
	  if (Graphics.loadPNGTexture(APKArchive, "assets/Textures/ButtonZ.png", &zTex) != GRAPHICS_OK)
	  {
		   return NATIVE_ERROR;
	  }

     zip_close(APKArchive);

     Input.setAnalogTexture(0, dPadTex);
     Input.setButtonTexture(BUTTON_INDEX_C, cTex);
     Input.setButtonTexture(BUTTON_INDEX_B, bTex);
     Input.setButtonTexture(BUTTON_INDEX_A, aTex);
     Input.setButtonTexture(BUTTON_INDEX_X, xTex);
     Input.setButtonTexture(BUTTON_INDEX_Y, yTex);
     Input.setButtonTexture(BUTTON_INDEX_Z, zTex);
     Input.setButtonTexture(BUTTON_INDEX_START, startTex);


     return NATIVE_OK;
}


void Application::setAudioSampleRate(int rate)
{
	bool hardReset = false;
	if (_sampleRate != rate)
	{
		hardReset = true;
	}

	_sampleRate = rate;

	if (_romLoaded && hardReset)
	{
		// TODO: wont catch auto saves
		loadROM(_currentRom);
	}
}


void Application::setAudioEnabled(const bool b)
{
	bool reset = false;
	if (b != snd.enabled)
	{
		reset = true;
	}

	if (b)
	{
		//snd.enabled = 1;

		_audioInitialized = true;
	}
	else
	{
		//snd.enabled = 0;

		_audioInitialized = false;
	}

	if (reset)
	{
		// TODO: wont catch auto saves
		loadROM(_currentRom);
	}
}


void Application::resetROM()
{
	system_reset();
}


int Application::loadROM(const char* filename)
{
	LOGD("NDK:LoadingRom: %s", filename);

	if (_romLoaded)
	{
		// TODO: close genesis

		// save sram
		saveSRam(_sramDir);

		_romLoaded = false;
	}

     /* load ROM file */
   	if (!load_rom((char*)filename))
   	{
   		LOGD("Error loading rom!");
   		return NATIVE_ERROR;
   	}

   	// store current rom
   	strcpy(_currentRom, filename);

   	// controllers options
	config.gun_cursor[0]  = 1;
	config.gun_cursor[1]  = 1;
	config.invert_mouse   = 0;
	config.s_device = 0;

	LOGD("GenesisPlus Hardware: %d", system_hw);

	if (system_hw == SYSTEM_MEGADRIVE || system_hw == SYSTEM_GENESIS)
	{
		Input.setButtonVisibility(BUTTON_INDEX_C, true);
		Input.setButtonVisibility(BUTTON_INDEX_X, true);
		Input.setButtonVisibility(BUTTON_INDEX_Y, true);
		Input.setButtonVisibility(BUTTON_INDEX_Z, true);

		input.system[0]       = SYSTEM_MD_GAMEPAD;
		input.system[1]       = SYSTEM_MD_GAMEPAD;

		config.input[0].padtype = DEVICE_PAD6B;
	}
	else if (system_hw == SYSTEM_PBC)
	{
		Input.setButtonVisibility(BUTTON_INDEX_C, false);
		Input.setButtonVisibility(BUTTON_INDEX_X, false);
		Input.setButtonVisibility(BUTTON_INDEX_Y, false);
		Input.setButtonVisibility(BUTTON_INDEX_Z, false);

		input.system[0]       = SYSTEM_MS_GAMEPAD;
		input.system[1]       = SYSTEM_MS_GAMEPAD;
		config.input[0].padtype = DEVICE_PAD2B;
	}

	input_init();

    // hot-swap previous & current cartridge
   	bool hotswap = config.hot_swap && cart.romsize;
   	//cart.romsize = size;

   	if(hotswap)
	{
   		if (system_hw == SYSTEM_PBC)
   		{
   			sms_cart_init();
   			sms_cart_reset();
   		}
   		else
   		{
   			md_cart_init();
   			md_cart_reset(1);
   		}
	}
   	else
   	{
   		// initialize audio emulation
   		audio_init(_sampleRate, vdp_pal ? 50.0 : 60.0);

   		// system power ON
   		system_init();
   		system_reset();
   	}

   	// sram
   	loadSRam(_currentRom);

   	// mark video updated
   	bitmap.viewport.changed |= 1;

     /* load Cheats */
     //CheatLoad();

   	_ssize = 0;

    _romLoaded = true;

     return NATIVE_OK;
}


void Application::saveState(const int i)
{
     char path[MAX_PATH];
     makePath(_stateDir, path);
     sprintf(path, "%s%d.sav", path, i);

     LOGD("SaveState: %s", path);

     FILE *f = fopen(path,"wb");
     if (f)
     {
          uint8 *buf =(uint8 *)memalign(32,STATE_SIZE);
          int state_size = state_save(buf, 1);
          fwrite(buf, state_size, 1, f);
          fclose(f);
          free(buf);
     }
}


void Application::loadState(const int i)
{
     char path[MAX_PATH];
     makePath(_stateDir, path);
     sprintf(path, "%s%d.sav", path, i);

     LOGD("LoadState: %s", path);

     FILE *f = fopen(path,"rb");
     if (f)
     {
          uint8 *buf =(uint8 *)memalign(32,STATE_SIZE);
          fread(buf, STATE_SIZE, 1, f);
          state_load(buf, 1);
          fclose(f);
          free(buf);
     }
}


void Application::selectState(const int i)
{

}


void Application::loadSRam(const char* romName)
{
	char path[MAX_PATH];
	makePath(_sramDir, path);
	sprintf(path, "%s.srm", path);

	LOGD("Loading SRAM: %s", path);

   	FILE *f = fopen(path,"rb");
   	if (f != NULL)
   	{
   		fread(sram.sram,0x10000,1, f);
         sram.crc = crc32(0, sram.sram, 0x10000);
   		fclose(f);
   	}
}


void Application::saveSRam(const char* romName)
{
	char path[MAX_PATH];
	makePath(_sramDir, path);
	sprintf(path, "%s.srm", path);

	LOGD("Saving SRAM: %s", path);

	FILE *f = fopen(path,"wb");
	if (f!= NULL)
	{
		fwrite(sram.sram,0x10000,1, f);
		fclose(f);
	}
}


int Application::makePath(const char* dirName, char* out)
{
	LOGD("makeDir(%s)", dirName);

	char* dotIndex = strrchr(_currentRom, '.');
	char* slashIndex = strrchr(_currentRom, '/') + 1;

	if (dotIndex == NULL || slashIndex == NULL)
	{
		return NATIVE_ERROR;
	}

	*dotIndex = '\0';

	sprintf(out, "%s/%s", dirName, slashIndex);

	*dotIndex = '.';

	return NATIVE_OK;
}


// consumer, java driven
size_t Application::readAudio(jshort* in, const size_t samples)
{
     if (_ssize >= 0 && _ssize <= SB_SIZE && soundbuffer != NULL)
     {
          memcpy(in, (int16_t*)soundbuffer, _ssize * sizeof(short));
     }

     return _ssize;
}


void Application::processInput()
{
	input.pad[0] = 0;

    if (Input.isButtonDown(BUTTON_INDEX_DOWN) ||
        Input.getAnalogY(0) > Input.getYSensitivity())
    {
    	input.pad[0] |= INPUT_DOWN;
    }

    if (Input.isButtonDown(BUTTON_INDEX_RIGHT) ||
        Input.getAnalogX(0) > Input.getXSensitivity())
    {
    	input.pad[0] |= INPUT_RIGHT;
    }

    if (Input.isButtonDown(BUTTON_INDEX_LEFT) ||
        Input.getAnalogX(0) < -Input.getXSensitivity())
    {
    	input.pad[0] |= INPUT_LEFT;
    }

    if (Input.isButtonDown(BUTTON_INDEX_UP) ||
        Input.getAnalogY(0) < -Input.getYSensitivity())
    {
    	input.pad[0] |= INPUT_UP;
    }

	if (Input.isButtonDown(BUTTON_INDEX_A))
	{
		if (system_hw == SYSTEM_PBC)
		{
			input.pad[0] |= INPUT_BUTTON2;
		}
		else
		{
			input.pad[0] |= INPUT_A;
		}
	}

	if (Input.isButtonDown(BUTTON_INDEX_B))
	{
		if (system_hw == SYSTEM_PBC)
		{
			input.pad[0] |= INPUT_BUTTON1;
		}
		else
		{
			input.pad[0] |= INPUT_B;
		}
	}

	if (Input.isButtonDown(BUTTON_INDEX_C))
	{
		input.pad[0] |= INPUT_C;
	}

	if (Input.isButtonDown(BUTTON_INDEX_X))
	{
		input.pad[0] |= INPUT_X;
	}

	if (Input.isButtonDown(BUTTON_INDEX_Y))
	{
		input.pad[0] |= INPUT_Y;
	}

	if (Input.isButtonDown(BUTTON_INDEX_Z))
	{
		input.pad[0] |= INPUT_Z;
	}

	if (Input.isButtonDown(BUTTON_INDEX_START))
	{
		input.pad[0] |= INPUT_START;
	}


     Input.update();
}


void Application::step(const JNIEnv *env)
{
     // quick exit if no rom loaded to step
     if (!_romLoaded)
     {
          return;
     }

     processInput();

     // calculate frameskipping
     _frameSkipCount=(_frameSkipCount+1)%(_frameSkip+1);

     // calc framerate
     /*_timeStart = _timeEnd;
     _timeEnd = now_ms();
     _timeDelta = (_timeEnd - _timeStart);
     FCEUI_printf("FPS: %g", _timeDelta);
     */

     // check for viewport change from last frame
     int update = bitmap.viewport.changed & 1;
     if (update)
     {
          int vwidth = bitmap.viewport.w + (2 * bitmap.viewport.x);
          int vheight = bitmap.viewport.h + (2 * bitmap.viewport.y);

          // interlaced mode
          if (config.render && interlaced)
          {
               vheight = vheight << 1;
          }

          LOGD("Bitmap Changed: %d, %d, %d, pitch: %d", bitmap.viewport.changed, vwidth, vheight, bitmap.pitch);
          Graphics.ReshapeEmuTexture(vwidth, vheight, SCREEN_RENDER_TEXTURE_WIDTH);

          _viewPortW = vwidth;
          _viewPortH = vheight;

          bitmap.viewport.changed &= ~1;

          bitmap.pitch = bitmap.viewport.w * bitmap.granularity;
     }

     // step emulation
     system_frame(_frameSkipCount);

     // draw the texture/fbo
     Graphics.DrawEMU(bitmap.data, _viewPortW, _viewPortH);

     // retrieve audio
     _ssize = audio_update() << 1;
     //if (!_audioInitialized) _ssize = 0;

     //LOGD("AudioSamples: %d", _ssize);

    //LOGD("DONE STEP");
}


void Application::draw(const JNIEnv* env)
{
    Graphics.Clear();

    Graphics.Draw();

    Input.draw(Graphics);
}
