#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

// Room related constants
#define MAX_ROOM_NAME_SIZE 50
#define MAX_ROOM_TYPE_SIZE 50
#define MAX_ROOM_CONNECTIONS 6
#define START_ROOM_COUNT 7

// History related constants
#define HISTORY_INITIAL_CAPACITY 8
#define HISTORY_CAPACITY_MULTIPLIER 2

// Filename/Directory related constants
#define MAX_DIR_NAME_SIZE 50
#define MAX_ROOM_FILEPATH_SIZE 100

// Time related constants
#define MAX_TIME_BUFFER_SIZE 50

// Struct object for path history
typedef struct
{
    int count;
    int capacity;
    int* history;
} RoomHistory;

// Struct object for room
typedef struct
{
    int id;
    int connectionCount;
    char connectedRoomNames[MAX_ROOM_CONNECTIONS][MAX_ROOM_NAME_SIZE];
    char name[MAX_ROOM_NAME_SIZE];
    char roomType[MAX_ROOM_TYPE_SIZE];
} Room;

// Struct for passing filename and mutex thread
// responsible for writing the current time to 
// the specified file
typedef struct
{
    char* filename;
    pthread_mutex_t mutex;
    pthread_mutex_t exitMutex;
} TimeFileArg;

// Adds a room id entry to the room history object
void addToRoomHistory(RoomHistory* history, int roomId)
{
    // Double the history size if full
    if(history->count >= history->capacity)
    {
        // Increase the capacity size and create new int array with that size
        history->capacity *= HISTORY_CAPACITY_MULTIPLIER;
        int* newHistory = malloc(history->capacity * sizeof(int));

        // Copy over the old room ids to the new history array
        int i;

        for(i = 0; i < history->count; i++)
            newHistory[i] = history->history[i];

        // Free the old history array and set it to the new bigger array
        free(history->history);
        history->history = newHistory;
    }

    // Add room id to history array, inc size
    history->history[history->count] = roomId;
    history->count++;
}

// Initialize history object size, capacity, and array
void initializeRoomHistory(RoomHistory* history)
{
    history->count = 0;
    history->capacity = HISTORY_INITIAL_CAPACITY;
    history->history = malloc(history->capacity * sizeof(int));
}

// Gets the dir name of the newest dir that contains room files
void getNewestDirName(char* dirName)
{
    // Newest modification time
    int newestDirTime = -1;

    // Prefix in which the dir that holds room files has
    char* dirPrefix = "klimanf.rooms.";

    // Objects for dir entry and dir attributes
    struct dirent* dirEntry;
    struct stat dirAttr;

    // Open current dir to loop over
    DIR* startDir = opendir(".");

    if(startDir > 0)
    {
        // Loop through each dir/file in current dir
        while((dirEntry = readdir(startDir)) != NULL)
        {
            // Check if it has the wanted prefix
            if(strstr(dirEntry->d_name, dirPrefix) != NULL)
            {
                // Get information about the dir
                stat(dirEntry->d_name, &dirAttr);

                // Check modification date of current dir with 
                // current newest dir date
                if((int) dirAttr.st_mtime > newestDirTime)
                {
                    // Updates newest date if current dir is higher 
                    // and copies over dir name
                    newestDirTime = (int) dirAttr.st_mtime;
                    strcpy(dirName, dirEntry->d_name);
                }
            }
        }
    }
    
    // Close current dir
    closedir(startDir);
}

// Sets the room to the data from the passed in filename
void getRoomFromFile(Room* room, char* dirName, char* filename)
{
    // Filename rooms file
    char filepath[MAX_ROOM_FILEPATH_SIZE];
    sprintf(filepath, "%s/%s", dirName, filename);

    // Prefixes for various room information
    char* roomNamePrefix = "ROOM NAME: ";
    char* connectionPrefix = "CONNECTION ";
    char* roomTypePrefix = "ROOM TYPE: ";
    char* colon = ": ";

    // Index for room connections
    int connectionIndex = 0;
    
    // File obj to read room data from
    FILE* fp = fopen(filepath, "r");
    
    // Buffer size and variable to hold a line of data from the room file
    size_t bufSize = 0;
    char* lineRead = NULL;

    // Process each line in the file
    while(getline(&lineRead, &bufSize, fp) != -1)
    {
        // Get rid the newline
        lineRead[strlen(lineRead) - 1] = '\0';
        
        // Will point to matching found word in line from file
        char* match = NULL;

        // If line contains room name data
        if((match = strstr(lineRead, roomNamePrefix)) != NULL)
        {
            memset(room->name, '\0', MAX_ROOM_NAME_SIZE);
            strcpy(room->name, &match[strlen(roomNamePrefix)]);
        }

        // If line contains connected room data
        else if((match = strstr(lineRead, connectionPrefix)) != NULL)
        {
            memset(room->connectedRoomNames[connectionIndex], '\0', MAX_ROOM_NAME_SIZE);
            match = strstr(lineRead, colon);
            strcpy(room->connectedRoomNames[connectionIndex], &match[strlen(colon)]);
            connectionIndex++;
        }

        // If line contains room type data
        else if((match = strstr(lineRead, roomTypePrefix)) != NULL)
        {
            memset(room->roomType, '\0', MAX_ROOM_TYPE_SIZE);
            strcpy(room->roomType, &match[strlen(roomTypePrefix)]);
        }

        // Gotta free the lineRead and reset it to not have memory leaks
        free(lineRead);
        lineRead = NULL;
    }

    // Have to free lineRead since it only gets freed if valid data
    // was read before
    free(lineRead);

    // Update the connection count for the room and close the file
    room->connectionCount = connectionIndex;
    fclose(fp);
}

