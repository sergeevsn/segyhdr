## SEGYHDR. Command line utility for scanning SEG-Y trace headers

Usage: 
```sh
./segyhdr <options> <filename>
-b [ --binary ]       print out file's binary header
-t [ --traces ] arg   specific traces mode. Numbers of traces comma separated. You can specify ranges and step like 10:100:5
-u [ --unique]        print out unique collocations of specified headers based on all traces, for example, source X and Y coordinates.
-s [ --stats ]        all traces statistics mode
-h [ --headers ] arg  headers to scan. Format: <B1>:<F1>,<B2>:<F2>,... where 
                        <Bn> is starting byte,    <Fn> is number format. 
                        0-int16,1-int32,2-IEEE Float,3-IBM Float
```

## How to build

```sh
git clone https://github.com/sergeevsn/segyhdr.git
cd segyhdr
mkdir build
cd build
cmake ..
```

### Linux
```
make
```

### Windows
```
cmake --build .
```
