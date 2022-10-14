TOPIC 6: DAEMON
===

## 1. Creating a daemon
Process of creating deamon
1. Fork a child.
2. Create new session and removing current associated terminal.
3. Clear the umask.
4. Change the current working directory to '/'
5. Close all open file descriptor that the daemon inherited from its parent.
6. Map 1,2,3 file descriptors to /dev/null
