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
      -DJCC_USE_ASAN=ON                   \ # enable Address sanitizer
      -GNinja ../ && ninja
```
### Run tests
```
ninja test
```
