```bash
#!/bin/bash

echo "========================================="
echo " Building UserSpaceFileSystem"
echo "========================================="

mkdir -p data

touch data/disk.img

g++ \
    src/main.cpp \
    src/filesystem.cpp \
    -Iinclude \
    -std=c++17 \
    -Wall \
    -Wextra \
    -O2 \
    -o filesystem

if [ $? -eq 0 ]; then
    echo ""
    echo "Build Successful!"
    echo ""
    echo "Run the program using:"
    echo "./filesystem"
else
    echo ""
    echo "Build Failed!"
fi
```
