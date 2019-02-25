# This file is NOT licensed under the GPLv3, which is the license for the rest
# of YouCompleteMe.
#
# Here's the license text for this file:
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

from distutils.sysconfig import get_python_inc
import os
import ycm_core

DIR_OF_THIS_SCRIPT = os.path.abspath( os.path.dirname( __file__ ) )
SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
  '-Wall',
  '-Werror=all',
  '-Wextra',
  '-Wno-error=deprecated-declarations',
  '-Wno-error=unused-but-set-variable',
  '-Wno-error=unused-function',
  '-Wno-error=unused-variable',
  '-Wno-unused-parameter',
  '-Wno-sign-compare',
  '-Wno-old-style-declaration',
  '-DESP_PLATFORM',
  '-DGCC_NOT_5_2_0=0',
  '-DHAVE_CONFIG_H',
  '-DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"',
  '-DUNITY_INCLUDE_CONFIG_H',
  '-DWITH_POSIX',
  #  '-std=gnu99',
  '-std=c++11',
  '-x',
  'c++',
  '-I/Users/volkhin/esp/esp-idf/components',
  '-I.',
  '-mlongcalls',
  '-mlongcalls',
  '-Og',
  '-ffunction-sections',
  '-fdata-sections',
  '-fstrict-volatile-bitfields',
  '-ggdb',
]

includes = [
    '-I/Users/volkhin/esp/volkhin_nixie_timer/main',
    '-I/Users/volkhin/esp/volkhin_nixie_timer/build/config',
    '-I/Users/volkhin/esp/esp-idf/components/app_trace/include',
    '-I/Users/volkhin/esp/esp-idf/components/app_update/include',
    '-I/Users/volkhin/esp/esp-idf/components/spi_flash/include',
    '-I/Users/volkhin/esp/esp-idf/components/bootloader_support/include',
    '-I/Users/volkhin/esp/esp-idf/components/asio/asio/asio/include',
    '-I/Users/volkhin/esp/esp-idf/components/asio/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/lwip/include/apps',
    '-I/Users/volkhin/esp/esp-idf/components/lwip/lwip/src/include',
    '-I/Users/volkhin/esp/esp-idf/components/lwip/port/esp32/include',
    '-I/Users/volkhin/esp/esp-idf/components/lwip/port/esp32/include/arch',
    '-I/Users/volkhin/esp/esp-idf/components/lwip/include_compat',
    '-I/Users/volkhin/esp/esp-idf/components/vfs/include',
    '-I/Users/volkhin/esp/esp-idf/components/bt/include',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/bluedroid/decoder/include',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/bluedroid/encoder/include',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/hxcmod-player',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/hxcmod-player/mods',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/md5',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/3rd-party/yxml',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/src/classic',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/src/ble/gatt-service',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/src/ble',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/src',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/platform/embedded',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/platform/freertos',
    '-I/Users/volkhin/esp/esp-idf/components/btstack/include',
    '-I/Users/volkhin/esp/esp-idf/components/micro-ecc/micro-ecc',
    '-I/Users/volkhin/esp/esp-idf/components/nvs_flash/include',
    '-I/Users/volkhin/esp/esp-idf/components/mbedtls/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/mbedtls/mbedtls/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp32/include',
    '-I/Users/volkhin/esp/esp-idf/components/driver/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp_ringbuf/include',
    '-I/Users/volkhin/esp/esp-idf/components/tcpip_adapter/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp_event/include',
    '-I/Users/volkhin/esp/esp-idf/components/log/include',
    '-I/Users/volkhin/esp/esp-idf/components/newlib/platform_include',
    '-I/Users/volkhin/esp/esp-idf/components/newlib/include',
    '-I/Users/volkhin/esp/esp-idf/components/freertos/include',
    '-I/Users/volkhin/esp/esp-idf/components/heap/include',
    '-I/Users/volkhin/esp/esp-idf/components/soc/esp32/include',
    '-I/Users/volkhin/esp/esp-idf/components/soc/include',
    '-I/Users/volkhin/esp/esp-idf/components/coap/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/coap/port/include/coap',
    '-I/Users/volkhin/esp/esp-idf/components/coap/libcoap/include',
    '-I/Users/volkhin/esp/esp-idf/components/coap/libcoap/include/coap',
    '-I/Users/volkhin/esp/esp-idf/components/console',
    '-I/Users/volkhin/esp/esp-idf/components/esp-tls',
    '-I/Users/volkhin/esp/esp-idf/components/esp_adc_cal/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp_http_client/include',
    '-I/Users/volkhin/esp/esp-idf/components/nghttp/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/nghttp/nghttp2/lib/includes',
    '-I/Users/volkhin/esp/esp-idf/components/esp_http_server/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp_https_ota/include',
    '-I/Users/volkhin/esp/esp-idf/components/esp_https_server/include',
    '-I/Users/volkhin/esp/esp-idf/components/openssl/include',
    '-I/Users/volkhin/esp/esp-idf/components/ethernet/include',
    '-I/Users/volkhin/esp/esp-idf/components/expat/expat/expat/lib',
    '-I/Users/volkhin/esp/esp-idf/components/expat/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/fatfs/src',
    '-I/Users/volkhin/esp/esp-idf/components/wear_levelling/include',
    '-I/Users/volkhin/esp/esp-idf/components/sdmmc/include',
    '-I/Users/volkhin/esp/esp-idf/components/freemodbus/modbus/include',
    '-I/Users/volkhin/esp/esp-idf/components/freemodbus/modbus_controller',
    '-I/Users/volkhin/esp/esp-idf/components/idf_test/include',
    '-I/Users/volkhin/esp/esp-idf/components/jsmn/include',
    '-I/Users/volkhin/esp/esp-idf/components/json/cJSON',
    '-I/Users/volkhin/esp/esp-idf/components/libsodium/libsodium/src/libsodium/include',
    '-I/Users/volkhin/esp/esp-idf/components/libsodium/port_include',
    '-I/Users/volkhin/esp/esp-idf/components/mdns/include',
    '-I/Users/volkhin/esp/esp-idf/components/mqtt/esp-mqtt/include',
    '-I/Users/volkhin/esp/esp-idf/components/tcp_transport/include',
    '-I/Users/volkhin/esp/esp-idf/components/protobuf-c/protobuf-c',
    '-I/Users/volkhin/esp/esp-idf/components/protocomm/include/common',
    '-I/Users/volkhin/esp/esp-idf/components/protocomm/include/security',
    '-I/Users/volkhin/esp/esp-idf/components/protocomm/include/transports',
    '-I/Users/volkhin/esp/esp-idf/components/pthread/include',
    '-I/Users/volkhin/esp/esp-idf/components/smartconfig_ack/include',
    '-I/Users/volkhin/esp/esp-idf/components/spiffs/include',
    '-I/Users/volkhin/esp/esp-idf/components/ulp/include',
    '-I/Users/volkhin/esp/esp-idf/components/unity/include',
    '-I/Users/volkhin/esp/esp-idf/components/unity/unity/src',
    '-I/Users/volkhin/esp/esp-idf/components/wifi_provisioning/include',
    '-I/Users/volkhin/esp/esp-idf/components/wpa_supplicant/include',
    '-I/Users/volkhin/esp/esp-idf/components/wpa_supplicant/port/include',
    '-I/Users/volkhin/esp/esp-idf/components/xtensa-debug-module/include',
]

