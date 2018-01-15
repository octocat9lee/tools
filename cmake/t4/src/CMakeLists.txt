FIND_PATH(myHeader hello.h)
IF(myHeader)
MESSAGE(STATUS "header dir: " ${myHeader})
INCLUDE_DIRECTORIES(${myHeader})
ENDIF(myHeader)

#INCLUDE_DIRECTORIES(/tmp/cmake/t3/include/hello)

LINK_DIRECTORIES(/tmp/cmake/t3/lib)
ADD_EXECUTABLE(main main.c)

TARGET_LINK_LIBRARIES(main libhello.so)

