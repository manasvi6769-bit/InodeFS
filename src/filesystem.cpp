#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "../include/filesystem.h"
using namespace std;


// Constructor


FileSystem::FileSystem()
{
    nextInode = 1;

    diskBlocks.resize(NUM_BLOCKS);

    freeBlockBitmap.assign(
        NUM_BLOCKS,
        false
    );

    currentDirectory = 0;

    loadDisk();

    if(inodeTable.empty())
    {
        Inode root;

        root.inodeNumber = 0;
        root.isDirectory = true;
        root.parentInode = 0;
        root.size = 0;
        root.permissions = "rwxrwxrwx";

        inodeTable[0] = root;
    }
}
// Destructor 
FileSystem::~FileSystem()
{
    saveDisk();
}



// Create Inode


int FileSystem::createInode(
    bool isDirectory,
    int parent
)
{
    Inode node;

    node.inodeNumber =
        nextInode;

    node.isDirectory =
        isDirectory;

    node.parentInode =
        parent;

    node.size = 0;

    node.permissions =
        "rwxrwxrwx";

    node.dataBlocks.clear();

    inodeTable[nextInode]
        = node;

    return nextInode++;
}



// Split Path


vector<string>
FileSystem::splitPath(
    const string& path
)
{
    vector<string> parts;

    string cur;

    stringstream ss(path);

    while(
        getline(
            ss,
            cur,
            '/'
        )
    )
    {
        if(!cur.empty())
        {
            parts.push_back(cur);
        }
    }

    return parts;
}



// Resolve Path


int FileSystem::resolvePath(
    const string& path
)
{
        if(path.empty())
        {
            return currentDirectory;
        }

        int current;

        if(path[0] == '/')
        {
            current = 0;
        }
        else
        {
            current =
                currentDirectory;
        }

        vector<string> parts =
            splitPath(path);

        for(
            string part : parts
        )
        {
            if(part == ".")
            {
                continue;
            }

            if(part == "..")
            {
                current =
                    inodeTable[current]
                    .parentInode;

                continue;
            }

            if(
                directoryData[current]
                .count(part) == 0
            )
            {
                return -1;
            }

            current =
                directoryData[current]
                            [part];
        }

        return current;
}



// Parent + Name


pair<int,string>
FileSystem::getParentAndName(
    const string& path
)
{
    size_t pos =
        path.find_last_of('/');

    if(pos == string::npos)
    {
        return {
            currentDirectory,
            path
        };
    }

    string parentPath;

    if(pos == 0)
    {
        parentPath = "/";
    }
    else
    {
        parentPath =
            path.substr(
                0,
                pos
            );
    }

    string name =
        path.substr(pos + 1);

    int parent =
        resolvePath(
            parentPath
        );

    return {
        parent,
        name
    };
}



// mkdir


void FileSystem::mkdir(
    const string& path
)
{
    auto [parent,name] =
        getParentAndName(path);

    if(parent == -1)
    {
        cout
        << "mkdir: parent not found\n";

        return;
    }

    if(
        directoryData[parent]
        .count(name)
    )
    {
        cout
        << "mkdir: already exists\n";

        return;
    }

    int inode =
        createInode(
            true,
            parent
        );

    directoryData[parent]
                 [name]
                 = inode;
}



// touch


void FileSystem::touch(
    const string& path
)
{
    auto [parent,name] =
        getParentAndName(path);

    if(parent == -1)
    {
        cout
        << "touch: parent not found\n";

        return;
    }

    if(
        directoryData[parent]
        .count(name)
    )
    {
        cout
        << "touch: already exists\n";

        return;
    }

    int inode =
        createInode(
            false,
            parent
        );

    directoryData[parent]
                 [name]
                 = inode;
}



// ls

void FileSystem::ls(
    const string& path
)
{
    int inode;

    if(path.empty())
    {
        inode =
            currentDirectory;
    }
    else
    {
        inode =
            resolvePath(path);
    }

    if(inode == -1)
    {
        cout
        << "ls: path not found\n";

        return;
    }

    vector<string> names;

    for(
        auto &entry :
        directoryData[inode]
    )
    {
        string name =
            entry.first;

        int child =
            entry.second;

        if(
            inodeTable[child]
            .isDirectory
        )
        {
            name += "/";
        }

        names.push_back(
            name
        );
    }

    sort(
        names.begin(),
        names.end()
    );

    for(
        string x :
        names
    )
    {
        cout
        << x
        << "\n";
    }
}


// cd


