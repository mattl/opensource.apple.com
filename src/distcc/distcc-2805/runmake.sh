#!/bin/sh
if [ "${ACTION}" != 'clean' -a "${ACTION}" != 'installhdrs' ]; then
    # Build the i386 side
	BUILD_ROOT="${OBJROOT}/${CONFIGURATION}/${PROJECT_NAME}.build/i386/"
    mkdir -p "${BUILD_ROOT}"
	cd "${BUILD_ROOT}"
    export WHICHPYTHON=python`sed -n '/^DEFAULT = /s///p' /usr/local/versioner/python/versions`
    EXTRAS=`${WHICHPYTHON} -c "import sys, os;print(os.path.join(sys.prefix, 'Extras'))"`
    EXTRASLIBPYTHON="${EXTRAS}/lib/python"
    CFLAGS="-gdwarf-2 -arch i386" "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    export ARCHFLAGS='-arch i386'
    export PYTHON_SETUP_HOME="${EXTRAS}"
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
        mv "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/distcc_pump_c_extensions.so" "${TEMP_FILE_DIR}/i386/distcc_pump_c_extensions.so"
    fi
    
    # Clean and make the x86_64 side
	BUILD_ROOT="${OBJROOT}/${CONFIGURATION}/${PROJECT_NAME}.build/x86_64/"
	mkdir -p "${BUILD_ROOT}"
    cd "${BUILD_ROOT}"
    CFLAGS="-gdwarf-2 -arch x86_64" "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    export ARCHFLAGS='-arch x86_64'
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
      mkdir -p "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/"
      lipo -arch i386 "${TEMP_FILE_DIR}/i386/distcc_pump_c_extensions.so" -arch x86_64 "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/distcc_pump_c_extensions.so" -create -output "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/distcc_pump_c_extensions.so" || exit 1;
      strip -S "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/distcc_pump_c_extensions.so"
    fi

    # This resolves verification errors with buildit
    if [ "${ACTION}" = 'install' ]; then
      echo " " >  "${INSTALL_ROOT}${EXTRASLIBPYTHON}/include_server/__init__.py"
    fi

elif [ "${ACTION}" == 'clean' ]; then
    "${SRCROOT}/distcc_dist/configure" --prefix=/usr
    make clean
fi
