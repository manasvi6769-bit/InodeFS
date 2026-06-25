# UserSpaceFileSystem Design Document

## Overview

UserSpaceFileSystem is a miniature Unix-like filesystem implemented entirely in C++. The objective of this project is to understand how operating systems organize files, directories and storage internally without relying on the host operating system's filesystem APIs.

The project implements an inode-based filesystem supporting persistent storage through a custom disk image.

---

# Architecture

```
                User

                 │

                 ▼

          Interactive Shell

                 │

                 ▼

          Path Resolution Layer

                 │

                 ▼

        Directory Entry Lookup

                 │

                 ▼

              Inode Table

                 │

                 ▼

          Block Allocation Layer

                 │

                 ▼

             Data Blocks

                 │

                 ▼

          data/disk.img
```

---

# Components

## 1. Inodes

Every file and directory is represented using an inode.

Each inode stores

* inode number
* parent inode
* file type
* file size
* permissions
* list of allocated data blocks

The inode never stores filenames.

---

## 2. Directory Entries

Directories are implemented as

```cpp
unordered_map<
    int,
    unordered_map<string,int>
>
```

Each directory inode maps

```
filename
        ↓
inode number
```

Example

```
home
 ├── docs
 └── images
```

Internally

```
directoryData[0]

home -> inode 1

directoryData[1]

docs -> inode 2

images -> inode 3
```

---

## 3. Block Storage

Files store their contents inside fixed-size blocks.

Each inode contains

```
vector<int> dataBlocks;
```

which stores the indices of allocated blocks.

The actual file contents reside inside

```
diskBlocks
```

---

## 4. Free Block Bitmap

A bitmap tracks whether each block is free or allocated.

```
0 → free

1 → allocated
```

Allocation scans for the first free block.

Deletion releases all associated blocks.

---

## 5. Path Resolution

Supported paths

* Relative paths
* Absolute paths
* "."
* ".."

Examples

```
mkdir home/docs

touch home/docs/file.txt

cd ../images

cd /home/docs
```

---

# Persistent Storage

The entire filesystem is serialized into

```
data/disk.img
```

The following information is saved

* next available inode
* inode table
* directory entries
* free block bitmap
* allocated data blocks

Upon startup, the filesystem reconstructs itself by deserializing this file.

---

# Serialization Format

The disk image is stored as a human-readable text format.

Example

```
NEXT_INODE

7

INODES

...

DIRECTORIES

...

BITMAP

...

BLOCKS

...
```

Fields are separated using the '|' delimiter to safely preserve spaces inside file contents.

Example

```
5|Hello IIT Delhi
```

---

# Supported Commands

Directory Operations

* mkdir
* touch
* ls
* cd
* pwd
* tree

File Operations

* write
* append
* cat

Metadata

* stat
* blocks

Deletion

* rm
* rm -r

Persistence

* saveDisk()
* loadDisk()

---

# Time Complexity

| Operation | Complexity           |
| --------- | -------------------- |
| mkdir     | O(depth)             |
| touch     | O(depth)             |
| ls        | O(number of entries) |
| cd        | O(depth)             |
| stat      | O(depth)             |
| write     | O(number of blocks)  |
| append    | O(appended blocks)   |
| cat       | O(number of blocks)  |
| rm        | O(number of blocks)  |
| rm -r     | O(size of subtree)   |

---

# Future Improvements

* Unix permissions (chmod)
* Hard links
* Symbolic links
* Open file table
* Journaling
* Defragmentation
* Binary disk image
* Caching
* Multi-user support

---

# Concepts Demonstrated

* Operating Systems
* Filesystem Design
* Inode Management
* Directory Hierarchies
* Block Allocation
* Persistent Storage
* Serialization
* Object-Oriented Programming
* Systems Programming