void FileSystem::cd(
    const string& path
)
{
    int inode =
        resolvePath(path);

    if(inode == -1)
    {
        cout
        << "cd: path not found\n";

        return;
    }

    if(
        !inodeTable[inode]
         .isDirectory
    )
    {
        cout
        << "cd: not directory\n";

        return;
    }

    currentDirectory =
        inode;
}



// pwd


void FileSystem::pwd()
{
    if(currentDirectory == 0)
    {
        cout << "/\n";
        return;
    }

    vector<string> path;

    int cur =
        currentDirectory;

    while(cur != 0)
    {
        int parent =
            inodeTable[cur]
            .parentInode;

        for(
            auto &entry :
            directoryData[parent]
        )
        {
            if(
                entry.second
                == cur
            )
            {
                path.push_back(
                    entry.first
                );

                break;
            }
        }

        cur = parent;
    }

    reverse(
        path.begin(),
        path.end()
    );

    cout << "/";

    for(
        int i=0;
        i<path.size();
        i++
    )
    {
        cout
        << path[i];

        if(
            i+1
            <
            path.size()
        )
        {
            cout << "/";
        }
    }

    cout << "\n";
}
// Remove inode Recursive Helper function  indide Private
void FileSystem::removeInodeRecursive(
    int inode
)
{
    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        vector<int> children;

        for(
            auto &entry :
            directoryData[inode]
        )
        {
            children.push_back(
                entry.second
            );
        }

        for(
            int child :
            children
        )
        {
            removeInodeRecursive(
                child
            );
        }

        directoryData.erase(
            inode
        );
    }

    
    for(
        int block :
        inodeTable[inode]
        .dataBlocks
    )
    {
        freeBlock(block);
    }

    inodeTable.erase(
        inode
    );
}

// rm function implementation 
void FileSystem::rm(
    const string& path
)
{
    int inode =
        resolvePath(path);

    if(inode == -1)
    {
        cout
        << "rm: path not found\n";

        return;
    }

    if(inode == 0)
    {
        cout
        << "rm: cannot delete root\n";

        return;
    }

    if(
        inodeTable[inode]
        .isDirectory
        &&
        !directoryData[inode]
            .empty()
    )
    {
        cout
        << "rm: directory not empty\n";

        return;
    }

    int parent =
        inodeTable[inode]
        .parentInode;

    string name;

    for(
        auto &entry :
        directoryData[parent]
    )
    {
        if(
            entry.second
            == inode
        )
        {
            name =
                entry.first;

            break;
        }
    }
    for(
        int block :
        inodeTable[inode]
        .dataBlocks
    )
    {
        freeBlock(block);
    }

    directoryData[parent]
        .erase(name);

    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        directoryData.erase(
            inode
        );
    }

    inodeTable.erase(
        inode
    );
}

// rm -r function 
void FileSystem::rmRecursive(
    const string& path
)
{
    int inode =
        resolvePath(path);

    if(inode == -1)
    {
        cout
        << "rm: path not found\n";

        return;
    }

    if(inode == 0)
    {
        cout
        << "rm: cannot delete root\n";

        return;
    }

    int parent =
        inodeTable[inode]
        .parentInode;

    string name;

    for(
        auto &entry :
        directoryData[parent]
    )
    {
        if(
            entry.second
            == inode
        )
        {
            name =
                entry.first;

            break;
        }
    }

    removeInodeRecursive(
        inode
    );

    directoryData[parent]
        .erase(name);
}

// stat function implementation 
void FileSystem::stat(
    const string& path
)
{
    int inode =
        resolvePath(path);

    if(inode == -1)
    {
        cout
        << "stat: path not found\n";

        return;
    }

    string name = "/";

    if(inode != 0)
    {
        int parent =
            inodeTable[inode]
            .parentInode;

        for(
            auto &entry :
            directoryData[parent]
        )
        {
            if(
                entry.second
                == inode
            )
            {
                name =
                    entry.first;

                break;
            }
        }
    }

    cout<< "Name        : "<< name<< "\n";

    cout<< "Inode       : "<< inode<< "\n";

    cout<< "Type        : "<< (inodeTable[inode].isDirectory?"Directory":"File")<< "\n";

    cout<< "Parent      : "<< inodeTable[inode].parentInode<< "\n";

    cout<< "Size        : "<< inodeTable[inode].size<< "\n";

    cout<< "Permissions : "<< inodeTable[inode].permissions<< "\n";

    cout<< "Blocks      : "<< inodeTable[inode].dataBlocks.size()<< "\n";
}

