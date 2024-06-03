#!/bin/bash
set -e
################## SETUP BEGIN
THREAD_COUNT=$(sysctl hw.ncpu | awk '{print $2}')
XCODE_ROOT=$( xcode-select -print-path )
BOOST_VER=1.85.0

MACOSX_VERSION=12.3
IOS_VERSION=14.0
IOS_SIM_VERSION=14.0
################## SETUP END

BOOST_NAME=boost_${BOOST_VER//./_}
HOME_DIR="$( cd "$( dirname "./" )" >/dev/null 2>&1 && pwd )"

[ ! -d boost ] && mkdir boost

if [ ! -d boost/include ]; then
	if [ ! -d boost/$BOOST_NAME ]; then
		
		if [ ! -f boost/$BOOST_NAME.tar.bz2 ]; then
			echo "### downloading https://boostorg.jfrog.io/artifactory/main/release/$BOOST_VER/source/$BOOST_NAME.tar.bz2 ..."
			curl -L https://boostorg.jfrog.io/artifactory/main/release/$BOOST_VER/source/$BOOST_NAME.tar.bz2 -o boost/$BOOST_NAME.tar.bz2
		fi
		echo "### extracting $BOOST_NAME.tar.bz2 ..."
		tar -xf boost/$BOOST_NAME.tar.bz2 -C boost
	fi
	mkdir boost/include
	cp -rf boost/$BOOST_NAME/boost boost/include
fi

generic_build()
{
	BUILD_DIR=$HOME_DIR/build/build.$1.${2//;/_}
	if [ ! -d $BUILD_DIR ]; then
		echo "### building $1 $2 in $BUILD_DIR"
		
		mkdir -p $BUILD_DIR
		pushd $BUILD_DIR

		echo "### running configure in $BUILD_DIR"
		cmake $4 -DCMAKE_OSX_ARCHITECTURES=$2 -DBUILD_SHARED_LIBS=OFF -DCXX_FLAGS=$5 -GXcode ../../
		
		echo "### running build"

		cmake --build . --config Release --target fuego_gouct -- $3 -j $THREAD_COUNT

		libtool -static -o libfuego.a \
			gouct/Release*/libfuego_gouct.a \
			go/Release*/libfuego_go.a \
			smartgame/Release*/libfuego_smartgame.a \
			gtpengine/Release*/libfuego_gtpengine.a

		popd
	fi
}

if true; then
[ -d build ] && rm -rf build

HEADERS="-headers $HOME_DIR/gouct/FuegoEngine.hpp"
LIBRARIES=""

if true; then
	generic_build xros-simulator "arm64;x86_64" "-sdk xrsimulator" "-DCMAKE_SYSTEM_NAME=iOS -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO" "-fembed-bitcode"
	LIBRARIES="-library $HOME_DIR/build/build.xros-simulator.arm64_x86_64/libfuego.a $HEADERS"
fi

if true; then
	generic_build xros arm64 "-sdk xros" "-DCMAKE_SYSTEM_NAME=visionOS" "-fembed-bitcode"
	LIBRARIES="$LIBRARIES -library $HOME_DIR/build/build.xros.arm64/libfuego.a $HEADERS"
fi

if false; then
	generic_build ios arm64 "-sdk iphoneos" "-DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=$IOS_VERSION" "-fembed-bitcode"
	LIBRARIES="$LIBRARIES -library $HOME_DIR/build/build.ios.arm64/libfuego.a $HEADERS"
fi

if false; then
	generic_build simulator "arm64;x86_64" "-sdk iphonesimulator" "-DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=$IOS_SIM_VERSION" "-fembed-bitcode"
	LIBRARIES="$LIBRARIES -library $HOME_DIR/build/build.simulator.arm64_x86_64/libfuego.a $HEADERS"
fi

if false; then
	generic_build osx "arm64;x86_64" "" "-DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO -DCMAKE_IOS_INSTALL_COMBINED=YES -DCMAKE_OSX_DEPLOYMENT_TARGET=$MACOSX_VERSION" "-mmacosx-version-min=$MACOSX_VERSION"
	LIBRARIES="$LIBRARIES -library $HOME_DIR/build/build.osx.arm64_x86_64/libfuego.a $HEADERS"
fi

fi

[ -d $HOME_DIR/build/fuego.xcframework ] && rm -rf $HOME_DIR/build/fuego.xcframework

xcodebuild -create-xcframework $LIBRARIES -output "$HOME_DIR/build/fuego.xcframework"
