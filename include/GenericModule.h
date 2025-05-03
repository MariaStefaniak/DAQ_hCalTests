#ifndef GENERICMODULE_H
#define GENERICMODULE_H

#include "Common.h"
#include "Module.h"

/**
   @brief GenericModule class. Most basic kind of module.
   Has standard reading, writing, clearing commands.
   Example is Lecroy 2249W. Data is most simple 12bit.
*/
class GenericModule : public Module{

 public:
  /**
     @brief Default constructor for GenericModule.
   */
  GenericModule();

  /**
     @brief Overloaded constructor for GenericModule.

     @param param1 GenericModule name
     @param param2 Number of channels in this module
     @param param3 Slot number of this module in CAMAC
     @param param4 Command (F) used for reading
     @param param5 Number of digits for data
  */
    
  GenericModule( std::string, int, int, int, int);

  /**
     @brief Default destructor for GenericModule
   */
  virtual ~GenericModule();

  /**
     @brief Function to connect to MyDaq's SharedData

     @param param1 Pointer to shared data
  */
//  virtual void RegisterSharedData( SharedData* );

  /**
     @brief Function adds reading with clear to stack.

     @param param1 Reference to stack being built
  */
  virtual void AddClearingReadToStack( std::vector<long>& );

  /**
     @brief Function creates simple CAMAC (N,A,F) command.

     @param param1 N value
     @param param2 A value
     @param param3 F value
     @return long representing CAMAC command
  */
  virtual long CreateSimpleCommand( int, int, int );

  /**
     @brief Function adds output for given module
  */
  virtual void AddOutput ();

  /**
     @brief Function reads data from module

     @param param1 Address where module's data beings
  */
  virtual void ReadData( short* , TH1D *histo, TTree *tree, int *channels);

  /**
     @brief Function to fill histograms for module
   */
  virtual void FillHistograms(int test = 0);

  /**
     @brief Function prints info about module
  */
  virtual void Print();


};

#endif
