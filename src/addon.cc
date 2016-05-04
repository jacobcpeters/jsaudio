#include "jsaudio.h"
#include "stream.h"
#include "callback.h"

/* BEGIN Init & Exports */
NAN_MODULE_INIT(InitAll) {
    NAN_EXPORT(target, initialize);
    NAN_EXPORT(target, terminate);
    NAN_EXPORT(target, getVersion);
    NAN_EXPORT(target, getHostApiCount);
    NAN_EXPORT(target, getDefaultHostApi);
    NAN_EXPORT(target, getHostApiInfo);
    NAN_EXPORT(target, getDeviceCount);
    NAN_EXPORT(target, getDefaultInputDevice);
    NAN_EXPORT(target, getDefaultOutputDevice);
    NAN_EXPORT(target, getDeviceInfo);
    NAN_EXPORT(target, openStream);
    // NAN_EXPORT(target, openDefaultStream);
    
    JsPaStream::Init(target);
    JsPaStreamCallback::Init(target);
}

NODE_MODULE(jsaudio, InitAll)