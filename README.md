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

To check for memory leaks using Valgrind, execute:

```
make quash_valgrind
```

To clean the executable file:

```
make clean
```

## Contributors

Navya Nittala & Shayaan Mohammed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
