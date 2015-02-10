#ifndef _SMALLFEM_H_
#define _SMALLFEM_H_

#include "mpi.h"
#include "Options.h"

#include <complex>
#include <string>

/**
   @class SmallFem
   @brief Initialize and finalize SmallFem sessions.

   Initialize and finalize SmallFem sessions.
   It also handels this SmallFem session Options.

   A SmallFem session has the following default Options keywords
   @li -solver
*/

/**
   @typedef Complex
   @brief A short cut for std::complex<double>

   A short cut for std::complex<double>
 */

typedef std::complex<double> Complex;

class SmallFem{
 private:
  static bool        initOne;
  static bool        finaOne;
  static Options*    option;
  static std::string myKeywords;

 public:
   SmallFem(void);
  ~SmallFem(void);

  static void Keywords(const std::string& keywords);
  static void Initialize(int argc, char** argv);
  static void Finalize(void);

  static Options& getOptions(void);
};

/**
   @fn SmallFem::SmallFem
   Instantiates a new SmallFem

   Not needed, since SmallFem is bunch of static class
   **

   @fn SmallFem::~SmallFem
   Deletes this SmallFem

   Not needed, since SmallFem is bunch of static class
   **

   @fn SmallFem::Keywords
   @param keywords A string with comma separated keywords

   This sets new valid Options keywords for this SmallFem session
   from the given string.

   If new keywords are to be set, SmallFem::Keywords() must be called
   before SmallFem::Initialize().
   Otherwise, an Exception is throw.

   @see SmallFem::getOptions()
   **

   @fn SmallFem::Initialize
   @param argv A vector of char*
   @param argc The size of the previous vector

   This class method initializes SmallFem.
   Moreover it parses {argv[0], ..., argv[argc - 1]} for Options.

   @see SmallFem::getOptions()
   **

   @fn SmallFem::Finalize
   This class method finalizes SmallFem
   **

   @fn SmallFem::getOptions
   @return Returns the Options found in SmallFem::Initialize()
   **
*/

#endif