// Gets room data from newest directory containing room files
void initializeRooms(Room* rooms)
{
    // Dir name of the newest dir with room files
    char dirName[MAX_DIR_NAME_SIZE] = { 0 };
    getNewestDirName(dirName);

    // Prefix for room files
    char* roomPrefix = "room_";

    // Obj for dir entry
    struct dirent* dirEntry;

    // Open dir that contains room files
    DIR* roomsDir = opendir(dirName);

    if(roomsDir > 0)
    {
        // Used for setting room id and looping through rooms array
        int roomIndex = 0;

        // Loop through each dir entry as long as rooms still need to be initialized
        while(roomIndex < START_ROOM_COUNT && (dirEntry = readdir(roomsDir)) != NULL)
        {
            // If dir entry name contains room file prefix
            if(strstr(dirEntry->d_name, roomPrefix) != NULL)
            {
                // Set room id and get room data from dir entry room file
                rooms[roomIndex].id = roomIndex;
                getRoomFromFile(&rooms[roomIndex], dirName, dirEntry->d_name);
                roomIndex++;
            }
        }
    }

    // Close dir containing room files
    closedir(roomsDir);
}

// Returns true (1) if the passed in room name is one of the names
// in the passed in room connected room names, otherwise return
// false (0)
int isConnectedRoomName(Room* room, char* name)
{
    // Loop through each connected room name for the room,
    // comparing the name with the passed in name
    int i;

    for(i = 0; i < room->connectionCount; i++)
        if(strcmp(room->connectedRoomNames[i], name) == 0)
            return 1;

    return 0;
}

// Returns the address of the room with the name passed in
// otherwise returns NULL
Room* getRoomByName(Room* rooms, char* name)
{
    // Loop through each room, comparing name with the
    // passed in name
    int i;

    for(i = 0; i < START_ROOM_COUNT; i++)
        if(strcmp(rooms[i].name, name) == 0)
            return &rooms[i];

    return NULL;
}

// Displays the current location which is the room passed in
// and displays all connected room names of the passed in room
void displayCurrentRoom(Room* curRoom)
{
    printf("CURRENT LOCATION: %s\n", curRoom->name);
    printf("POSSIBLE CONNECTIONS: ");

    // Loop through each connected room name of passed in name,
    // displaying it to the user
    int i;

    for(i = 0; i < curRoom->connectionCount; i++)
    {
        printf("%s", curRoom->connectedRoomNames[i]);

        // Display ", " if its not the last room name
        if(i < curRoom->connectionCount - 1)
            printf(", ");
        else
            printf(".\n");
    }
}

// Get the current time from a file
char* getTimeFromFile(TimeFileArg* timeFileArg)
{
    // Lock the mutex to be begin reading from the file
    pthread_mutex_lock(&timeFileArg->mutex);

    // Open the time file and read the first line from it
    FILE* fp = fopen(timeFileArg->filename, "r");
    size_t bufSize = 0;
    char* lineRead = NULL;
    getline(&lineRead, &bufSize, fp);
    fclose(fp);
    
    // Get rid of the newline
    lineRead[strlen(lineRead) - 1] = '\0';

    // Unlock the mutext since we're done reading from the file
    pthread_mutex_unlock(&timeFileArg->mutex);
    
    // Return the current time
    return lineRead;
}

