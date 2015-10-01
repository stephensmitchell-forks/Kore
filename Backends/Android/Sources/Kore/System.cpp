#include "pch.h"
/*#include <Kore/System.h>
#include <Kore/Application.h>
#include <Kore/Audio/Audio.h>
#include <Kore/IO/FileReader.h>
#include <Kore/IO/miniz.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Input/Sensor.h>
#include <Kore/Input/Surface.h>
#include <Kore/Log.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include <cstring>
#if SYS_ANDROID_API >= 15
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

// TODO: FM: Inconsistent namespaces
#include <Kore/Vr/VrInterface.h>

void* Kore::System::createWindow() {
	return nullptr;
}

void Kore::System::swapBuffers() {

}

void Kore::System::destroyWindow() {

}

void Kore::System::changeResolution(int, int, bool) {

}

void Kore::System::setTitle(const char*) {

}

void Kore::System::showWindow() {

}

namespace {
	char theApkPath[1001];
	char filesDir[1001];
	int width;
	int height;
	bool keyboardShown = false;

	//Android GDB does not attach immediately after a native lib is loaded.
	//To debug startup behavior set the debuggingDelay to about 200.
	bool initialized = false;
	int debuggingDelayCount = 0;
	const int debuggingDelay = 0;
#if SYS_ANDROID_API >= 15
	AAssetManager* assets;
#endif
	JNIEnv* env;
	bool firstinit = true;
}

JNIEnv* getEnv() {
	return env;
}

#if SYS_ANDROID_API >= 15
AAssetManager* getAssetManager() {
	return assets;
}
#endif

void Kore::System::showKeyboard() {
	keyboardShown = true;
}

void Kore::System::hideKeyboard() {
	keyboardShown = false;
}

void Kore::System::loadURL(const char* url) {
    
}

int Kore::System::screenWidth() {
    return width;
}

int Kore::System::screenHeight() {
    return height;
}

const char* Kore::System::savePath() {
	return filesDir;
}

const char* Kore::System::systemId() {
	return "Android";
}

extern int kore(int argc, char** argv);

extern "C" {
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_init(JNIEnv* env, jobject obj, jint width, jint height, jobject assetManager, jstring apkPath, jstring filesDir);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_step(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_touch(JNIEnv* env, jobject obj, jint index, jint x, jint y, jint action);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_writeAudio(JNIEnv* env, jobject obj, jbyteArray buffer, jint size);
	JNIEXPORT bool JNICALL Java_com_ktxsoftware_kore_KoreLib_keyboardShown(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_keyUp(JNIEnv* env, jobject obj, jint code);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_keyDown(JNIEnv* env, jobject obj, jint code);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_accelerometerChanged(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_gyroChanged(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onCreate(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onStart(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onPause(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onResume(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onStop(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onRestart(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onDestroy(JNIEnv* env, jobject obj);
    JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_gaze(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z, jfloat w);
};

void initAndroidFileReader(AAssetManager* assets);

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_init(JNIEnv* env, jobject obj, jint width, jint height, jobject assetManager, jstring apkPath, jstring filesDir) {
	if (firstinit) {
		{
			const char* path = env->GetStringUTFChars(filesDir, nullptr);
			std::strcpy(::filesDir, path);
			std::strcat(::filesDir, "/");
			env->ReleaseStringUTFChars(filesDir, path);
		}

    	//(*env)->NewGlobalRef(env, foo);
    	assets = AAssetManager_fromJava(env, assetManager);
		initAndroidFileReader(assets);

		firstinit = false;
	}

	glViewport(0, 0, width, height);
	::width = width;
	::height = height;

	// Save the JVM for the Vr system
	JavaVM* cachedJVM;
	env->GetJavaVM(&cachedJVM);
	Kore::VrInterface::SetJVM(cachedJVM);
}

namespace {
	void init() {
		kore(0, nullptr);
		initialized = true;
	}
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_step(JNIEnv* env, jobject obj) {
    ::env = env;
	if (!initialized) {
		++debuggingDelayCount;
		if (debuggingDelayCount > debuggingDelay) {
			init();
			Kore::Application::the()->callback();
		}
	}
	else {
		Kore::Application::the()->callback();
	}
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_touch(JNIEnv* env, jobject obj, jint index, jint x, jint y, jint action) {
    ::env = env;
	switch (action) {
	case 0: //DOWN
		if (index == 0) {
			Kore::Mouse::the()->_press(0, x, y);
		}
		Kore::Surface::the()->_touchStart(index, x, y);
		break;
	case 1: //MOVE
		if (index == 0) {
			Kore::Mouse::the()->_move(x, y);
		}
		Kore::Surface::the()->_move(index, x, y);
		break;
	case 2: //UP
		if (index == 0) {
			Kore::Mouse::the()->_release(0, x, y);
		}
		Kore::Surface::the()->_touchEnd(index, x, y);
		break;
	}
}

JNIEXPORT bool JNICALL Java_com_ktxsoftware_kore_KoreLib_keyboardShown(JNIEnv* env, jobject obj) {
	::env = env;
	return keyboardShown;
}

namespace {
	bool shift = false;
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_keyUp(JNIEnv* env, jobject obj, jint code) {
	::env = env;
	switch (code) {
	case 0x00000120:
		shift = false;
		Kore::Keyboard::the()->_keyup(Kore::Key_Shift, 0);
		break;
	case 0x00000103:
		Kore::Keyboard::the()->_keyup(Kore::Key_Backspace, 0);
		break;
	case 0x00000104:
		Kore::Keyboard::the()->_keyup(Kore::Key_Return, 0);
		break;
	default:
		if (shift)
			Kore::Keyboard::the()->_keyup((Kore::KeyCode)code, code);
		else
			Kore::Keyboard::the()->_keyup((Kore::KeyCode)(code + 'a' - 'A'), code + 'a' - 'A');
		break;
	}
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_keyDown(JNIEnv* env, jobject obj, jint code) {
	::env = env;
	switch (code) {
	case 0x00000120:
		shift = true;
		Kore::Keyboard::the()->_keydown(Kore::Key_Shift, 0);
		break;
	case 0x00000103:
		Kore::Keyboard::the()->_keydown(Kore::Key_Backspace, 0);
		break;
	case 0x00000104:
		Kore::Keyboard::the()->_keydown(Kore::Key_Return, 0);
		break;
	default:
		if (shift)
			Kore::Keyboard::the()->_keydown((Kore::KeyCode)code, code);
		else
			Kore::Keyboard::the()->_keydown((Kore::KeyCode)(code + 'a' - 'A'), code + 'a' - 'A');
		break;
	}
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_accelerometerChanged(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z) {
    ::env = env;
    Kore::Sensor::_changed(Kore::SensorAccelerometer, x, y, z);
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_gyroChanged(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z) {
    ::env = env;
    Kore::Sensor::_changed(Kore::SensorGyroscope, x, y, z);
}

#ifdef VR_CARDBOARD

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_gaze(JNIEnv* env, jobject obj, jfloat x, jfloat y, jfloat z, jfloat w) {
    Kore::VrInterface::updateGaze(x, y, z, w);
}

#endif

namespace {
	using namespace Kore;

	void copySample(void* buffer) {
		float value = *(float*)&Audio::buffer.data[Audio::buffer.readLocation];
		Audio::buffer.readLocation += 4;
		if (Audio::buffer.readLocation >= Audio::buffer.dataSize) Audio::buffer.readLocation = 0;
		*(s16*)buffer = static_cast<s16>(value * 32767);
	}
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_writeAudio(JNIEnv* env, jobject obj, jbyteArray buffer, jint size) {
    //::env = env;
	if (Kore::Audio::audioCallback != nullptr) {
		Kore::Audio::audioCallback(size / 2);
		jbyte* arr = env->GetByteArrayElements(buffer, 0);
		for (int i = 0; i < size; i += 2) {
			copySample(&arr[i]);
		}
		env->ReleaseByteArrayElements(buffer, arr, 0);
	}
}

bool Kore::System::handleMessages() {
	return true;
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onCreate(JNIEnv* env, jobject obj) {

}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onStart(JNIEnv* env, jobject obj) {
	if (Kore::Application::the() != nullptr && Kore::Application::the()->foregroundCallback != nullptr) Kore::Application::the()->foregroundCallback();
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onPause(JNIEnv* env, jobject obj) {
	if (Kore::Application::the() != nullptr && Kore::Application::the()->pauseCallback != nullptr) Kore::Application::the()->pauseCallback();
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onResume(JNIEnv* env, jobject obj) {
	if (Kore::Application::the() != nullptr && Kore::Application::the()->resumeCallback != nullptr) Kore::Application::the()->resumeCallback();
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onStop(JNIEnv* env, jobject obj) {
	if (Kore::Application::the() != nullptr && Kore::Application::the()->backgroundCallback != nullptr) Kore::Application::the()->backgroundCallback();
}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onRestart(JNIEnv* env, jobject obj) {

}

JNIEXPORT void JNICALL Java_com_ktxsoftware_kore_KoreLib_onDestroy(JNIEnv* env, jobject obj) {
	if (Kore::Application::the() != nullptr && Kore::Application::the()->shutdownCallback != nullptr) Kore::Application::the()->shutdownCallback();
}

#include <sys/time.h>
#include <time.h>

double Kore::System::frequency() {
	return 1000000.0;
}

Kore::System::ticks Kore::System::timestamp() {
	timeval now;
	gettimeofday(&now, NULL);
	return static_cast<ticks>(now.tv_sec) * 1000000 + static_cast<ticks>(now.tv_usec);
}

double Kore::System::time() {
    timeval now;
    gettimeofday(&now, NULL);
    return (double)now.tv_sec + (double)(now.tv_usec / 1000000.0);
}
*/

