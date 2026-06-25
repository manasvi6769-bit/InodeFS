#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "../include/filesystem.h"
using namespace std;



int main()
{
    FileSystem fs;

    cout
    << "User Space File System\n";

    cout
    << "Milestone 3.5\n";

    string line;

    while(true)
    {
        cout << "fs> ";

        getline(cin,line);

        if(line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string cmd;

        ss >> cmd;

        
        // mkdir
        
        if(cmd=="mkdir")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "mkdir: missing operand\n";

                continue;
            }

            fs.mkdir(path);
        }

        
        // touch
        

        else if(cmd=="touch")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "touch: missing operand\n";

                continue;
            }

            fs.touch(path);
        }

        
        // ls
        

        else if(cmd=="ls")
        {
            string path;

            ss >> path;

            fs.ls(path);
        }

        
        // cd
        

        else if(cmd=="cd")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "cd: missing operand\n";

                continue;
            }

            fs.cd(path);
        }

        
        // pwd
        
        else if(cmd=="pwd")
        {
            fs.pwd();
        }

        
        // rm
        

        else if(cmd=="rm")
        {
            string arg;

            ss >> arg;

            if(arg.empty())
            {
                cout
                << "rm: missing operand\n";

                continue;
            }

            if(arg=="-r")
            {
                string path;

                ss >> path;

                fs.rmRecursive(path);
            }
            else
            {
                fs.rm(arg);
            }
        }

        
        // tree
        

        else if(cmd=="tree")
        {
            fs.tree();
        }

        
        // stat
        

        else if(cmd=="stat")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "stat: missing operand\n";

                continue;
            }

            fs.stat(path);
        }

        //--------------------------------
        // write
        //--------------------------------

        else if(cmd=="write")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "write: missing operand\n";

                continue;
            }

            cout
            << "Enter content:\n";

            string content;

            getline(cin,content);

            fs.writeFile(
                path,
                content
            );
        }

        
        // append
        

        else if(cmd=="append")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "append: missing operand\n";

                continue;
            }

            cout
            << "Enter content:\n";

            string content;

            getline(cin,content);

            fs.appendFile(
                path,
                content
            );
        }

        
        // cat
        

        else if(cmd=="cat")
        {
            string path;

            ss >> path;

            if(path.empty())
            {
                cout
                << "cat: missing operand\n";

                continue;
            }

            fs.cat(path);
        }

        
        // blocks
        

        else if(cmd=="blocks")
        {
            fs.blocks();
        }


        else if(cmd=="save")
        {
            fs.saveDisk();
        }

        else if(cmd=="load")
        {
            fs.loadDisk();
        }
        
        
        // exit
        

        else if(cmd=="exit")
        {
            cout
            << "Exiting filesystem...\n";

            break;
        }

        
        // unknown
        

        else
        {
            cout
            << "Unknown command\n";
        }
    }

    return 0;
}