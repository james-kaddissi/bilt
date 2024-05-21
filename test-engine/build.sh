set echo on

mkdir -p ../bin

cFilenames=$(find . -type f -name "*.c")



assembly="test-engine"
compilerFlags="-g -fdeclspec -fPIC" 

includeFlags="-Isrc -I../bilt-engine/src/"
linkerFlags="-L../bin/ -lbilt-engine -Wl,-rpath,."
defines="-D_DEBUG -DKIMPORT"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags