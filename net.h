#ifndef NET_H
#define NET_H

#include "data.h"

void makeInitPacket(char *name, int iid, Lot lot);
void makeUpdatePacket(char *name, int iid, int isOcc, int lotID, int spotID, int popularity, unsigned long time);
void sendPacket(char *JSONFile, char *path, int iid);

#endif
