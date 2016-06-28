#include "callback.h"
#include "helpers.h"

JsPaStreamCallbackBridge::JsPaStreamCallbackBridge(Callback *callback_,
                                                   size_t bytesPerFrameIn,
                                                   size_t bytesPerFrameOut,
                                                   const LocalValue &userData)
  : AsyncWorker(callback_), m_frameCount(0), m_callbackResult(0),
    m_bytesPerFrameIn(bytesPerFrameIn), m_bytesPerFrameOut(bytesPerFrameOut),
    m_inputBuffer(nullptr), m_outputBuffer(nullptr) {
  async = new uv_async_t;
  uv_async_init(
    uv_default_loop()
    , async
    , UVCallback
  );
  async->data = this;
  uv_mutex_init(&async_lock);
      
  // Save userData to persistent object
  SaveToPersistent(ToLocString("userData"), userData);
}

JsPaStreamCallbackBridge::~JsPaStreamCallbackBridge() {
  uv_mutex_destroy(&async_lock);
  uv_close((uv_handle_t*)async, NULL);
  
  //free buffer memory
  if(m_inputBuffer != nullptr)
      free(m_inputBuffer);
  if(m_outputBuffer != nullptr)
      free(m_outputBuffer);
}

int JsPaStreamCallbackBridge::sendToCallback(const void* input, unsigned long frameCount) {
  uv_mutex_lock(&async_lock);
    m_frameCount = frameCount;
  
    if(m_inputBuffer != nullptr)
      free(m_inputBuffer);
    m_inputBuffer = malloc(sizeof(float) * frameCount * 2);
  
    memmove(
      m_inputBuffer,
      input,
      m_bytesPerFrameIn * frameCount
    );
  uv_mutex_unlock(&async_lock);
  
  uv_async_send(async);
  return 0;
}

void JsPaStreamCallbackBridge::dispatchJSCallback() {
  HandleScope scope;
  unsigned long frameCount;
  v8::Local<v8::ArrayBuffer> input;
  v8::Local<v8::ArrayBuffer> output;
  v8::Local<v8::Value> callbackReturn;
  
  uv_mutex_lock(&async_lock);
  
    frameCount = m_frameCount;
    
    // Setup ArrayBuffer for input audio data
    input = v8::ArrayBuffer::New(
      v8::Isolate::GetCurrent(),
      m_bytesPerFrameIn * frameCount
    );
    // Copy input audio data from bridge buffer to ArrayBuffer
    memmove(
      input->GetContents().Data(),
      m_inputBuffer,
      input->ByteLength()
    );
  
    // Setup ArrayBuffer for output audio data
    output = v8::ArrayBuffer::New(
      v8::Isolate::GetCurrent(),
      m_bytesPerFrameOut * frameCount
    );
    
    // Create array of arguments and call the javascript callback
    LocalValue argv[] = {
      input,
      output,
      New<Number>(frameCount),
      GetFromPersistent(ToLocString("userData"))
    };
    callbackReturn = callback->Call(4, argv);
    
    if(m_outputBuffer != nullptr)
      free(m_outputBuffer);
    m_outputBuffer = malloc(output->ByteLength());
    // Copy output audio data from bridge buffer to ArrayBuffer
    memmove(
      m_outputBuffer,
      output->GetContents().Data(),
      output->ByteLength()
    );
    
    // Store the return result of the javascript callback 
    // so it be sent to the PaStreamCallback function
    m_callbackResult = LocalizeInt(callbackReturn);
  
  uv_mutex_unlock(&async_lock);
  
}
  
void JsPaStreamCallbackBridge::consumeAudioData(void* output, unsigned long frameCount) {
  
  if(m_outputBuffer != nullptr) {
    memmove(
      output,
      m_outputBuffer,
      m_bytesPerFrameOut * frameCount
    );
    
    // Free the output buffer and set it to nullptr to prevent it from sending the same output data twice
    free(m_outputBuffer);
    m_outputBuffer = nullptr;
  }
}

int JsPaStreamCallbackBridge::getCallbackResult() {
  int ret;
  uv_mutex_lock(&async_lock);
    ret = m_callbackResult;
  uv_mutex_unlock(&async_lock);
  return ret;
}