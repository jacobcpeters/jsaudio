#ifndef CALLBACK_H
#define CALLBACK_H

#include "jsaudio.h"

class JsPaStreamCallback : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

    int sendCallback(const void* input,
                     void* output,
                     unsigned long frameCount,
                     const PaStreamCallbackTimeInfo *timeInfo,
                     PaStreamCallbackFlags statusFlags);
private:
    explicit JsPaStreamCallback(LocalFunction callbackHandle);
    ~JsPaStreamCallback();

    static NAN_METHOD(New);
    static Persistent<Function> constructor;
    LocalFunction _callbackHandle;
};

#endif