#include <Kore/System.h>
#include <Kore/Log.h>
#include <errno.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <GLContext.h>

extern int kore(int argc, char** argv);

namespace {
	android_app* app;
	AAssetManager* assets;
	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	ndk_helper::GLContext* glContext;
	
	bool started = false;
	bool paused = true;

	void initDisplay() {
		if (glContext->Resume(app->window) != EGL_SUCCESS) {
			Kore::log(Kore::Warning, "GL context lost.");
		}
	}

	void termDisplay() {
		glContext->Suspend();
	}

	int32_t input(android_app* app, AInputEvent* event) {
		if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
			//engine->state.x = AMotionEvent_getX(event, 0);
			//engine->state.y = AMotionEvent_getY(event, 0);
			return 1;
		}
		return 0;
	}

	void cmd(android_app* app, int32_t cmd) {
		switch (cmd) {
			case APP_CMD_SAVE_STATE:
				break;
			case APP_CMD_INIT_WINDOW:
				if (app->window != NULL) {
					initDisplay();
					if (!started) {
						started = true;
					}
					Kore::System::swapBuffers();
				}
				break;
			case APP_CMD_TERM_WINDOW:
				termDisplay();
				break;
			case APP_CMD_GAINED_FOCUS:
				if (accelerometerSensor != NULL) {
					ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
					ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L / 60) * 1000);
				}
				paused = false;
				break;
			case APP_CMD_LOST_FOCUS:
				if (accelerometerSensor != NULL) {
					ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
				}
				paused = true;
				Kore::System::swapBuffers();
				break;
		}
	}
}

