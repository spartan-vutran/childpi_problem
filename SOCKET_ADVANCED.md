TOPIC 5: SOCKET ADVANCED
===


## 1. Partial read an write
There are circumstances where read() and write() can just read or write a partial number of character as requested. 

Use readn() or writen() if needed instead to read and write exactly number of characters desired.

## 2. Shutdown() to close one haft of a bidirectional channel

## 3. Socket specific I/O system calls: recv() and send()

## 4. The sendfile() system call:

We could use ``sendfile()`` to perform zero-copy technique which could be used to increase performance when reading a file and passing to the socket without the need of buffer and 2 system calls.

```cpp
#include <sys/socket.h>
int shutdown(int sockfd, int how); //Returns 0 on success, or –1 on error
```

