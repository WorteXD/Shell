# MyShell - Custom Unix-like Shell

## 📌 Project Overview
MyShell is a lightweight custom shell that replicates essential Unix commands. This shell was developed as part of an **Operating Systems course assignment** and follows strict structural and functional guidelines outlined in the course material.

## 🛠 Installation & Compilation
To compile MyShell, navigate to the project directory and run:
```bash
make
```
This generates the `myShell` executable.

To clean up object files:
```bash
make clean
```

## 🚀 Running MyShell
After compiling, launch MyShell by executing:
```bash
./myShell
```
You will see a custom **welcome screen** followed by a prompt.

## 🔥 Implemented Commands
MyShell supports various commands, including:

### 1️⃣ **File & Directory Management**
- `cd <path>`: Change the working directory.
- `cp <source> <destination>`: Copy a file from source to destination.
- `mv <source> <destination>`: Move or rename a file.
- `delete <file>`: Remove a file.
- `dir`: List files in the current directory.

### 2️⃣ **I/O Redirection & Piping**
- `command1 | command2`: Pipe output from one command to another.
- `echoppend <file> <text>`: Append text to a file.
- `echowrite <file> <text>`: Overwrite text in a file.
- `read <file>`: Read and display file contents.

### 3️⃣ **Word Count (WC) Implementation**
- `wc -l <file>`: Count lines in a file.
- `wc -w <file>`: Count words in a file.

### 4️⃣ **System Call Execution**
- Runs any valid system command (e.g., `ls`, `cat`, `pwd`).

### 5️⃣ **Exit & Logout**
- `exit`: Closes MyShell.

## 📖 Usage Examples
```bash
cd /home/user/documents
cp file1.txt file2.txt
mv oldname.txt newname.txt
delete temp.log
dir
echo Hello, World! > myfile.txt
echoppend myfile.txt "Appended text"
echowrite myfile.txt "Overwritten content"
read myfile.txt
wc -l myfile.txt
ls | grep "*.c"
exit
```

## ⚠️ Error Handling
- Invalid paths: Displays an error message.
- Missing arguments: Prompts user to provide correct syntax.
- Memory management: Ensures no memory leaks.

## 📌 GitHub Repository
🔗 [https://github.com/WorteXD/Shell](https://github.com/WorteXD/Shell)

## 👨‍💻 Developed By
**Nikita** - Ariel University, Operating Systems Course (2025)

### Bug Fixes (Latest Update)
✅ Fixed segmentation fault caused by improper memory handling.
✅ Improved systemCall() to handle NULL-terminated arguments correctly.
✅ Fixed mypipe() to prevent crashes when fork() fails.
✅ Enhanced splitArguments() to avoid realloc errors.
