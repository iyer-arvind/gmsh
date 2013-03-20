#ifndef _WRITERDUMMY_H_
#define _WRITERDUMMY_H_

#include "Writer.h"

/**
   @class WriterDummy
   @brief A Dummy Writer

   This class is a Writer that writes @em noting.
*/

class WriterDummy: public Writer{
 public:
  WriterDummy(void);

  virtual ~WriterDummy(void);

  virtual void write(const std::string name) const;
  virtual void write(const std::string name,
                     const std::string type) const;
};

/**
   @fn WriterDummy::WriterDummy
   Instantiates a new WriterDummy
   **

   @fn WriterDummy::~WriterDummy
   Deletes this WriterDummy
   **

   @fn void WriterDummy::write(const std::string) const
   @param name The name of the file to write into
   (@em without extensions)

   Does @em nothing
   **

   @fn void WriterDummy::write(const std::string, const std::string) const
   @param name The name of the file to write into
   (@em without extensions)
   @param type A string

   Does @em nothing
   **
*/

#endif
