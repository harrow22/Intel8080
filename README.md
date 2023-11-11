# Intel8080 Emulator
Project to develop as close to cycle-accurate of an Intel 8080 emulator as I can. It passes the full test suite found [here](https://altairclone.com/downloads/cpu_tests/). 

I created this for use with my [Space Invaders emulator](https://github.com/harrow22/SpaceInvaders).

## Usage
### Dependencies
* **A c++ compiler**
* **CMake version 3.26+**
* **CMake build generator**

### Adding to project
Using CMake's `FetchContent`, add this to your CMakeLists.txt file:
```
include(FetchContent)
FetchContent_Declare(
        Intel8080
        GIT_REPOSITORY "https://github.com/harrow22/Intel8080.git"
)
set(INTEL8080_TESTS OFF)
FetchContent_MakeAvailable(Intel8080)
```
Then you can just link it to your executable using `target_link_libraries`.

### Documentation
See [header file](include/Intel8080.h)

## Running Tests
### With CMake
```
git clone https://github.com/harrow22/Intel8080.git
cd Intel8080/
cmake -S . -B build -G your_generator -DCMAKE_BUILD_TYPE=RELEASE
cmake --build build --config Release
build/tests/Intel8080_test.exe <ARGUMENTS>...
```

By default, it will write output to stdout. Running it on my i5-8250U on Windows 11 produces this,
```
*** TEST: TST8080.COM
MICROCOSM ASSOCIATES 8080/8085 CPU DIAGNOSTIC
 VERSION 1.0  (C) 1980

 CPU IS OPERATIONAL
*** 651 instructions executed on 4924 cycles (expected=4924, diff=0) in 6 ms

*** TEST: 8080PRE.COM
8080 Preliminary tests complete
*** 1061 instructions executed on 7817 cycles (expected=7817, diff=0) in 1 ms

*** TEST: CPUTEST.COM

DIAGNOSTICS II V1.2 - CPU TEST
COPYRIGHT (C) 1981 - SUPERSOFT ASSOCIATES

ABCDEFGHIJKLMNOPQRSTUVWXYZ
CPU IS 8080/8085
BEGIN TIMING TEST
END TIMING TEST
CPU TESTS OK

*** 33971311 instructions executed on 255653383 cycles (expected=255653383, diff=0) in 2.820 sec

*** TEST: 8080EXM.COM
8080 instruction exerciser
dad <b,d,h,sp>................  PASS! crc is:14474ba6
aluop nn......................  PASS! crc is:9e922f9e
aluop <b,c,d,e,h,l,m,a>.......  PASS! crc is:cf762c86
<daa,cma,stc,cmc>.............  PASS! crc is:bb3f030c
<inr_,dcr_> a...................  PASS! crc is:adb6460e
<inr_,dcr_> b...................  PASS! crc is:83ed1345
<inx,dcx> b...................  PASS! crc is:f79287cd
<inr_,dcr_> c...................  PASS! crc is:e5f6721b
<inr_,dcr_> d...................  PASS! crc is:15b5579a
<inx,dcx> d...................  PASS! crc is:7f4e2501
<inr_,dcr_> e...................  PASS! crc is:cf2ab396
<inr_,dcr_> h...................  PASS! crc is:12b2952c
<inx,dcx> h...................  PASS! crc is:9f2b23c0
<inr_,dcr_> l...................  PASS! crc is:ff57d356
<inr_,dcr_> m...................  PASS! crc is:92e963bd
<inx,dcx> sp..................  PASS! crc is:d5702fab
lhld nnnn.....................  PASS! crc is:a9c3d5cb
shld nnnn.....................  PASS! crc is:e8864f26
lxi <b,d,h,sp>,nnnn...........  PASS! crc is:fcf46e12
ldax <b,d>....................  PASS! crc is:2b821d5f
mvi <b,c,d,e,h,l,m,a>,nn......  PASS! crc is:eaa72044
mov <bcdehla>,<bcdehla>.......  PASS! crc is:10b58cee
sta nnnn / lda nnnn...........  PASS! crc is:ed57af72
<rlc,rrc,ral,rar>.............  PASS! crc is:e0d89235
stax <b,d>....................  PASS! crc is:2b0471e9
Tests complete
*** 2919050698 instructions executed on 23803381171 cycles (expected=23803381171, diff=0) in 3.33 min

Done. Total time elapsed 3.35 min
```
which, admittedly, is pretty slow compared to some, but I wasn't necessarily developing this for speed.

### Command line arguments
* `-debug` - the processor state will be output during M1 cycle of each instruction
* `-v` - (requires `-debug`) the state of the processor's bus lines will be output during every read/write cycle
> [!WARNING] 
> If you redirect stdout to a file with `-debug` enabled, it will output *many* GBs of data

## Thanks
* [Intel 8080 user manual](http://bitsavers.trailing-edge.com/components/intel/MCS80/98-153B_Intel_8080_Microcomputer_Systems_Users_Manual_197509.pdf) (ch. 2-4)
* [floooh's blog post](https://floooh.github.io/2021/12/17/cycle-stepped-z80.html) for the goto label idea
* [superzazu's 8080](https://github.com/superzazu/8080/tree/master) for comparable log files
