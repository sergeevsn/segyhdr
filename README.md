## SEGYHDR. Command line utility for scanning SEG-Y trace headers
Takes 4 points of the grid with linear coordinates, geographic coordinates and inlines/crosslines.
Interpolates onto new linear coordinates with desired step.
Uses simple linear 4-point interpolation

Usage: 
```sh
./segyhdr <options> <filename>
-b [ --binary ]       print file's binary header
-t [ --traces ] arg   specific traces mode. Numbers of traces comma separated. You can specify ranges and step like 10:100:5
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
make
```

