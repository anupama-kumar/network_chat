CIS 505 - Project 2

Milestone 2 - part 4:



While giving arguments to the client, port number should be specified and be given as 2866.

Clients will have ./client-executable localhost 2866  
followed by other arguments if there [arg1]  [arg2]  [arg3] ..

Example :

Run the server: 

./server-tcp 

Run the clients 

./change-password-tcp localhost 2866
./add-voter-tcp localhost 2866 [voterid]
./vote-tcp localhost 2866 [voter-name] [voterid]
./vote-zero-tcp localhost 2866 
./listcandidates localhost 2866
./vote-count-tcp localhost 2866 [voter-name]
./view-result-tcp localhost 2866 [usesrname] [password]

test:

run server and on one client window/terminal open change password , leave it unfinished and move to another window and start a new client and complete it.
return to the unfinished client and complete it. 

Makefile:

run make part4 
to compile all object files.