// Tree Helper Function inside private
void FileSystem::treeHelper(
    int inode,
    const string& name,
    const string& prefix
)
{
    cout<< prefix<< name;

    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        cout << "/";
    }

    cout << "\n";

    vector<
        pair<string,int>
    > children;

    for(
        auto &entry :
        directoryData[inode]
    )
    {
        children.push_back(
        {
            entry.first,
            entry.second
        });
    }

    sort(
        children.begin(),
        children.end()
    );

    for(
        auto &child :
        children
    )
    {
        treeHelper(
            child.second,
            child.first,
            prefix + "    "
        );
    }
}

// tree function implementation 
void FileSystem::tree()
{
    cout << "/\n";

    vector<
        pair<string,int>
    > children;

    for(
        auto &entry :
        directoryData[0]
    )
    {
        children.push_back(
        {
            entry.first,
            entry.second
        });
    }

    sort(
        children.begin(),
        children.end()
    );

    for(
        auto &child :
        children
    )
    {
        treeHelper(
            child.second,
            child.first,
            "    "
        );
    }
}

// Allocate Blocks code (private function)
int FileSystem::allocateBlock()
{
    for(int i=0;
        i<NUM_BLOCKS;
        i++)
    {
        if(
            !freeBlockBitmap[i]
        )
        {
            freeBlockBitmap[i]
                = true;

            return i;
        }
    }

    return -1;
}

// Free Block function private
void FileSystem::freeBlock(
    int block
)
{
    if(
        block < 0
        ||
        block >= NUM_BLOCKS
    )
    {
        return;
    }

    freeBlockBitmap[block]
        = false;

    diskBlocks[block] = "";
}
// Block function private
void FileSystem::blocks()
{
    cout
    << "Allocated Blocks\n";

    for(
        int i=0;
        i<NUM_BLOCKS;
        i++
    )
    {
        if(
            freeBlockBitmap[i]
        )
        {
            cout<< "Block "<< i<< " : "<< diskBlocks[i]<< "\n";
        }
    }
}
// write function
void FileSystem::writeFile(
    const string& path,
    const string& content
)
{
    int inode =
        resolvePath(path);

    if(inode==-1)
    {
        cout
        << "write: file not found\n";

        return;
    }

    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        cout
        << "write: cannot write directory\n";

        return;
    }

    Inode &file =
        inodeTable[inode];

    // free old block
    for(
        int block :
        file.dataBlocks
    )
    {
        freeBlock(block);
    }

    file.dataBlocks.clear();

    //allocate new block

    file.size =
        content.size();

    int pos = 0;

    while(
        pos < content.size()
    )
    {
        int block =
            allocateBlock();

        if(block==-1)
        {
            cout
            << "Disk full\n";

            return;
        }

        string chunk =
            content.substr(
                pos,
                BLOCK_SIZE
            );

        diskBlocks[block]
            = chunk;

        file.dataBlocks
            .push_back(block);

        pos += BLOCK_SIZE;
    }
}
// cat function
void FileSystem::cat(
    const string& path
)
{
    int inode =
        resolvePath(path);

    if(inode==-1)
    {
        cout
        << "cat: file not found\n";

        return;
    }

    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        cout
        << "cat: is directory\n";

        return;
    }

    for(
        int block :
        inodeTable[inode]
        .dataBlocks
    )
    {
        cout<< diskBlocks[block];
    }

    cout << "\n";
}
//append function
void FileSystem::appendFile(
    const string& path,
    const string& content
)
{
    int inode =
        resolvePath(path);

    if(inode==-1)
    {
        cout
        << "append: file not found\n";

        return;
    }

    if(
        inodeTable[inode]
        .isDirectory
    )
    {
        cout
        << "append: is directory\n";

        return;
    }

    Inode& file =
        inodeTable[inode];

    int pos = 0;

    
    // Try filling last block first
    if(
        !file.dataBlocks.empty()
    )
    {
        int lastBlock =
            file.dataBlocks.back();

        int used =
            diskBlocks[lastBlock]
            .size();

        int freeSpace =
            BLOCK_SIZE - used;

        int take =
            min(
                freeSpace,
                (int)(content.size()-pos)
            );

        diskBlocks[lastBlock]
            +=
            content.substr(
                0,
                take
            );

        pos += take;
    }

    
    // Allocate new blocks

    while(
        pos < content.size()
    )
    {
        int block =
            allocateBlock();

        if(block==-1)
        {
            cout
            << "Disk full\n";

            return;
        }

        string chunk =
            content.substr(
                pos,
                BLOCK_SIZE
            );

        diskBlocks[block]
            = chunk;

        file.dataBlocks
            .push_back(block);

        pos += BLOCK_SIZE;
    }

    file.size +=
        content.size();
}

