# Operating-System-Programming
All the codes in this repository are done in C programming language. Various concepts of operating system such as signals, multi threaded programming, Linux shell etc are implemented using C.

my_shell.c : The code emulates the LINUX SHELL using system calls fork(), exec(), exit(), wait().

mylsn.c : The codes implements the exact behavior of linux command ls -lLR 
          (-L argument means to resolve symlinks for files within directories and display information for the file they point to)
          (-R argument means to list subdirectories recursively)
          (-l argument means use long listing format to display information about directory contents )

IntrQuitTermSig.c : The code implements signal handler to catch signals (SIGINT), SIGQUIT, SIGTSTP) 
                    whenever the user types a ^C (interrupt, SIGINT) print an I,
                    whenever the user types a ^\ (quit, SIGQUIT) print a Q, flush,
                    whenever the user types a ^Z (terminal stop, SIGTSTP) print an S. 
                    On the third ^Z, print a summary of the number of signals received on new line.
                    
usr1usr2Intsig.c : The code implements user defined signals to communicate between two processes.
                   The process receives SIGUSR1 and SIGINT signals from other process. In response to SIGUSR1 signal the process
                   sends SIGUSR2 signal using process id of other signal.
                   In response to SIGINT signal print the number of SIGUSR1 signal received from other process then execute exit.
