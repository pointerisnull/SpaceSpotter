#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "data.h"
#include "net.h"
#include "io.h"

State state;

void checkUpdates(Lot *lots) {
  char *buffer = malloc(sizeof(char)*256);
  int done = 0;
  while(done != 1) {
    readSerialData(buffer);
   // if(buffer != NULL) printf("%s\n", buffer);
    if(buffer[strlen(buffer)-3] == 't') done = 1;
  //  printf("strlen: %d\n", strlen(buffer));
  }
  int currentIndex = 0;
  for(int l = 0; l < state.lotCount; l++) {
    currentIndex++;
    for(int s = 0; s < lots[l].spaceCount; s++) {
      if(buffer[currentIndex + s] != lots[l].space[s].isOccupied) {
        lots[l].space[s].isOccupied = buffer[currentIndex + s] - '0';
        if(lots[l].space[s].isOccupied == 1) {
          lots[l].space[s].timeTaken = time(NULL);
          lots[l].space[s].popularity++;
        }
      }
      makeUpdatePacket("update.JSON", 0, lots[l].space[s].isOccupied, l, s, lots[l].space[s].popularity, lots[l].space[s].timeTaken);
      sendPacket("update.JSON", "/api/update/space", 0);
      printf("SENT: %s\n", buffer);
    }
  }
  free(buffer);
}

Lot *init() {
  system("sudo chmod a+rw /dev/ttyACM0");
  system("sudo udevadm trigger");
  state.RUNNING = 1;
  char *buffer = malloc(sizeof(char)*256);
  int done = 0;
  while(done != 1) {
    readSerialData(buffer);
    if(buffer[strlen(buffer)-3] == 't') done = 1;
  }
 // printf("init: %s\n", buffer);
  int lotCount;
  for(int i = 0; i < strlen(buffer); i++) if(buffer[i] == 'b') lotCount++;
  state.lotCount = lotCount;
  Lot *lots = malloc(sizeof(Lot)*lotCount);

  int spaceCount = 0;
  for(int i = 0; i < lotCount; i++) {
    for(int j = 0; j < strlen(buffer)-1; j++) {
      if(buffer[j] == '0' || buffer[j] == '1')
        spaceCount++;
    }
    lots[i].spaceCount = spaceCount;
  }

 
  for(int i = 0; i < state.lotCount; i++) {
    lots[i].id = i;
    lots[i].space = malloc(sizeof(Space)*lots[i].spaceCount);
    for(int j = 0; j < lots[i].spaceCount; j++) {

      lots[i].space[j].id = j;
      lots[i].space[j].isOccupied = rand() %2;
      lots[i].space[j].timeTaken = 0;
      lots[i].space[j].popularity = 0;
    }
  }
  int currentIndex = 0;
  for(int l = 0; l < state.lotCount; l++) {
    currentIndex++;
    for(int s = 0; s < lots[l].spaceCount; s++) {
      if(buffer[currentIndex + s] != lots[l].space[s].isOccupied) {
        lots[l].space[s].isOccupied = buffer[currentIndex + s] - '0';
        if(lots[l].space[s].isOccupied == 1) {
          lots[l].space[s].timeTaken = time(NULL);
          lots[l].space[s].popularity++;
        }
      }
    }
  }
  free(buffer);
  return lots;
}

int main(int argc, char *argv[]) {

  //state.lotCount = rand()%200 +1;

  Lot *lots = init();
  /* if new lots, ping /api/new/lot. check for new lots every boot */
  /* boot with api/update/lot with current lots */
  
  for(int i = 0; i < state.lotCount; i++) {
    makeInitPacket("init.JSON", 0, lots[i]);
    sendPacket("init.JSON", "/api/new/lot", 0);
    sendPacket("init.JSON", "/api/update/lot", 0);
  } 
  printf("fin\n");

  while(state.RUNNING) {
    checkUpdates(lots);
 
  } 
  return 0;
}
