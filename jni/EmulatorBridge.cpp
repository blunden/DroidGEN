#include <math.h>
#include <jni.h>
#include "EmulatorBridge.h"

#include <android/log.h>

#include "common.h"
#include "GraphicsDriver.h"
#include "Application.h"

Application Emulator;

extern "C" {

/*jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
        JNIEnv  *env;
        jclass  cls;
        jmethodID constr;
        jobject  s_savedAudioClass;
        jmethodID s_savedAudioPlayMethod;
        jmethodID s_savedAudioGetMinBufferMethod;

        // verify correct JNI
        if ((*vm).GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
        {
                return -1;
        }

        return JNI_VERSION_1_6;
}*/


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_destroy
  (JNIEnv *, jclass)
{
     Emulator.destroy();
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_setPaths
  (JNIEnv *env, jclass, jstring extStorageDir, jstring romDir, jstring stateDir, jstring sramDir, jstring cheatsDir)
{
	jboolean isCopy = false;
    const char * szExternalStorage = env->GetStringUTFChars(extStorageDir, &isCopy);
    const char * szRomDir = env->GetStringUTFChars(romDir, &isCopy);
    const char * szStateDir = env->GetStringUTFChars(stateDir, &isCopy);
    const char * szSramDir = env->GetStringUTFChars(sramDir, &isCopy);
    const char * szCheatsDir = env->GetStringUTFChars(cheatsDir, &isCopy);

    int retVal = Emulator.setPaths(szExternalStorage, szRomDir, szStateDir, szSramDir, szCheatsDir);

    env->ReleaseStringUTFChars(extStorageDir, szExternalStorage);
    env->ReleaseStringUTFChars(romDir, szRomDir);
    env->ReleaseStringUTFChars(stateDir, szStateDir);
    env->ReleaseStringUTFChars(sramDir, szSramDir);
    env->ReleaseStringUTFChars(cheatsDir, szCheatsDir);
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_init
  	  	  (JNIEnv* env, jclass cls,
		  jstring apkAbsolutePath)
{
     LOGD("Emulator_init()");

     jboolean isCopy;
     const char * szFilename = env->GetStringUTFChars(apkAbsolutePath, &isCopy);

     int retVal = Emulator.init(env, szFilename);

     env->ReleaseStringUTFChars(apkAbsolutePath, szFilename);


	return retVal;
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_initGraphics
  (JNIEnv* env, jclass cls)
{
     int result = Emulator.initGraphics();
     Emulator.Graphics.InitEmuShader(NULL, NULL);
     return result;
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_loadRom
  (JNIEnv *env, jclass obj, jstring fileName)
{
     LOGD("Emulator_loadROM()");

    jboolean isCopy;
    const char * szFilename = env->GetStringUTFChars(fileName, &isCopy);

    int retVal = Emulator.loadROM(szFilename);

    env->ReleaseStringUTFChars(fileName, szFilename);

    return retVal;
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_initAudioBuffer
  (JNIEnv *, jclass, jint sizeInSamples)
{
     return Emulator.initAudioBuffers(sizeInSamples);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setAudioSampleRate
  (JNIEnv *, jclass, jint rate)
{
     Emulator.setAudioSampleRate(rate);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_saveState
  (JNIEnv *, jclass, jint i)
{
     Emulator.saveState(i);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_loadState
  (JNIEnv *, jclass, jint i)
{
     Emulator.loadState(i);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_selectState
  (JNIEnv *, jclass, jint i)
{
     Emulator.selectState(i);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setEnableRewind
  (JNIEnv *, jclass, jboolean b)
{
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setFrameSkip
  (JNIEnv *, jclass, jint i)
{
     Emulator.setFrameSkip(i);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setGameGenie
  (JNIEnv *, jclass, jboolean b)
{
	// TODO: enable genie
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_resetGame
  (JNIEnv *, jclass)
{
	Emulator.resetROM();
}


JNIEXPORT jboolean JNICALL Java_com_droidhits_genesisdroid_Emulator_isRomLoaded
  (JNIEnv *, jclass)
{
     return Emulator.isRomLoaded();
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_unzipFile
  (JNIEnv* env, jclass, jstring zipFileName, jstring extractFileName, jstring outLocation)
{
     jboolean isCopy;
     const char * szZipFile = env->GetStringUTFChars(zipFileName, &isCopy);
     const char * szExtractFile = env->GetStringUTFChars(extractFileName, &isCopy);
     const char * szOutLocation = env->GetStringUTFChars(outLocation, &isCopy);

     LOGD("unzipFile(%s, %s, %s)", szZipFile, szExtractFile, szOutLocation);

     char tmp[8092];
     size_t read = 0;

     FILE* outFile = fopen(szOutLocation, "w");
     zip* zipArchive = zip_open(szZipFile, 0, NULL);
     struct zip_file* file = zip_fopen(zipArchive, szExtractFile, 0);
     read = zip_fread(file, tmp, sizeof(tmp));
     while (read > 0)
     {
          fwrite(tmp, sizeof(char), read, outFile);
          read = zip_fread(file, tmp, sizeof(tmp));
     }

     zip_fclose(file);
     fclose(outFile);
     zip_close(zipArchive);

     env->ReleaseStringUTFChars(zipFileName, szZipFile);
     env->ReleaseStringUTFChars(extractFileName, szExtractFile);
     env->ReleaseStringUTFChars(outLocation, szOutLocation);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_onTouchUp
  (JNIEnv* env, jclass cls, jint finger, jfloat x, jfloat y, jfloat radius)
{
     TouchEvent event;
     event.finger = finger;
     event.x = x;
     event.y = y;

     Emulator.Input.onTouchUp(event);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_onTouchDown
  (JNIEnv* env, jclass cls, jint finger, jfloat x, jfloat y, jfloat radius)
{
     TouchEvent event;
     event.finger = finger;
     event.x = x;
     event.y = y;
     event.radius = radius;

     Emulator.Input.onTouchDown(event);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_onTouchMove
  (JNIEnv* env, jclass cls, jint finger, jfloat x, jfloat y, jfloat radius)
{
     TouchEvent event;
     event.finger = finger;
     event.x = x;
     event.y = y;
     event.radius = radius;

     Emulator.Input.onTouchMove(event);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_onKeyDown
 (JNIEnv *, jclass, jint keycode)
{
     Emulator.Input.onKeyDown(keycode);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_onKeyUp
 (JNIEnv *, jclass, jint keycode)
{
     Emulator.Input.onKeyUp(keycode);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setAnalog
  (JNIEnv *, jclass, jfloat x, jfloat y, jfloat width, jfloat height,
            jint leftcode, jint upcode, jint rightcode, jint downcode, jboolean visible)
{
     LOGD("Emulator_setAnalog(%d, %f, %f, %f, %f, %d, %d, %d, %d, %d)", 0, x, y, width, height, leftcode, upcode, rightcode, downcode, visible);
     Emulator.Input.setAnalog(0, x, y, width, height, leftcode, upcode, rightcode, downcode, visible);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setButton
  (JNIEnv *, jclass, jint buttonIndex, jfloat x, jfloat y, jfloat width, jfloat height, jint keycode, jboolean visible)
{
     LOGD("Emulator_setButton(%d, %f, %f, %f, %f, %d, %d)", buttonIndex, x, y, width, height, keycode, visible);
     Emulator.Input.setButton(buttonIndex, x, y, width, height, keycode, visible);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setSmoothFiltering
  (JNIEnv *, jclass, jboolean b)
{
     Emulator.Graphics.SetSmooth(b);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setAspectRatio
  (JNIEnv *, jclass, jfloat ratio)
{
     Emulator.Graphics.SetAspectRatio(ratio);
     Emulator.setGenPlusVideoChanged();
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setViewport
  (JNIEnv *, jclass, jint width, jint height)
{
     Emulator.Graphics.SetDimensions(width, height);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setAudioEnabled
  (JNIEnv *, jclass, jboolean b)
{
     Emulator.setAudioEnabled(b);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_setSensitivity
  (JNIEnv *, jclass, jfloat xAxis, jfloat yAxis)
{
     LOGD("Emulator_setSensitivity(%f, %f)", xAxis, yAxis);

     Emulator.Input.setXSensitivity(xAxis);
     Emulator.Input.setYSensitivity(yAxis);
}


JNIEXPORT jint JNICALL Java_com_droidhits_genesisdroid_Emulator_mixAudioBuffer
  (JNIEnv* env, jclass cls, jshortArray audioBuf)
{
    //jboolean isCopy = false;
    /*short* buf = (short*)env->GetPrimitiveArrayCritical(audioBuf, NULL);
    if (buf == NULL)
    {
         LOGE("ERROR GETTING AUDIO BUFFER REFERENCE!");
         return -1;
    }*/
    //LOGD("isCopy: %d", isCopy);

    // get samples from emulator
    //size_t readAmount = Emulator.readAudio(buf);

    //LOGD("mixAudioBuffer(read: %d)", readAmount);
    //LOGD("FINISHED AUDIO MIX, READ: %d, REMAINING: %d", readAmount, Emulator._AudioFiFo->write_avail());
    size_t readAmount = Emulator.getCurrentEmuSamples();

    // hack for now
    if (readAmount >= Emulator.getAudioBufferSize()) return 0;

    //LOGD("Samples: %d", readAmount);
    env->SetShortArrayRegion(audioBuf, 0, readAmount, (jshort*)Emulator.getAudioBuffer());
    //env->ReleasePrimitiveArrayCritical(audioBuf, buf, 0);

    return readAmount;
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_step
  (JNIEnv* env, jclass obj)
{
	Emulator.step(env);
}


JNIEXPORT void JNICALL Java_com_droidhits_genesisdroid_Emulator_draw
  (JNIEnv* env, jclass obj)
{
     Emulator.draw(env);
}

}
