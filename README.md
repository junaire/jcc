# JCC
Jun's own C compiler from scractch.

**Please note this is just my little toy and could be abandoned any time**

## Build instruction

### Build
```
pip3 install --user conan
mkdir build
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
