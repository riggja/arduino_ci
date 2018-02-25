#pragma once
#include "Print.h"
#include "Stream.h"
#include "Godmode.h"

// definitions neeeded for Serial.begin's config arg

class SoftwareSerial : public Stream
{
  private:
    int mPinIn;
    int mPinOut;
    bool mInverse;
    bool mIsListening;
    GodmodeState* mState;
    unsigned long mOffset; // bits to offset stream

  public:
    SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false) {
      mPinIn = receivePin;
      mPinOut = transmitPin;
      mIsListening = false;
      mOffset = 0; // godmode starts with 1 bit in the queue
      mState = GODMODE();
    }

    ~SoftwareSerial() {};

    void setGodmodeOffset(unsigned long offset) {
      mOffset = offset;
    }

    bool listen() { return mIsListening = true; }
    bool isListening() { return mIsListening; }
    bool stopListening() {
      bool ret = mIsListening;
      mIsListening = false;
      return ret;
    }
    void begin(long speed) { listen(); }
    void end() { stopListening(); }
    bool overflow() { return false; }

    int peek() {
      if (!isListening()) return -1;
      String input = mState->digitalPin[mPinIn].incomingToAscii(mOffset, true);
      if (input.empty()) return -1;
      return input[0];
    }

    virtual int read() {
      if (!isListening()) return -1;
      String input = mState->digitalPin[mPinIn].incomingToAscii(mOffset, true);
      if (input.empty()) return -1;
      int ret = input[0];
      for (int i = 0; i < 8; ++i) digitalRead(mPinIn);
      return ret;
    }

    //using Print::write;

    virtual size_t write(uint8_t byte) {
      mState->digitalPin[mPinOut].outgoingFromAscii(String((char)byte), true);
      return 1;
    }

    virtual int available() { return mState->digitalPin[mPinIn].incomingToAscii(mOffset, true).length();  }
    virtual void flush() {}
    operator bool() { return true; }

    static inline void handle_interrupt() {};

};


