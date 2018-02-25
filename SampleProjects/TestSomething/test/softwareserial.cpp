#include <ArduinoUnitTests.h>
#include <SoftwareSerial.h>

unittest(software_input_output)
{
  GodmodeState* state = GODMODE();
  state->reset();

  SoftwareSerial ss(1, 2, false);

  assertEqual(-1, ss.peek());

  state->digitalPin[1].fromAscii("Holy crap ", true);
  state->digitalPin[1].fromAscii("this took a lot of prep work", true);

  assertFalse(ss.isListening());
  assertEqual(-1, ss.peek());

  ss.listen();
  assertTrue(ss.isListening());
  assertEqual(38, ss.available());
  assertEqual('H', ss.peek());
  assertEqual('H', ss.read());
  assertEqual('o', ss.read());
  assertEqual('l', ss.read());
  assertEqual('y', ss.read());

  ss.write('b');
  ss.write('A');
  ss.write('r');
  assertEqual("bAr", state->digitalPin[2].toAscii(1, true));
}

unittest(print) {
  GodmodeState* state = GODMODE();
  state->reset();

  SoftwareSerial ss(1, 2, false);
  ss.listen();
  ss.print(1.3, 2);
  assertEqual("1.30", state->digitalPin[2].toAscii(1, true));
}

unittest_main()
