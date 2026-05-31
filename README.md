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

## 💡 Overview
This project explores multiprocessing and inter-process communication in C. The goal is to recreate the behavior of the shell pipe operator (`|`) and file redirections (`<` and `>`). By building `pipex`, I learned how to create child processes, redirect standard input/output, and safely execute terminal commands from within a C program.

## ✨ Key Features

### Core Functionality
- Process Control Block (PCB) management and understanding of OS-level process tracking.
- Multiprocessing and process synchronization using `fork` (for creating child processes) and `waitpid` (to wait for child processes to finish).
- Resource inheritance (such as open file descriptors and environment variables) between parent and child processes.
- Inter-process communication (IPC) via `pipe`.
- File descriptor manipulation and standard I/O redirection using `dup2`.
- Parsing environment variables (like `$PATH`) to locate and run executables with `execve` (the system call that replaces the current process image with a new one).
- Robust error handling for invalid files, command execution failures, and memory leaks.

### Bonus Features
- **Multipipes:** Capability to handle an indeterminate number of commands, dynamically linking the output of one into the input of the next (`cmd1 | cmd2 | ... | cmdn`).
- **Here_doc (`<<`):** Support for the `here_doc` shell feature. This is implemented by storing the user's standard input into a temporary file created inside the `/tmp` directory, which is safely unlinked (deleted) at the end of its use to ensure no leftover files.

<br>

## 🛠️ Requirements
This project is natively designed and developed for **Unix systems** (Linux/macOS).
My toolchain includes:
* [Libft](Include/libft) — My personal C standard library.
* [ft_printf](Include/ft_printf) — My implementation of the formatted print function

Install the required compiler tools and development libraries (Ubuntu/Debian):
```bash
sudo apt update # Update package lists
sudo apt install -y make # Install GNU Make
sudo apt install -y gcc libc6-dev # Install GCC and C std headers
```



## 🧱 Build
Clone the repository and compile the executable using the provided Makefile:
```bash
git clone https://github.com/alcarril/pipex.git # Clone the repository
cd pipex # Navigate into the project directory
make        # Builds the standard version
make bonus  # Builds with multipipes and here_doc support
```



## ▶️ Run & Test

### 1. Standard Execution
---
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

---

### 2. Bonus Execution

---
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

## 📖 Some: Core OS Concepts

### 1. The Process Control Block (PCB)

---

Every time `pipex` spawns a process using `fork()`, the operating system kernel creates a tracking structure known as the **Process Control Block (PCB)**. The PCB represents the process in the kernel memory table. It holds critical metadata used by the OS scheduler:
* **PID (Process Identifier):** A unique numerical ID given to differentiate parent from child.
* **Process State:** Tracks whether the process is *Running*, *Ready*, or *Waiting* (e.g., waiting for `waitpid` or waiting for pipe data).
* **CPU Register State:** Saves instruction pointers and stack indicators when context-switching.
* **Memory Management:** Maps virtual memory pages allocated to that exact execution.
* **File Descriptor Table:** A private index of integer pointers mapping to open system files or pipe ends.

> ⚠️ **Note on Implementation:** The architecture managed within this repository simulates a **minimal and conceptual version** of a PCB focused on user-space process synchronization. Real kernel-level PCBs contain hundreds of architectural status flags, network tokens, and complex signal-handling bitmasks.

---

### 2. Process Cloning & Resource Inheritance

---

When `fork()` is called, the parent process creates a near-identical clone of itself. However, rather than copying entire RAM segments immediately (which would be incredibly slow), modern Unix kernels optimize this via **Copy-on-Write (COW)**.

| Resource Type | Shared or Duplicated? | Behavioral Mechanism |
| --- | --- | --- |
| **Memory Space (Stack/Heap)** | **Duplicated (COW)** | Parent and child point to the *same physical RAM pages* initially. If either tries to modify a variable, the kernel catches the write flag, duplicates that exact page, and isolates the modification. Variables are **not** shared synchronously. |
| **File Descriptors (FDs)** | **Duplicated Reference** | The FD table indices are duplicated into the child's PCB. Crucially, they point to the *same underlying Open File Table entries* in the kernel. Moving a read/write offset in the child affects the parent. |
| **Environment Variables** | **Duplicated** | The array of context parameters (`envp`) is accurately duplicated, preserving access to configuration blocks. |

<p align="center">
  <img src="img/process_forking_copyonwrite.webp" alt="Process Fork and Copy on Write Diagram" width="650">
  <br>
  <em>Figure 2: Memory Space Duplication via Copy-on-Write (COW) Mechanics</em>
</p>

---

### 3. How Bash Locates Executables & Links `execve`

---

When executing a command string like `"grep test"`, the system cannot run `"grep"` raw; it requires an absolute binary path (e.g., `/usr/bin/grep`). To replicate Bash's path resolution, `pipex` logic follows these sequential steps:

