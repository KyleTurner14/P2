#include <linux/mutex.h>

extern struct mutex queueMutex;

extern char message[1024];

int front = 0;
int back = 0;
int size = 0;
