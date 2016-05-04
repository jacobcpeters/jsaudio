#include "callback.h"

Persistent<Function> JsPaStreamCallback::constructor;

JsPaStreamCallback::JsPaStreamCallback(LocalFunction callbackHandle) : _callbackHandle(callbackHandle) {
}

JsPaStreamCallback::~JsPaStreamCallback() {
}

int JsPaStreamCallback::sendCallback(const void* input, void* output,
                 unsigned long frameCount,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags
) {
     
    MakeCallback(Nan::GetCurrentContext()->Global(), _callbackHandle, 0, 0);
    return 0;
}

NAN_MODULE_INIT(JsPaStreamCallback::Init) {
  // Prepare constructor template
  LocalFunctionTemplate tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("JsPaStreamCallback").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("JsPaStreamCallback").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(JsPaStreamCallback::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new JsPaStreamCallback(Function)`
    LocalFunction  callback = info[0]->IsUndefined() ? Nan::Undefined().As<Function>() : info[0].As<Function>();
    JsPaStreamCallback* obj = new JsPaStreamCallback(callback);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `JsPaStreamCallback(...)`, turn into construct call.
    const int argc = 1;
    LocalValue argv[argc] = { info[0] };
    LocalFunction cons =  Nan::New<Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}