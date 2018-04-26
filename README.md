# FileSystem
File System Simulator

To run our code:
1. Compile our code using the makefile.
2. Once the code has been successfulled compiled, in command line type "./main" to run the built object file and then pass in 4 arguments. The form of the four arguments is as follows: 
A dir_list.txt file (a text file with a list of directories), a file_list.txt file (a text file with a list of files and each file's information), disk size (size of the memory disk), and block size (how big each block in disk should be).
3. If an invalid argument is passed in, our code will output an error message and the user will have to input valid arguments.
4. Once four valid arguments have been passed in command line, our code will create the directory tree of directories, assign files correspondingly to each directory, and place the files in disk. Our code will now show root directory in command line and will wait for user input.
5. Based on the commands that were asked of us to implement, our code will wait for the user to give these valid commands as input and run those commands accordingly. For instance, giving "cd [directory]" as input in command line will change the current directory to the directory given as input. Only one command can be run at a time.
6. Once the user has decided to finish running the program, the command "exit" can be used to exit the program.
