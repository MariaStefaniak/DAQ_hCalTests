#include "MyDaq.h"

#include "GenericModule.h"

#include <TApplication.h>
#include <iostream>

int main (int argc,  char *argv[])
{
  MyDaq* daq = new MyDaq(); // main object
  /** Adding the modules  */
  // 1. ADC module, 12 channels, slot 1, 2 - reading function, 1024 bit output //
  daq->AddModule( new GenericModule("ADC", 12, 1, 2, 1024));
 // daq->AddModule( new GenericModule("ADC", 12, 7, 2, 1024));
    
  daq->Run();
  return 0;
}
