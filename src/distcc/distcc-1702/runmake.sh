#!/bin/sh
if [ "${ACTION}" != 'clean' -a "${ACTION}" != 'installhdrs' ]; then
    # Build the i386 side
	BUILD_ROOT="${OBJROOT}/${CONFIGURATION}/${PROJECT_NAME}.build/i386/"
    mkdir -p "${BUILD_ROOT}"
	cd "${BUILD_ROOT}"
    CFLAGS="-gdwarf-2 -arch i386" "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    make clean > /dev/null
	make ${ACTION}
    if [ ! -d "${TEMP_FILE_DIR}/i386" ]; then
        mkdir -p "${TEMP_FILE_DIR}/i386"
    fi
    
    # Copy aside the i386 binaries for later lipoing
    for p in distcc distccd distccmon-text lsdistcc ; do
        mv "${BUILD_ROOT}/$p" "${TEMP_FILE_DIR}/i386/$p" || exit 1;
    done
    if [ "${ACTION}" = 'install' ]; then
        mv "${INSTALL_ROOT}/usr/lib/python2.5/site-packages/include_server/distcc_pump_c_extensions.so" "${TEMP_FILE_DIR}/i386/distcc_pump_c_extensions.so"
    fi
    
    # Clean and make the x86_64 side
	BUILD_ROOT="${OBJROOT}/${CONFIGURATION}/${PROJECT_NAME}.build/x86_64/"
	mkdir -p "${BUILD_ROOT}"
    cd "${BUILD_ROOT}"
    CFLAGS="-gdwarf-2 -arch x86_64" "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    make clean > /dev/null
	make ${ACTION}
    
    # lipo the i386 binaries into the build product or install product if installing
    for p in distcc distccd distccmon-text lsdistcc ; do \
      if [ "${ACTION}" = 'install' ]; then
        lipo -arch i386 "${TEMP_FILE_DIR}/i386/$p" -arch x86_64 "${BUILD_ROOT}/$p" -create -output "${INSTALL_ROOT}/usr/bin/$p" || exit 1;
        strip -S "${INSTALL_ROOT}/usr/bin/$p"
      else
        echo TEMP_FILE_DIR = ${TEMP_FILE_DIR}
	  echo BUILD_ROOT = ${BUILD_ROOT}
	  echo TARGET_BUILD_DIR = ${TARGET_BUILD_DIR}
	  lipo -arch i386 "${TEMP_FILE_DIR}/i386/$p" -arch x86_64 "${BUILD_ROOT}/$p" -create -output "${TARGET_BUILD_DIR}/$p" || exit 1;
      fi
    done
    if [ "${ACTION}" = 'install' ]; then
      if [ ! -d "${INSTALL_ROOT}/System/Library/Frameworks/Python.framework/Versions/2.5/lib/python2.5/site-packages/include_server/" ]; then
        mkdir -p "${INSTALL_ROOT}/System/Library/Frameworks/Python.framework/Versions/2.5/lib/python2.5/site-packages/include_server/"
      fi
      lipo -arch i386 "${TEMP_FILE_DIR}/i386/distcc_pump_c_extensions.so" -arch x86_64 "${INSTALL_ROOT}/usr/lib/python2.5/site-packages/include_server/distcc_pump_c_extensions.so" -create -output "${INSTALL_ROOT}/usr/lib/python2.5/site-packages/include_server/distcc_pump_c_extensions.so" || exit 1;
      strip -S "${INSTALL_ROOT}/usr/lib/python2.5/site-packages/include_server/distcc_pump_c_extensions.so"
    fi

    # This resolves verification errors with buildit
    if [ "${ACTION}" = 'install' ]; then
      echo " " >  "${INSTALL_ROOT}/usr/lib/python2.5/site-packages/include_server/__init__.py"
      ditto "${INSTALL_ROOT}/usr/lib/python2.5/site-packages" "${INSTALL_ROOT}/System/Library/Frameworks/Python.framework/Versions/2.5/lib/python2.5/site-packages"
      rm -r "${INSTALL_ROOT}/usr/lib/python2.5/"
    fi

elif [ "${ACTION}" == 'clean' ]; then
    "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    make clean
fi
