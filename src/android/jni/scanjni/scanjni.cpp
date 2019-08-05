#include <jni.h>
#include <string>
#include <threadbuf.h>
#include <search.cpp>
#include <android/log.h>

#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)

extern "C" {
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniInit(JNIEnv *env, jobject obj, jlong memorySize);
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniExit(JNIEnv *env, jobject obj);
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniCmd(JNIEnv *env, jobject obj, jstring jcmd);
};

static JavaVM *jvm;
static jobject jobj;
static jmethodID onMessage;

static std::string CMD_EXIT = "scan:exit";

namespace PSQT {
  void init();
}

auto readstdout = []() {
  JNIEnv *jenv;

  jvm->GetEnv((void **)&jenv, JNI_VERSION_1_6);
  jvm->AttachCurrentThread(&jenv, (void*) NULL);

  // Save standard output
  std::streambuf* out = std::cout.rdbuf();

  threadbuf lichbuf(8, 1024);
  std::ostream lichout(&lichbuf);
  std::cout.rdbuf(lichout.rdbuf());
  std::istream lichin(&lichbuf);

  std::string o = "";

  while(o != CMD_EXIT) {
    std::string line;
    std::getline(lichin, line);
    if(line != CMD_EXIT) {
      const char* coutput = line.c_str();
      int len = strlen(coutput);
      jbyteArray aoutput = jenv->NewByteArray(len);
      jenv->SetByteArrayRegion (aoutput, 0, len, (jbyte*)coutput);
      jenv->CallVoidMethod(jobj, onMessage, aoutput);
      jenv->DeleteLocalRef(aoutput);
    } else {
      o = CMD_EXIT;
    }
  };

  // Restore output standard
  std::cout.rdbuf(out);

  lichbuf.close();
  jvm->DetachCurrentThread();
};

std::thread reader;

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniInit(JNIEnv *env, jobject obj, jlong memorySize) {
  jobj = env->NewGlobalRef(obj);
  env->GetJavaVM(&jvm);
  jclass classScan = env->GetObjectClass(obj);
  onMessage = env->GetMethodID(classScan, "onMessage", "([B)V");

  reader = std::thread(readstdout);

  // Allow using 16th of the total memory for the hashtable
  uint64_t hashSize = memorySize / 16;
  // Convert to megabytes
  hashSize /= 1024 * 1024;
  hashSize = std::max((uint64_t)16, hashSize);

  UCI::init(Options);
  Options["Hash"] = (int)hashSize;
  PSQT::init();
  Bitboards::init();
  Position::init();
  Bitbases::init();
  Search::init();
  Pawns::init();
  Tablebases::init(CHESS_VARIANT, Options["SyzygyPath"]); // After Bitboards are set
  Threads.set(Options["Threads"]);
  Search::clear(); // After threads are up
  TT.resize(Options["Hash"]);
}

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniExit(JNIEnv *env, jobject obj) {
  UCI::command("quit");
  sync_cout << CMD_EXIT << sync_endl;
  reader.join();
  Threads.set(0);
}

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniCmd(JNIEnv *env, jobject obj, jstring jcmd) {
  const char *cmd = env->GetStringUTFChars(jcmd, (jboolean *)0);
  UCI::command(cmd);
  env->ReleaseStringUTFChars(jcmd, cmd);
}
