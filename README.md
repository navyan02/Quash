# Quash

## Introduction

Quash is a simple UNIX shell implemented in C. This project aims to familiarize users with the UNIX system interface, exercise UNIX system calls, and provide a practical understanding of process management from a user perspective. Quash mimics the behavior of popular shell programs such as `bash` and `csh`.

## Features

Quash implements the following features:

- Run executables with command-line parameters.
- Background and foreground execution of commands.
- I/O redirection for standard input and output.
- Support for pipes to connect command outputs to inputs.
- Built-in functions:
  - `echo`: Print strings to standard output.
  - `cd`: Change the current working directory.
  - `pwd`: Print the current working directory.
  - `export`: Set and read environment variables.
  - `jobs`: List currently running background processes.
  - `quit` and `exit`: Terminate the shell.

## Building and Running

To build quash, you can use the provided [Makefile](makefile):

```
make quash
```

To run quash, execute:

```
./quash or make test
```

To clean the executable file:

```
make clean
```
## Functionality
Quash should be able to run executables (the basic function of a shell) with command line parameters
If the executable is not specified in the absolute or relative path format (starting with sequences of ‘/’, './', or '../'), quash should search the directories in the environment variable PATH (see below). If no executable file is found, quash should print an error message to standard error. Quash should allow both foreground and background executions. Character ‘&’ is used to indicate background execution. Commands without ‘&’ are assumed to run in foreground.
When a command is run in the background, quash should print: "Background job started: [JOBID] PID COMMAND"
When a background command finishes, quash should print: "Completed: [JOBID] PID COMMAND"

To create a Background Job:
```
[QUASH]$ program1 &
Background job started: [1] 2342 program1 &
[QUASH]$ ls
Documents Downloads
Completed: [1] 2342 program1 &
```

I/O Redirection:
```
[QUASH]$ echo Hello Quash! > a.txt # Write "Hello Quash!\n" to a file
[QUASH]$ cat a.txt
Hello Quash!
[QUASH]$ echo Hey Quash! > a.txt # Truncate the previous contents of a.txt and write
          "Hey Quash!\n" to the file
[QUASH]$ cat a.txt # Print file contents. If we didn't actually truncate we would see
          "Hey Quash!h!\n" as the output of this command.
Hey Quash!
[QUASH]$ cat < a.txt # Make cat read from a.txt via standard in
Hey Quash!
[QUASH]$ cat < a.txt > b.txt # Multiple redirect. Read from a.txt and write to b.txt.
        
[QUASH]$ cat b.txt
Hey Quash!
[QUASH]$ cat a.txt >> b.txt # Append output of a.txt to b.txt
[QUASH]$ cat b.txt
Hey Quash!
Hey Quash!
[QUASH]$
```

Pipes | 
```
[QUASH]$ cat src/quash.c | grep running
// Check if loop is running
bool is_running() {
 return state.running;
 state.running = false;
 while (is_running()) {
[QUASH]$ cat src/quash.c | grep running | grep return
 return state.running;
```

## Built-In Functions

echo - Print a string given as an argument. The output format should be the same as bash (a string followed by new line '\n')
```
[QUASH]$ echo Hello world! 'How are you today?'
Hello world! How are you today?
[QUASH]$ echo $HOME/Development
/home/jrobinson/Development
[QUASH]$
```

export - Sets the value of an environment variable. Quash should support reading from and writing to environment variables.
```
[QUASH]$ export PATH=/usr/bin:/bin # Set the PATH environment variable
[QUASH]$ echo $PATH # Print the current value of PATH
/usr/bin:/bin
[QUASH]$ echo $HOME
/home/jrobinson
[QUASH]$ export PATH=$HOME # Set the PATH environment variable to the value of HOME
[QUASH]$ echo $PATH # Print the current value of PATH
/home/jrobinson
[QUASH]$
```

cd - Change current working directory. This updates both the actual working directory and the PWD environment variable.
```
[QUASH]$ echo $PWD
/home/jrobinson
[QUASH]$ cd .. # Go up one directory
[QUASH]$ echo $PWD
/home
[QUASH]$ cd $HOME # Go to path in the HOME environment variable
/home/jrobinson
[QUASH]$
```

pwd - Print the absolute path of the current working directory. Make sure you are printing out the actual working directory and not just the PWD environment variable.
```
[QUASH]$ pwd # Print the working directory
/home/jrobinson
[QUASH]$ echo $PWD # Print the PWD environment variable
/home/jrobinson
[QUASH]$ export PWD=/usr # Change the PWD environment variable
[QUASH]$ pwd
/home/jrobinson
[QUASH]$ echo $PWD
/usr
[QUASH]$
```

quit & exit - Use these to terminate quash. These are already implemented for you.
```
[BASH]$ ./quash
Welcome...
[QUASH]$ exit
[BASH]$ ./quash
Welcome...
[QUASH]$ quit
[BASH]$
```

jobs - Should print all of the currently running background processes in the format: "[JOBID] PID COMMAND" where JOBID is a unique positive integer quash assigns to the job to identify it, PID is the PID of the child process used for the job, and COMMAND is the command used to invoke the job.
```
[QUASH]$ find . -name *.c > out.txt &
Background job started: [1] 2342 find . -name *.c > out.txt &
[QUASH]$ sleep 15 &
Background job started: [2] 2343 sleep 15 &
[QUASH]$ jobs # List currently running background jobs
[1] 2342 find . -name *.c > out.txt &
[2] 2343 sleep 15 &
[QUASH]$
```

## Contributors

Navya Nittala & Shayaan Mohammed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
