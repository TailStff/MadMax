#ifndef INTERVALCALLBACK
#define INTERVALCALLBACK

class IntervalCallback {

private:
  uint64_t previousMillis;
  uint32_t _interval;
  bool active;
  bool _automaticRestart;

  // Pointer to access to callback function
  void (*voidCallback)(void);

public:
  // Constructors
  IntervalCallback(void (*callbackFunction)(void));

  void Tick();

  void Start(long interval, bool automaticRestart);
  void Stop();

  uint32_t LastExecutionInterval;
};

#endif