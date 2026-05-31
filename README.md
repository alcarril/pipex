pipex
=========

<div align="center">
	<strong>My implementation of shell pipes and process execution</strong>
	<br />
	<a href="https://42.fr/">
		<img src="https://img.shields.io/badge/42-Project-black" alt="42" />
	</a>
	<a href="https://en.cppreference.com/w/c">
		<img src="https://img.shields.io/badge/C-Language-blue" alt="C" />
	</a>
	<a href="https://www.gnu.org/software/make/">
		<img src="https://img.shields.io/badge/GNU-Makefile-green" alt="GNU Makefile" />
	</a>
</div>

<br>

![pipex](img/pipes.jpeg)

## Overview
This project explores multiprocessing and inter-process communication in C. The goal is to recreate the behavior of the shell pipe operator (`|`) and file redirections (`<` and `>`). By building `pipex`, I learned how to create child processes, redirect standard input/output, and safely execute terminal commands from within a C program.

## Key Features

### Core Functionality
- Process Control Block (PCB) management and understanding of OS-level process tracking.
- Multiprocessing and process synchronization using `fork` (for creating child processes) and `waitpid` (to wait for child processes to finish).
- Resource inheritance (such as open file descriptors and environment variables) between parent and child processes.
- Inter-process communication (IPC) via `pipe`.
- File descriptor manipulation and standard I/O redirection using `dup2`.
- Parsing environment variables (like `$PATH`) to locate and run executables with `execve` (the system call that replaces the current process image with a new one).
- Robust error handling for invalid files, command execution failures, and memory leaks.

### ✨ Bonus Features
- **Multipipes:** Capability to handle an indeterminate number of commands, dynamically linking the output of one into the input of the next (`cmd1 | cmd2 | ... | cmdn`).
- **Here_doc (`<<`):** Support for the `here_doc` shell feature. This is implemented by storing the user's standard input into a temporary file created inside the `/tmp` directory, which is safely unlinked (deleted) at the end of its use to ensure no leftover files.
<br>

## 🛠️ Requirements
This project is natively designed and developed for **Unix systems** (Linux/macOS).

Install the required compiler tools and development libraries (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install -y make gcc libc6-dev
```
No external downloads are required for helper functions, as my own custom libraries are pre-integrated directly within this repository:
* [Libft](Include/libft) — My personal C standard library.
* [ft_printf](Include/ft_printf) — My implementation of the formatted print function

---

## 🧱 Build
Clone the repository and compile the executable using the provided Makefile:
```bash
git clone https://github.com/alcarril/pipex.git # Clone the repository
cd pipex # Navigate into the project directory
make        # Builds the standard version
make bonus  # Builds with multipipes and here_doc support
```

---

## ▶️ Run & Test

### 1. Standard Execution
The program takes exactly 4 arguments: two file names and two shell commands.

| Argument | Description |
| --- | --- |
| `file1` | The input file (equivalent to the shell `< file1`). |
| `cmd1` | The first command to execute. Its input comes from `file1`. |
| `cmd2` | The second command. Its input is the output of `cmd1` (via pipe). |
| `file2` | The output file (equivalent to the shell `> file2`). |

**Equivalent shell syntax:**
```bash
< file1 cmd1 | cmd2 > file2
```

**Testing the standard version:**
```bash
echo -e "Hello 42\nThis is a pipex test\nAnother line" > infile
./pipex infile "grep test" "wc -w" outfile
cat outfile
```
*The result in `outfile` will perfectly match the output of running `< infile grep test | wc -w > outfile` in your regular terminal.*

### 2. Bonus Execution
Make sure you compiled the project using `make bonus` before running these tests.

* **Multipipes:** Dynamically links an indeterminate number of commands consecutively.
```bash
./pipex infile "ls -l" "grep pipex" "wc -l" outfile
```
*Equivalent to:* `< infile ls -l | grep pipex | wc -l > outfile`

* **Here_doc:** Replicates the `<<` shell operator, reading input until the specified limiter is reached.
```bash
./pipex here_doc LIMITER "grep test" "wc -w" outfile
```
*Equivalent to:* `grep test << LIMITER | wc -w > outfile`

<br>

## ℹ️ Resources
#### Processes and Pipes
- [Video: Unix Processes in C (fork, wait)](https://www.youtube.com/watch?v=cex9XrZCU14)
- [Video: Simulating the pipe "|" operator in C](https://www.youtube.com/watch?v=6xbLgZpFAcs)
- `man 2 pipe` & `man 2 fork`

#### File Descriptors and Execution
- [Video: Redirection using dup2()](https://www.youtube.com/watch?v=5fnVr-zH-SE)
- [Video: Executing programs with execve()](https://www.youtube.com/watch?v=iq7xIoXidTU)
- `man 2 dup2` & `man 2 execve`

## 👨‍💻 Author
**Alejandro Carrillo** - https://github.com/alcarril

















# pipex_42 🚰

**Pipes, redirections, IPC, process management, global variables, parsing, and more. This project aims to simulate the behavior of Shell pipes using C. The goal of the project... to become a reliable plumber! 🧑‍🔧**

**pipex_42** is a C project focused on simulating the behavior of shell pipes. It includes topics such as pipes, redirections, inter-process communication (IPC), process management, global variables, parsing, and more. The main objective is to reliably reproduce shell pipe functionalities in C—because every good developer needs to be a reliable plumber!

## Features

- Implementation of pipe and redirection mechanisms in C
- Process creation and management
- Inter-process communication (IPC)
- Shell command parsing and execution
- **Bonus:** Multiple commands and "here document" support

## Requirements

- GCC or compatible C compiler
- Unix-like operating system (Linux, macOS)
- Basic knowledge of Shell and C programming

## Installation

Clone the repository:
```sh
git clone https://github.com/alexBickle24/pipex_42.git
cd pipex_42
```

Compile the project:
```sh
make
```

## Usage

### Basic Usage

Execute the program with the following arguments:
```sh
./pipex infile cmd1 cmd2 outfile
```
This command will:
- Take input from `infile`  
- Execute `cmd1`, pipe its output to `cmd2`  
- Write the final output to `outfile`

### Bonus Features 🎉

If you build and use the **bonus version**, you unlock extra functionalities:

#### 1. Multiple Commands

Chain more than two commands, just like in a real shell pipeline:
```sh
./pipex infile cmd1 cmd2 cmd3 ... cmdN outfile
```
This will pipe the output through each command in sequence, from `infile` to `outfile`.

#### 2. Here Document ("here_doc") 📑

Support for “here_doc” syntax, allowing you to provide input directly in the terminal:
```sh
./pipex here_doc LIMITER cmd1 cmd2 ... cmdN outfile
```
This will:
- Read input from the terminal until the `LIMITER` word is encountered
- Process the commands in sequence
- Write the result to `outfile`

## Example

```sh
./pipex infile "grep hello" "wc -l" outfile
```
Or, using bonus features:
```sh
./pipex here_doc END "cat" "wc -l" outfile
```
Then type your input, and finish with `END` on a new line.

## Author

alexBickle24  
[GitHub Profile](https://github.com/alexBickle24)

---

Let me know if you’d like to adjust the emoji placement or add more examples!