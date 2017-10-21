#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_ROOM_NAMES 10
#define MAX_ROOM_CONNECTIONS 6
#define MIN_ROOM_CONNECTIONS 3
#define START_ROOM_COUNT 7

// Struct object for a room
typedef struct
{
    int id;
    int connectionCount;
    int connections[MAX_ROOM_CONNECTIONS];
    char* name;
    char* roomType;
} Room;

// Creates directory and file for each room
void createRoomFiles(Room* rooms)
{
    // Create directory to hold the room files
    char dirName[50];
    sprintf(dirName, "klimanf.rooms.%d", getpid());
    mkdir(dirName, 0755);

    // Create a file for each room containing room data
    int i;

    for(i = 0; i < START_ROOM_COUNT; i++)
    {
        Room* room = &rooms[i];
        
        // Concatenate dirname and filename for filename
        char filename[100];
        sprintf(filename, "%s/%s", dirName,  room->name);
        
        // Pointer to file for writing room data to
        FILE *fp;
        fp = fopen(filename, "w+");

        // Write the room name
        fprintf(fp, "ROOM NAME: %s\n", room->name);
        
        // Write the name of each room the current room is connected to
        int j;

        for(j = 0; j < room->connectionCount; j++)
            fprintf(fp, "CONNECTION %d: %s\n", j + 1, rooms[room->connections[j]].name);

        // Write the room type
        fprintf(fp, "ROOM TYPE: %s\n", room->roomType);
        
        fclose(fp);
    }
}

// Checks if a room can have more connections
// Return false (0) if the amount of the connections the room has is
// greater than or equal to the max allowed room connections, returns
// true (1) otherwise
int roomCanConnect(Room* room)
{
    if(room->connectionCount >= MAX_ROOM_CONNECTIONS)
        return 0;

    return 1;
}

// Checks if roomB is already connected to roomA
// Loops through roomA connections and checks for roomB's id
// returns true (1) if roommB's id is found, return false (0)
// otherwise
int roomsConnected(Room* roomA, Room* roomB)
{
    int i;

    for(i = 0; i < roomA->connectionCount; i++)
        if(roomA->connections[i] == roomB->id)
            return 1;

    return 0;
}

// Checks if roomA is roomB
// Returns true (1) if roomA id is the same as roomB id,
// otherwise returns false (0)
int sameRooms(Room* roomA, Room* roomB)
{
    if(roomA->id == roomB->id)
        return 1;

    return 0;
}

// Creates a connection between roomA and roomB
// Adds roomB's id to roomA's connection and vice versa
// Also incrememnts roomA and roomB connections count by 1
void connectRooms(Room* roomA, Room* roomB)
{
    roomA->connections[roomA->connectionCount] = roomB->id;
    roomB->connections[roomB->connectionCount] = roomA->id;
    roomA->connectionCount++;
    roomB->connectionCount++;
}

// Checks each room has at least min connections
// Loops through each room checking how many connection the room has,
// returns false (0) if a room has below the minimum amount of connections
// allowed, otherwise returns true (1) 
int isGraphFull(Room* rooms)
{
    int i;

    for(i = 0; i < START_ROOM_COUNT; i++)
        if(rooms[i].connectionCount < MIN_ROOM_CONNECTIONS)
            return 0;

    return 1;
}

// Initialize each rooms id, connectionCount, name, roomType
// Assigns each room an id, connectionCount to 0, randomly assigned
// name and roomType
void initializeRooms(Room* rooms)
{
    // For checking if a name has already been taken by a room, 0 means its available
    int roomNameUsed[MAX_ROOM_NAMES] = { 0 };
    char* roomNames[MAX_ROOM_NAMES] = {
        "ALPHA",
        "BRAVO",
        "CHARLIE",
        "DELTA",
        "ECHO",
        "FOXTROT",
        "GOLF",
        "HOTEL",
        "INDIA",
        "JULIET"        
    };

    // Indices for which rooms will be the start/end room
    int startRoomIndex = rand() % START_ROOM_COUNT;
    int endRoomIndex;

    do {
        endRoomIndex = rand() % START_ROOM_COUNT;
    } while(endRoomIndex == startRoomIndex);

    // Loops through each room assigning an id, connectionCount to 0,
    // random name and roomType based on start/end room indices
    int i;

    for(i = 0; i < START_ROOM_COUNT; i++)
    {
        rooms[i].id = i;
        rooms[i].connectionCount = 0;
        rooms[i].name = NULL;

        // Assign random name
        while(rooms[i].name == NULL)
        {
            int nameIndex = rand() % MAX_ROOM_NAMES;

            // Only assign a name that hasn't been taken
            if(roomNameUsed[nameIndex] == 0)
            {
                rooms[i].name = roomNames[nameIndex];
                roomNameUsed[nameIndex] = 1;
            } 
        }

        // Assign room type based on start/end indices
        if(i == startRoomIndex)
            rooms[i].roomType = "START_ROOM";
        else if(i == endRoomIndex)
            rooms[i].roomType = "END_ROOM";
        else
            rooms[i].roomType = "MID_ROOM";
    }
}

// Entry point for the program
int main()
{
    // Seed random number generator
    srand(time(NULL));

    // Initialize 7 rooms
    Room rooms[START_ROOM_COUNT];
    initializeRooms(rooms);
    
    // Add room connections
    // Check if graph is full
    while(!isGraphFull(rooms))
    {
        Room* roomA;
        Room* roomB;

        // Choose random room A that can have connections
        do {
            roomA = &rooms[rand() % START_ROOM_COUNT];
        } while(!roomCanConnect(roomA));

        // Choose random room B that can have connection to A
        do {
            roomB = &rooms[rand() % START_ROOM_COUNT];
        } while(!roomCanConnect(roomB) || sameRooms(roomA, roomB) || roomsConnected(roomA, roomB));
        
        // Create connection between room A and room B
        connectRooms(roomA, roomB);
    }
    
    // Write room data to files
    createRoomFiles(rooms);

    return 0;
}