// split by pipe function private
vector<string>
FileSystem::splitByPipe(
    const string& line
)
{
    vector<string> parts;

    string cur;

    stringstream ss(line);

    while(getline(ss,cur,'|'))
    {
        parts.push_back(cur);
    }

    return parts;
}
// save to disk 
void FileSystem::saveDisk()
{
    ofstream out("data/disk.img");

    if(!out)
        return;

    
    // NEXT INODE
   

    out << "NEXT_INODE\n";

    out << nextInode << "\n";

    
    // INODES
    

    out << "INODES\n";

    out << inodeTable.size() << "\n";

    for(auto &p : inodeTable)
    {
        Inode node = p.second;

        out
        << node.inodeNumber << "|"
        << node.isDirectory << "|"
        << node.parentInode << "|"
        << node.size << "|"
        << node.permissions << "|"
        << node.dataBlocks.size();

        for(int block : node.dataBlocks)
        {
            out << "|" << block;
        }

        out << "\n";
    }

    
    // DIRECTORIES
    

    out << "DIRECTORIES\n";

    out << directoryData.size()
        << "\n";

    for(auto &dir : directoryData)
    {
        out
        << dir.first
        << "|"
        << dir.second.size()
        << "\n";

        for(auto &entry : dir.second)
        {
            out
            << entry.first
            << "|"
            << entry.second
            << "\n";
        }
    }

    
    // BITMAP
    

    vector<int> usedBlocks;

    for(int i=0;i<NUM_BLOCKS;i++)
    {
        if(freeBlockBitmap[i])
        {
            usedBlocks.push_back(i);
        }
    }

    out << "BITMAP\n";

    out << usedBlocks.size()
        << "\n";

    for(int block : usedBlocks)
    {
        out << block << "\n";
    }

    
    // BLOCKS
    

    out << "BLOCKS\n";

    out << usedBlocks.size()
        << "\n";

    for(int block : usedBlocks)
    {
        out
        << block
        << "|"
        << diskBlocks[block]
        << "\n";
    }

    out.close();
}

//load disk 
void FileSystem::loadDisk()
{
    ifstream in("data/disk.img");

    if(!in)
        return;

    inodeTable.clear();

    directoryData.clear();

    fill(
        freeBlockBitmap.begin(),
        freeBlockBitmap.end(),
        false
    );

    string line;

    
    // NEXT_INODE
   

    getline(in,line);

    if(line!="NEXT_INODE")
        return;

    getline(in,line);

    nextInode =
        stoi(line);

    
    // INODES
    

    getline(in,line);

    getline(in,line);

    int inodeCount =
        stoi(line);

    for(int i=0;i<inodeCount;i++)
    {
        getline(in,line);

        vector<string> parts =
            splitByPipe(line);

        Inode node;

        node.inodeNumber =
            stoi(parts[0]);

        node.isDirectory =
            stoi(parts[1]);

        node.parentInode =
            stoi(parts[2]);

        node.size =
            stoi(parts[3]);

        node.permissions =
            parts[4];

        int numBlocks =
            stoi(parts[5]);

        for(int j=0;j<numBlocks;j++)
        {
            node.dataBlocks.push_back(
                stoi(parts[6+j])
            );
        }

        inodeTable[
            node.inodeNumber
        ] = node;
    }

    
    // DIRECTORIES
    

    getline(in,line);

    getline(in,line);

    int dirCount =
        stoi(line);

    for(int i=0;i<dirCount;i++)
    {
        getline(in,line);

        vector<string> info =
            splitByPipe(line);

        int dirInode =
            stoi(info[0]);

        int entries =
            stoi(info[1]);

        for(int j=0;j<entries;j++)
        {
            getline(in,line);

            vector<string> entry =
                splitByPipe(line);

            directoryData
            [dirInode]
            [entry[0]]
            =
            stoi(entry[1]);
        }
    }

    
    // BITMAP
    

    getline(in,line);

    getline(in,line);

    int used =
        stoi(line);

    for(int i=0;i<used;i++)
    {
        getline(in,line);

        int block =
            stoi(line);

        freeBlockBitmap[block]
            = true;
    }

    // BLOCKS

    getline(in,line);

    getline(in,line);

    int blockCount =
        stoi(line);

    for(int i=0;i<blockCount;i++)
    {
        getline(in,line);

        size_t pos =
            line.find('|');

        int block =
            stoi(
                line.substr(
                    0,
                    pos
                )
            );

        string content =
            line.substr(
                pos+1
            );

        diskBlocks[block]
            = content;
    }

    in.close();
}
