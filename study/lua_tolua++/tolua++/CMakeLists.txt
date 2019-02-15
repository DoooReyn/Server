# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( tolua++ C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )

find_package ( Lua REQUIRED )
include_directories ( include src/lib ${LUA_INCLUDE_DIR} )

# Build lib
file ( GLOB SRC_LIBtolua++ src/lib/*.c )
if ( MSVC )
  set ( DEF_FILE libtolua++.def )
endif ( )

add_library ( tolua++_lib STATIC ${SRC_LIBtolua++} ${DEF_FILE} )
target_link_libraries ( tolua++_lib ${LUA_LIBRARIES} )
set_target_properties ( tolua++_lib PROPERTIES OUTPUT_NAME tolua++ CLEAN_DIRECT_OUTPUT
  1 )

# Build app
include_directories ( src/bin )
set ( SRC_TOLUA src/bin/tolua.c src/bin/toluabind_default.c )
add_executable ( tolua++ ${SRC_TOLUA} )
target_link_libraries ( tolua++ tolua++_lib ${LUA_LIBRARIES} dl)

# Install
install_library ( tolua++_lib )
install_executable ( tolua++ )
install_header ( include/ )
install_data ( README )
install_doc ( doc/ )
