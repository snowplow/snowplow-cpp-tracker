find_library(LibUUID_LIBRARY
  NAMES uuid
  PATHS /lib /usr/lib /usr/local/lib
)

set(CMAKE_FIND_FRAMEWORK_SAVE ${CMAKE_FIND_FRAMEWORK})
set(CMAKE_FIND_FRAMEWORK NEVER)

find_path(LibUUID_INCLUDE_DIR uuid/uuid.h
/usr/local/include
/usr/include
)

if (LibUUID_LIBRARY AND LibUUID_INCLUDE_DIR)
  set(LibUUID_LIBRARIES ${LibUUID_LIBRARY})
  set(LibUUID_FOUND "YES")
else ()
  set(LibUUID_FOUND "NO")
endif ()

if (LibUUID_FOUND)
   if (NOT LibUUID_FIND_QUIETLY)
      message(STATUS "Found UUID: ${LibUUID_LIBRARIES}")
   endif ()
   add_library(libuuid::libuuid SHARED IMPORTED)
   set_target_properties(libuuid::libuuid PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${LibUUID_INCLUDE_DIR}
      IMPORTED_LOCATION ${LibUUID_LIBRARY}
   )
else ()
   if (LibUUID_FIND_REQUIRED)
      message( "library: ${LibUUID_LIBRARY}" )
      message( "include: ${LibUUID_INCLUDE_DIR}" )
      message(FATAL_ERROR "Could not find UUID library")
   endif ()
endif ()

mark_as_advanced(
  LibUUID_LIBRARY
  LibUUID_INCLUDE_DIR
)
set(CMAKE_FIND_FRAMEWORK ${CMAKE_FIND_FRAMEWORK_SAVE})