// Save the current time to a file
void* saveTimeToFile(void* arg)
{
    // Convert the arg to TimeFileArg*
    TimeFileArg* timeFileArg = (TimeFileArg*) arg;
    
    // Keep writing the time as long as the exitMutex is locked
    // by the main thread
    while(pthread_mutex_trylock(&timeFileArg->exitMutex) != 0)
    {
        // Lock the mutex to begin writing to the file
        pthread_mutex_lock(&timeFileArg->mutex);

        // Get the current time
        time_t rawTime;
        time(&rawTime);
        struct tm* timeInfo= localtime(&rawTime);
        char timeBuffer[MAX_TIME_BUFFER_SIZE];
    
        // Convert the time to specified format
        // 1:03pm, Tuesday, September 13, 2016
        strftime(timeBuffer, MAX_TIME_BUFFER_SIZE, "%l:%M%P, %A, %B %d, %Y", timeInfo);
   
        // Open/Create the time file and write the current date to it
        FILE* fp = fopen(timeFileArg->filename, "w");
        fprintf(fp, "%s\n", timeBuffer);
        fclose(fp);
    
        // Unlock the mutext since we're done writing the time to the file
        pthread_mutex_unlock(&timeFileArg->mutex);
        sleep(1);
    }

    return 0;
}

// Gets a line of text from the user
// the caller will have to free the result
char* getUserInput()
{
    // Buffer size and variable to hold input data from the user
    size_t bufSize = 0;
    char* lineRead = NULL;

    // Get user input and remove the newline from it
    getline(&lineRead, &bufSize, stdin);
    lineRead[strlen(lineRead) - 1] = '\0';

    // Return user input
    return lineRead;
}

// Plays the adventure game
void play(Room* rooms)
{
    // Create and initialize the history obj to hold the history of rooms
    // the user has visited
    RoomHistory* history = malloc(sizeof(RoomHistory));
    initializeRoomHistory(history);

    // Will hold the start/end room
    Room* curRoom;
    Room* endRoom;

    // Loop over the rooms, finding the start/end room
    int i;

    for(i = 0; i < START_ROOM_COUNT; i++)
    {
        char* roomType = rooms[i].roomType;

        if(strcmp(roomType, "START_ROOM") == 0)
            curRoom = &rooms[i];

        else if(strcmp(roomType, "END_ROOM") == 0)
            endRoom = &rooms[i];
    }

    // TimeFileArg struct for creating/writing/reading
    // from a time file, includes filename and mutex
    // to ensure only one thread is accessing the file
    TimeFileArg timeFileArg = { 
        "currentTime.txt", // Filename for time to be written/read from
        PTHREAD_MUTEX_INITIALIZER, // Mutex for file stuff
        PTHREAD_MUTEX_INITIALIZER, // Mutex for keeping write time thread alive
    };

    // Lock the exit mutex to keep write time thread alive
    pthread_mutex_lock(&timeFileArg.exitMutex);

    // Write the current time to a file in a separate thread
    pthread_t writeTimeThreadId = NULL;
    pthread_create(&writeTimeThreadId, NULL, saveTimeToFile, (void*) &timeFileArg);

    // Keep looping as long as the current room is not the same as the end room
    while(curRoom->id != endRoom->id)
    {
        // Display information about current location and room
        displayCurrentRoom(curRoom);

        // Room name from user
        char* roomName;

        // Display the current time if the user types in "time"
        while(1)
        {
            // Get the room name from the user
            printf("WHERE TO? >");
            roomName = getUserInput();

            // If room name is "time" the user wants the current time displayed
            if(strcmp(roomName, "time") == 0)
            {
                // Read the current time from the file
                char* currentTime = getTimeFromFile(&timeFileArg);
                
                // Display the current time
                printf("\n%s\n\n", currentTime);

                // Free the char* that was allocated from reading the current time
                // and from reading input from the user
                free(currentTime);
                free(roomName);
            }

            else
                break;
        }

        // Check if the room name from the user is a valid room
        // connected to the current room
        if(isConnectedRoomName(curRoom, roomName))
        {
            // Get the room object with the name passed in by the user
            // and add its room id to the room history
            curRoom = getRoomByName(rooms, roomName);
            addToRoomHistory(history, curRoom->id);
        }

        // Print error message if room name passed in by the user
        // is not connected to the current room
        else
        {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        }

        // Free the variable allocated by getUserInput()
        free(roomName);
    }

    // Display outro message and the number of steps it took to get to the
    // end room
    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEP(S). YOUR PATH TO VICTORY WAS:\n", history->count);

    // Display each room that the user visited on his way to the end room
    for(i = 0; i < history->count; i++)
        printf("%s\n", rooms[history->history[i]].name);

    // Free up the history object to not have memory leaks!
    free(history->history);
    free(history);

    // Unlock the exit mutex to signal the end of the program and wait for it to exit
    pthread_mutex_unlock(&timeFileArg.exitMutex);
    pthread_join(writeTimeThreadId, NULL);

    // Destroy the mutexes
    pthread_mutex_destroy(&timeFileArg.mutex);
    pthread_mutex_destroy(&timeFileArg.exitMutex);
}

// Entry point for program
int main()
{
    // Create and initialize the rooms
    Room rooms[START_ROOM_COUNT];
    initializeRooms(rooms);
    
    // Begin playing the adventure game
    play(rooms);
    
    return 0;
}

