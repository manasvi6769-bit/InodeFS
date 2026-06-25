#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

struct Inode
{
    int inodeNumber;

    bool isDirectory;

    int parentInode;

    int size;

    string permissions;

    vector<int> dataBlocks;
};

class FileSystem
{
private:

    unordered_map<int, Inode> inodeTable;

    unordered_map<
        int,
        unordered_map<string,int>
    > directoryData;

    int currentDirectory;

    int nextInode;

    static const int BLOCK_SIZE = 64;

    static const int NUM_BLOCKS = 1024;

    vector<string> diskBlocks;

    vector<bool> freeBlockBitmap;

public:

    FileSystem();

    void mkdir(const string& path);

    void touch(const string& path);

    void ls(const string& path="");

    void cd(const string& path);

    void pwd();

    void rm(const string& path);

    void rmRecursive(const string& path);

    void tree();

    void stat(const string& path);

    void writeFile(const string& path, const string& content);

    void cat(const string& path);
    
    void blocks();

    void appendFile(const string& path,const string& content);

    ~FileSystem();

    void saveDisk();

    void loadDisk();

private:

    int createInode(
        bool isDirectory,
        int parent
    );

    vector<string> splitPath(
        const string& path
    );

    int resolvePath(
        const string& path
    );

    pair<int,string>
    getParentAndName(
        const string& path
    );

    void removeInodeRecursive(int inode);

    void treeHelper(
        int inode,
        const string& name,
        const string& prefix
    );

    int allocateBlock();

    void freeBlock(int block);

    vector<string> splitByPipe(const string& line);
};
#endif