# JCC: Jun's C compiler from scractch
```
    ___  ________  ________     
   |\  \|\   ____\|\   ____\    
   \ \  \ \  \___|\ \  \___|    
 __ \ \  \ \  \    \ \  \       
|\  \\_\  \ \  \____\ \  \____  
\ \________\ \_______\ \_______\
 \|________|\|_______|\|_______|
```
**Please note this is just my little toy and could be abandoned any time**

## Build instruction

**Instructions below assume you use Ubtuntu22**

### Prerequisites
```
sudo apt install cmake clang ninja-build -y
pip3 install --user conan
```
### Build
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug            \
      -DCMAKE_C_COMPILER=clang            \
      -DCMAKE_CXX_COMPILER=clang++        \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=YES \
      -DJCC_USE_ASAN=ON                   \
      -GNinja ../ && ninja
```
`-DJCC_USE_ASAN=ON` means enable the address sanitizer, it might be helpful to catch some memory bugs.
### Run tests
```
ninja test
```

### Usage
> Note JCC is still in the very early stage, so don't expected it can handle everything correctly :)

- Dump the AST tree of the program.
```
./jcc test.c --ast-dump # Note you can only pass the flag in the end!
```
- Generate the assembly.
```
./jcc test.c # It will create test.s.
```
