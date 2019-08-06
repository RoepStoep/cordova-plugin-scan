#include <jni.h>
#include <string>
#include <cmath>
#include <threadbuf.h>
#include "bb_comp.hpp"
#include "bb_index.hpp"
#include "bit.hpp"
#include "main.hpp"
#include "hash.hpp"
#include "pos.hpp"
#include "thread.hpp"
#include "var.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)

extern "C" {
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniInit(JNIEnv *env, jobject obj, jlong memorySize, jstring jvariant, jstring jcachePath, jobject jassetManager);
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniExit(JNIEnv *env, jobject obj);
  JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniCmd(JNIEnv *env, jobject obj, jstring jcmd);
};

static JavaVM *jvm;
static jobject jobj;
static jmethodID onMessage;

static std::string CMD_EXIT = "scan:exit";

void ensure_cached(AAssetManager *mgr, const std::string & filename);

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

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniInit(JNIEnv *env, jobject obj, jlong memorySize, jstring jvariant, jstring jcachePath, jobject jassetManager) {
  jobj = env->NewGlobalRef(obj);
  env->GetJavaVM(&jvm);
  jclass classScan = env->GetObjectClass(obj);
  onMessage = env->GetMethodID(classScan, "onMessage", "([B)V");

  reader = std::thread(readstdout);

  bit::init();
  hash::init();
  pos::init();
  var::init();

  bb::index_init();
  bb::comp_init();

  ml::rand_init(); // after hash keys

  // Allow using 16th of the total memory for the hashtable
  uint64_t hashSize = memorySize / 16;
  hashSize /= 1024 * 1024;
  hashSize = std::max((uint64_t)16, hashSize);
  std::string ttSize = std::to_string(floor(log2(hashSize * 1024 * 1024 / 16)));
  var::set("tt-size", ttSize);

  // store application cache directory as datapath
  const char* cachePath = env->GetStringUTFChars(jcachePath, (jboolean *)0);
  var::set("datapath", std::string(cachePath));
  env->ReleaseStringUTFChars(jcachePath, cachePath);

  // set variant (IMPORTANT: do not change after init because cached files can be cleaned by the system)
  const char* variant = env->GetStringUTFChars(jvariant, (jboolean *)0);
  var::set("variant", std::string(variant));
  env->ReleaseStringUTFChars(jvariant, variant);

  var::update();

  // make sure the required eval/book files exist in cache before init
  AAssetManager* mgr = AAssetManager_fromJava(env, jassetManager);
  ensure_cached(mgr, std::string("book") + var::variant_name());
  ensure_cached(mgr, std::string("eval") + var::variant_name());

  bit::init(); // depends on the variant

  // start input loop on a new thread
  listen_input();
}

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniExit(JNIEnv *env, jobject obj) {
  hub_command("quit");
  sync_cout << CMD_EXIT << sync_endl;
  reader.join();
}

JNIEXPORT void JNICALL Java_org_lidraughts_scan_CordovaPluginScan_jniCmd(JNIEnv *env, jobject obj, jstring jcmd) {
  const char *cmd = env->GetStringUTFChars(jcmd, (jboolean *)0);
  hub_command(cmd);
  env->ReleaseStringUTFChars(jcmd, cmd);
}

void ensure_cached(AAssetManager *mgr, const std::string & filename) {
  const char *filepath = var::data_file(filename).c_str();
  if (FILE *file = fopen(filepath, "r")) {
    fclose(file);
  } else {
    AAsset* asset = AAssetManager_open(mgr, filename.c_str(), AASSET_MODE_STREAMING);
    char buf[BUFSIZ];
    int nb_read = 0;

    FILE* out = fopen(filepath, "w");
    while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
      fwrite(buf, nb_read, 1, out);
    fclose(out);

    AAsset_close(asset);
  }
}
