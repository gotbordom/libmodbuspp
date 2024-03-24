// Read/Writes floating point holding registers to SolarPi pressure meter
// the MODBUS master is configured from the JSON ./master.json file

// The pressure sensor calibration is composed of 4 decimal values.
// The first 2 are the minimum and maximum pressure calibration values in hPa.
// The following 2 are the output values of the analog-to-digital converter
// (in LSB) corresponding to the calibration values.

// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>
#include <modbuspp/popl.h>

using namespace std;
using namespace Modbus;

int main(int argc, char **argv)
{
  string jsonfile("./master.json");

  if (argc > 1)
  {

    jsonfile = argv[1]; // the JSON filename can be provided as a parameter on the command line.
  }

  Master mb(jsonfile, "modbuspp-master");
  Slave &slv = mb.slave(2); // SolarPi Pressure meter

  cout << "Read/Write holding registers of slave[" << slv.number() << "] on " << mb.connection() << " (" << mb.settings() << ")" << endl;

  if (mb.open())
  { // open a connection
    // success, do what you want here
    int ret;

    Data<uint16_t, EndianBigBig> backup[16];
    // the bytes in the registers are arranged in big endian.
    // the solarpi calibration registers are arranged in little endian.
    // Data<uint16_t, EndianBigBig> bank1[16];
    Data<uint16_t, EndianBigBig> bank2[16];
    Data<uint16_t, EndianBig> turn_on = 0x0100;
    Data<uint16_t, EndianBig> turn_off = 0x0200;
    Data<uint16_t, EndianBig> turn_on_off = 0x0300;

    // reads values ....
    if (slv.readRegisters(1, backup, 16) > 0)
    {

      // then print them !
      cout << "Reading registers" << endl;
      for (auto i = 2; i < 18; i++)
      {
        cout << "R" << i << "=" << backup[i].value() << endl;
      }
    }
    else
    {
      cerr << "Unable to read holding registers ! " << mb.lastError() << endl;
      exit(EXIT_FAILURE);
    }

    // write to registers
    for (auto i = 2; i < 18; ++i)
    {
      cout << "Writing reg[" << i << "]" << endl;
      ret = slv.writeRegister(i, turn_on_off);
      if (ret < 0)
      {
        cerr << "Unable to write holding registers ! " << mb.lastError() << endl;
        exit(EXIT_FAILURE);
      }
      else
      {
        cout << ret << " registers written (16-bit)." << endl;
      }
    }
    // // check if the values have been written
    // if (slv.readRegisters (1, bank2, 16) > 0) {
    //   bool ok = true;

    //   for (int i = 0; i < 16; i++) {
    //     ok = ok && (bank1[i] == bank2[i]);
    //   }

    //   if (ok) {

    //     cout << "Registers have been correctly written" << endl;
    //   }
    //   else {

    //     cout << "Registers were not correctly written" << endl;
    //     // then print them !
    //     cout << "R0=" << bank2[0].value() << endl;
    //     cout << "R1=" << bank2[1].value() << endl;
    //     cout << "R2=" << bank2[2].value() << endl;
    //     cout << "R3=" << bank2[3].value() << endl;
    //     cout << "R10=" << bank2[10].value() << endl;
    //     cout << "R11=" << bank2[11].value() << endl;
    //     cout << "R12=" << bank2[12].value() << endl;
    //     cout << "R13=" << bank2[13].value() << endl;
    //   }
    // }
    // else {
    //   cerr << "Unable to read holding registers ! "  << mb.lastError() << endl;
    //   exit (EXIT_FAILURE);
    // }

    mb.close();
  }
  else
  {
    cerr << "Unable to open MODBUS connection to " << mb.connection() << " : "
         << mb.lastError() << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
/* ========================================================================== */
