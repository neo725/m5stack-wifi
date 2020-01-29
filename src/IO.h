#ifndef IO_h__
#define IO_h__

#include "FS.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void readFile(fs::FS &fs, const char * path, String &result, int &index);
void writeFile(fs::FS &fs, const char * path, const char * message);

#endif