AAssetManager* getAssetManager() {
	return assets;
}

void* Kore::System::createWindow() {
	return nullptr;
}

void Kore::System::swapBuffers() {
	if (glContext->Swap() != EGL_SUCCESS) {
		Kore::log(Kore::Warning, "GL context lost.");
	}
}

void Kore::System::destroyWindow() {

}

void Kore::System::showKeyboard() {
	//keyboardShown = true;
}

void Kore::System::hideKeyboard() {
	//keyboardShown = false;
}

void Kore::System::loadURL(const char* url) {

}

int Kore::System::screenWidth() {
	return glContext->GetScreenWidth();
}

int Kore::System::screenHeight() {
	return glContext->GetScreenHeight();
}

const char* Kore::System::savePath() {
	return ""; //filesDir;
}

const char* Kore::System::systemId() {
	return "Android";
}

void Kore::System::changeResolution(int, int, bool) {

}

void Kore::System::setTitle(const char*) {

}

void Kore::System::showWindow() {

}

#include <sys/time.h>
#include <time.h>

double Kore::System::frequency() {
	return 1000000.0;
}

Kore::System::ticks Kore::System::timestamp() {
	timeval now;
	gettimeofday(&now, NULL);
	return static_cast<ticks>(now.tv_sec) * 1000000 + static_cast<ticks>(now.tv_usec);
}

double Kore::System::time() {
	timeval now;
	gettimeofday(&now, NULL);
	return (double)now.tv_sec + (now.tv_usec / 1000000.0);
}

bool Kore::System::handleMessages() {
	int ident;
	int events;
	android_poll_source* source;

	while ((ident = ALooper_pollAll(paused ? -1 : 0, NULL, &events, (void**)&source)) >= 0) {
		if (source != NULL) {
			source->process(app, source);
		}

		if (ident == LOOPER_ID_USER) {
			if (accelerometerSensor != NULL) {
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
					//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
				}
			}
		}

		if (app->destroyRequested != 0) {
			termDisplay();
			return true;
		}
	}
	return true;
}

void initAndroidFileReader(AAssetManager* assets);

extern "C" void android_main(android_app* app) {
	app_dummy();

	::app = app;
	assets = app->activity->assetManager;
	initAndroidFileReader(assets);
	app->onAppCmd = cmd;
	app->onInputEvent = input;

	glContext = ndk_helper::GLContext::GetInstance();
	sensorManager = ASensorManager_getInstance();
	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	sensorEventQueue = ASensorManager_createEventQueue(sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);

	while (!started) {
		Kore::System::handleMessages();
	}
	kore(0, nullptr);
}
