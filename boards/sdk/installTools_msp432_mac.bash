mkdir tools
pushd tools

curl -O -L http://software-dl.ti.com/simplelink/esd/simplelink_msp432e4_sdk/2.10.00.17/simplelink_msp432e4_sdk_2_10_00_17.app.zip
unzip simplelink_msp432e4_sdk_2_10_00_17.app.zip
open simplelink_msp432e4_sdk_2_10_00_17.app --args --mode unattended --prefix `pwd` 

curl -O -L http://software-dl.ti.com/simplelink/esd/simplelink_msp432_sdk/2.10.00.14/simplelink_msp432p4_sdk_2_10_00_14.app.zip
unzip simplelink_msp432p4_sdk_2_10_00_14.app.zip
open simplelink_msp432p4_sdk_2_10_00_14.app --args --mode unattended --prefix `pwd`

curl -O -L https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-mac.tar.bz2
tar -xf gcc-arm-none-eabi-7-2017-q4-major-mac.tar.bz2

popd

ln -s `pwd`/tools/simplelink_msp432e4_sdk_2_10_00_17
ln -s `pwd`/tools/simplelink_msp432p4_sdk_2_10_00_14
ln -s `pwd`/tools/xdctools_3_50_05_12_core
ln -s `pwd`/tools/gcc-arm-none-eabi-7-2017-q4-major

./makedefs > defs.mk	