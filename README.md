# Controller-Overseer

C language & System Programming
Controller - Overseer program

The overseer program executes a compiled C program. You input some command in controller then the controller program sends the data to overseer program through socket.

The detail of commands are available by inputting a command in controller program by `./controller --help`

1. make
2. Start overseer (./overseer <PORT NUMBER>)
3. Start controller (./controller <IP> <PORT NUMBER> [-o <output file> -log <logfile> -t <timeout seconds>] <WHATEVER C PROGRAM THAT YOU WANT TO EXECUTE WITH ARGUMENTS>
4. The overseer receives requests from controller and execute them.
5. There are 5 threads and so up to 5 requests can be handled at a time. All other requests are appended to overseer's global queue and wait for a space in one of those threads.

