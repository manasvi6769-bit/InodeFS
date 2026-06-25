# UserSpaceFileSystem

A persistent user-space file system implemented in **C++** that simulates the core components of a traditional Unix-like file system. The project supports hierarchical directories, inode-based metadata management, block allocation, persistent storage through a disk image, and an interactive shell.

---

## Features

### Directory Operations

* `mkdir` — Create directories
* `touch` — Create empty files
* `ls` — List directory contents
* `cd` — Change current directory
* `pwd` — Print working directory
* `tree` — Display directory hierarchy

### File Operations

* `write` — Write data to a file
* `append` — Append data to an existing file
* `cat` — Display file contents

### Removal Operations

* `rm` — Remove files or empty directories
* `rm -r` — Recursively remove directories

### Metadata

* `stat` — Display inode information
* `blocks` — Display allocated disk blocks

### Persistence

The complete filesystem state is serialized into a persistent disk image (`data/disk.img`).

The following information survives program termination:

* Inode Table
* Directory Structure
* Free Block Bitmap
* Data Blocks
* Next Available Inode

---

# Architecture

The project follows a layered filesystem design.

```
Shell
   │
   ▼
Path Resolution
   │
   ▼
Directory Entries
   │
   ▼
Inode Table
   │
   ▼
Block Allocation
   │
   ▼
Disk Blocks
   │
   ▼
disk.img
```

---

# Internal Data Structures

## Inode

Each file or directory is represented by an inode containing

* inode number
* parent inode
* file type
* permissions
* file size
* allocated data blocks

---

## Directory Entries

Directories are stored as

```
Directory Inode
        │
        ▼
unordered_map<string, int>
```

mapping

```
filename
        │
        ▼
inode number
```

---

## Block Storage

The storage layer consists of

* Fixed-size disk blocks
* Free block bitmap
* Block allocator
* Block deallocator

Files store only block indices inside their inode.

---

# Persistence

When the filesystem exits, the complete state is written to

```
data/disk.img
```

On startup, the filesystem reconstructs

* inode table
* directory tree
* bitmap
* data blocks

allowing all files and directories to persist across executions.

---

# Project Structure

```
UserSpaceFileSystem/

├── include/
│   └── filesystem.h
│
├── src/
│   ├── filesystem.cpp
│   └── main.cpp
│
├── data/
│   └── disk.img
│
├── docs/
│   └── design.md
│
├── tests/
│   └── test_commands.txt
│
├── README.md
├── compile.sh
└── .gitignore
```

---

# Supported Commands

| Command | Description                 |
| ------- | --------------------------- |
| mkdir   | Create directory            |
| touch   | Create file                 |
| ls      | List contents               |
| cd      | Change directory            |
| pwd     | Print working directory     |
| tree    | Display hierarchy           |
| rm      | Remove file/empty directory |
| rm -r   | Recursive removal           |
| stat    | Show inode metadata         |
| write   | Write file contents         |
| append  | Append to file              |
| cat     | Display file contents       |
| blocks  | Display allocated blocks    |
| exit    | Save filesystem and exit    |

---

# Build

```
chmod +x compile.sh

./compile.sh
```

Run the executable

```
./filesystem
```

---

# Example Session

```
mkdir home

mkdir home/docs

touch home/docs/readme.txt

write home/docs/readme.txt

Hello IIT Delhi

cat home/docs/readme.txt

Hello IIT Delhi

tree

/
    home/
        docs/
            readme.txt

exit
```

Restarting the program preserves the filesystem because it is restored from `data/disk.img`.

---

# Concepts Demonstrated

* Operating Systems
* Filesystem Design
* Inode Management
* Hierarchical Directories
* Persistent Storage
* Block Allocation
* Bitmap Allocation
* Serialization / Deserialization
* Object-Oriented Design
* Systems Programming in C++

---

# Future Improvements

* Unix-style permissions (`chmod`)
* Hard links
* Symbolic links
* File descriptors
* Journaling
* Multi-user support
* Extent-based allocation
* Caching
* Defragmentation
* Binary disk image format

---

# Author

Manasvi
Indian Institute of Technology Delhi
