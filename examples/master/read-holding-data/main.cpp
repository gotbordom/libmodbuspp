// Reads floating point holding registers from SolarPi pressure meter

// The pressure sensor calibration is composed of 4 decimal values.
// The first 2 are the minimum and maximum pressure calibration values in %RH.
// The following 2 are the output values of the analog-to-digital converter
// (in LSB) corresponding to the calibration values.

// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

int main (int argc, char **argv) {
  string port ("/dev/ttyACM0");

  if (argc > 1) {

    port = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  Master mb (Rtu, port , "9600N1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  Slave & slv = mb.addSlave (2); // SolarPi Pressure meter

  cout << "Reads holding registers of slave[" << slv.number() << "] on " <<
       mb.connection() << " (" << mb.settings() << ")" << endl;

  if (mb.open ()) { // open a connection
    // success, do what you want here

    // the bytes in the registers are arranged in big endian.
    // the solarpi calibration registers are arranged in little endian.
    Data<uint16_t, EndianBigBig> registers_in[16];
    Data<uint16_t, EndianBigBig> register_out = 1;

    // reads values ....
    if (slv.readRegisters (1, registers_in, 16) > 0) {
      // then print them !i
      for (auto i = 0; i < 16; i++) {
        cout << "R" << i << "=" << registers_in[i].value() << endl;
      }
    }
    else {
      cerr << "Unable to read input registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }

    // now let's write some values
    if (slv.writeRegister(16, register_out)) {
      cout << "Wrote registers" << endl;
    }
    else {
      cerr << "Unable to read input registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }

    mb.close();
  }
  else {
    cerr << "Unable to open MODBUS connection to " << port << " : " << mb.lastError() << endl;
    exit (EXIT_FAILURE);
  }

  return 0;
}
/* ========================================================================== */
