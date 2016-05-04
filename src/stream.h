#ifndef STREAM_H
#define STREAM_H

#include "jsaudio.h"

class JsPaStream : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

    inline PaStream* streamPointer() const { return _stream; }
private:
    explicit JsPaStream();
    ~JsPaStream();

    static NAN_METHOD(test);
    
    static NAN_METHOD(New);
    static Persistent<Function> constructor;
    PaStream* _stream;
};

#endif