flags += includes

old_flags = [
'-Wall',
'-Wextra',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-fexceptions',
'-DNDEBUG',
# THIS IS IMPORTANT! Without the '-x' flag, Clang won't know which language to
# use when compiling headers. So it will guess. Badly. So C++ headers will be
# compiled as C headers. You don't want that so ALWAYS specify the '-x' flag.
# For a C project, you would set this to 'c' instead of 'c++'.
'-x',
'c++',
'-isystem',
'cpp/pybind11',
'-isystem',
'cpp/BoostParts',
'-isystem',
get_python_inc(),
'-isystem',
'cpp/llvm/include',
'-isystem',
'cpp/llvm/tools/clang/include',
'-I',
'cpp/ycm',
'-I',
'cpp/ycm/ClangCompleter',
'-isystem',
'cpp/ycm/tests/gmock/gtest',
'-isystem',
'cpp/ycm/tests/gmock/gtest/include',
'-isystem',
'cpp/ycm/tests/gmock',
'-isystem',
'cpp/ycm/tests/gmock/include',
'-isystem',
'cpp/ycm/benchmarks/benchmark/include',
]

# Clang automatically sets the '-std=' flag to 'c++14' for MSVC 2015 or later,
# which is required for compiling the standard library, and to 'c++11' for older
# versions.
# if platform.system() != 'Windows':
  # flags.append( '-std=c++11' )


# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# You can get CMake to generate this file for you by adding:
#   set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
# to your CMakeLists.txt file.
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = './build'

if os.path.exists( compilation_database_folder ):
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None


def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def FindCorrespondingSourceFile( filename ):
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        return replacement_file
  return filename


def Settings( **kwargs ):
  print("getting settings for", kwargs)
  if kwargs[ 'language' ] == 'cfamily':
    # If the file is a header, try to find the corresponding source file and
    # retrieve its flags from the compilation database if using one. This is
    # necessary since compilation databases don't have entries for header files.
    # In addition, use this source file as the translation unit. This makes it
    # possible to jump from a declaration in the header file to its definition
    # in the corresponding source file.
    filename = FindCorrespondingSourceFile( kwargs[ 'filename' ] )
    defaults = {
        'flags': flags,
        'include_paths_relative_to_dir': DIR_OF_THIS_SCRIPT,
        'override_filename': filename
    }

    if not database:
      print("no compilation database, use predefined flags:", flags)
      return defaults

    compilation_info = database.GetCompilationInfoForFile( filename )
    if not compilation_info.compiler_flags_:
      print(
        "file",
        filename,
        "is missing from compilation database, return defaults",
      )
      #  return {}
      return defaults

    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object.
    final_flags = list( compilation_info.compiler_flags_ )

    print("final flags:", final_flags)

    return {
      'flags': final_flags,
      'include_paths_relative_to_dir': compilation_info.compiler_working_dir_,
      'override_filename': filename
    }
  return {}