1. **Extract Environment Matrix:** The `main` function intercepts the `char **envp` parameter passed by the operating system.
2. **Find the `$PATH` Variable:** It loops through `envp` to locate the string starting with `PATH=`.
3. **Tokenize Directories:** Using a custom string splitter (`ft_split`), it extracts all potential binary directories using `:` as the delimiter (e.g., `/bin`, `/usr/bin`, `/usr/local/bin`).
4. **Path Construction:** It dynamically appends a slash and the base command string to each path directory (e.g., `/usr/bin` + `/` + `grep` $\rightarrow$ `/usr/bin/grep`).
5. **Validation via `access()`:** It runs a verification loop check utilizing `access(constructed_path, X_OK)`. This systemic system call checks whether the file exists and holds active execution permissions.
6. **Execution via `execve`:** Once a functional directory path matches, the verified path is pushed into `execve(path, args, envp)`. If successful, the existing child process code is completely overwritten by the binary executable's code block.

> [!NOTE]
> 🧠 **Comprehensive Process Documentation:** I have created a detailed Notion workspace covering all these OS concepts, advanced process management mechanics, and everything necessary to successfully build and understand this project from scratch. You can explore the full guide here: [Notion — Procesos & Pipex Guide](https://broken-snowdrop-f03.notion.site/Procesos-165b80eb3d88809cb1e4ff3cb634e1fc?pvs=74).

<br>

## ⚙️ Practical Process & Pipe Management

Managing multiple processes and pipes can easily lead to deadlocks, memory leaks, or hanging terminals if file descriptors (FDs) are not strictly controlled. Below is a highly scannable tactical blueprint of the architectural rules applied in this project:

### 1. Pipe Anatomy & Kernel Buffering
* **The Channel:** A pipe is a unidirectional ring buffer managed inside the kernel memory space (typically capped at 64KB).
    ```text
    Data Flow:  [Writer] ---> fd[1] (Write) ===[ KERNEL BUFFER ]===> fd[0] (Read) ---> [Reader]
    ```
* **Blocking Behavior:** * **Buffer Full:** If a child writes faster than the next can read, the kernel saturates and puts the writer to sleep automatically until space frees up.
    * **Buffer Empty:** Reading from an empty pipe blocks the consumer process until new data is pushed.

### 2. Atomic Redirection via `dup2()`
* **The Mechanics:** `dup2(oldfd, newfd)` forces `newfd` to target the exact same file/stream as `oldfd`. If `newfd` is already open, the kernel safely closes it first in an atomic operation.
* **Local Scope vs. Master Table:** > [!TIP]
    > Performing `dup2` redirections *inside the child scope* immediately after `fork()` keeps modifications local. This leaves the parent’s master FD table completely untouched and stable, preventing cross-contamination during subsequent pipeline loops.

### 3. Execution Sequencing
* **The Rule:** You must initialize the descriptor array using `pipe()` **prior** to calling `fork()`.
* **The Blueprint:**
    ```text
     CORRECT:  [Parent] -> pipe() -> fork() -> Child inherits SHARED kernel channel.
   INCORRECT:  [Parent] -> fork() -> pipe() -> Parent & Child get SEPARATE isolated channels.
    ```

### 4. The Reference Count & Hanging Trap
* **The Mechanics:** Commands like `grep`, `wc`, or `cat` read continuously until they receive an **EOF (End-of-File)** signal (when `read()` returns 0 bytes).
* **The Trap:** The kernel tracks a global reference counter for every open FD. The system will **never** trigger an EOF signal on a pipe if even *one* copy of its write end (`fd[1]`) remains open anywhere in any process control block.

> [!WARNING]
> **The Hanging Symptom:** If the parent process or a sibling forgets to close its copy of `fd[1]`, the reading process will wait indefinitely, freezing your terminal.

### 5. File Descriptor Table Saturation
* **The Cap:** Operating systems enforce strict concurrent open file limits per process (e.g., `RLIMIT_NOFILE`, often capped at 1024).
* **Asymmetric Cleanup:**
    * **In the Child:** Once `dup2()` replaces `STDIN`/`STDOUT`, the original pipe descriptors become redundant and must be closed immediately.
    * **In the Parent:** The parent must close its master copies of the pipe ends as soon as it finishes spawning the target children to prevent structural resource leaks.

### 6. Memory Overwrites & FD Survival
* **The Point of No Return:** A successful `execve(path, args, envp)` call completely wipes out the current child process's user-space memory (stack, heap, and code segments), replacing it entirely with the target binary.
* **FD Persistence:** Open file descriptors **survive an `execve()` call** by default. This allows the newly loaded binary (`grep`, `wc`, etc.) to inherit the pipe ends you set up.

> [!IMPORTANT]
> Because code located *after* a successful `execve` will never run, error handling and cleanup routines must be placed directly underneath the call to handle rare failure cases (e.g., execution denied).

### 7. Zombie Cleanup via `waitpid`
* **The Lifecycle:** When a child process completes execution, it doesn't disappear. It enters a **Zombie state**, freeing its RAM footprint but locking its PID and exit status inside the kernel's process table.
* **The Solution:** The parent process is strictly obligated to reap these dead statuses using `waitpid()`. Neglecting this causes zombie accumulation, which eventually saturates the OS maximum PID table and blocks the creation of any new system processes.

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

<br>

## 👨‍💻 Author
**Alejandro Carrillo** - https://github.com/alcarril

















