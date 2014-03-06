call "%VS100COMNTOOLS%..\..\VC\bin\vcvars32.bat"


: boost 1.55.0
7za x -o"%cd%\tmp" "%cd%\tmp\boost_1_55_0.7z"

: bzip2 1.0.6
7za x -o"%cd%\tmp" "%cd%\tmp\bzip2-1.0.6.tar.gz"
7za x -o"%cd%\tmp" "%cd%\tmp\bzip2-1.0.6.tar"
set BZIP2_SOURCE="%cd%\tmp\bzip2-1.0.6"

: icu 51.2
7za x -o"%cd%\tmp" "%cd%\tmp\icu4c-51_2-src.tgz"
7za x -o"%cd%\tmp" "%cd%\tmp\icu4c-51_2-src.tar"
set ICU_PATH="%cd%\tmp\icu"

: python 2.7.6
7za x -o"%cd%\tmp" "%cd%\tmp\Python-2.7.6.tgz"
7za x -o"%cd%\tmp" "%cd%\tmp\Python-2.7.6.tar"
set PYTHON_ROOT="%cd%\tmp\Python-2.7.6"
set PYTHON_VERSION=2.7

: zlib 1.2.8
7za x -o"%cd%\tmp" "%cd%\tmp\zlib-1.2.8.tar.gz"
7za x -o"%cd%\tmp" "%cd%\tmp\zlib-1.2.8.tar"
set ZLIB_SOURCE="%cd%\tmp\zlib-1.2.8"



: icu
pushd tmp\icu
msbuild /p:Platform=win32 /p:Configuration=Debug source/allinone/allinone.sln
msbuild /p:Platform=win32 /p:Configuration=Release source/allinone/allinone.sln
:msbuild /p:Platform=x64 /p:Configuration=Debug source/allinone/allinone.sln
:msbuild /p:Platform=x64 /p:Configuration=Release source/allinone/allinone.sln
popd



: b2
pushd tmp\boost_1_55_0\tools\build\v2
call bootstrap.bat
b2 install --prefix="..\..\..\..\b2"
popd
set path=%path%;%cd%\tmp\b2\bin;

: compile
pushd tmp\boost_1_55_0
:b2 --prefix=%cd%\..\..\boost_1_55_0 --without-mpi toolset=msvc-10.0 variant=debug,release threading=multi link=static address-model=32 install
b2 --prefix=%cd%\..\..\boost_1_55_0 --without-mpi toolset=msvc-10.0 variant=debug,release threading=multi link=static runtime-link=static address-model=32 install
popd