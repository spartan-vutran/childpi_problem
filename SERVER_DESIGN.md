Topic 4. Server design
========

## 1. Preforked and prethreaded servers

Using servel pool techniques. Two ways of implementation is:
+ Parent call accept(), then pass the file descriptor containning the new connection to one of the free processes in the pool.
+ Some way must impost to allow only one of a child process call accept(). Noting that the accept() is not atomic funtion so we must enclose it with some mutual exculusion technique.

The advangtage of this technique is that we can reduce the amount of time to create a new process for serving a new request from the client. Trade off with sotisphicated method of controlling thread pool.

## 2. Hanling multiple client from a single process
A single server is reponsible for controlling multiple request by applying some I/O models that allow a sigle process to simultaneously monitor multiple file descriptors for I/O events;

## 3. Server farm
Multiple servers are hosted.