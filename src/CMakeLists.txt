

include_directories(${CMAKE_BINARY_DIR} ${MATH_INCLUDE_DIR}
                    ${SDL_INCLUDE_DIR} ${SDLGFX_INCLUDE_DIR})


add_executable (ballerburg baller1.c baller2.c ballergui.c cannoneer.c
			screen.c psg.c dlgAlert.c sdlgui.c market.c
			music.c paths.c settings.c sdlgfx.c)

if(CMAKE_COMPILER_IS_MINGW)
  # dynamic build
  #target_link_libraries(ballerburg ${INTL_LIBRARY})

  # static build
  set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static")
  target_link_libraries(ballerburg ${CMAKE_FIND_ROOT_PATH}/lib/libintl.a
                                   ${CMAKE_FIND_ROOT_PATH}/lib/libiconv.a)
endif()

target_link_libraries(ballerburg ${SDL_LIBRARY} ${MATH_LIBRARY})

if(SDLMAIN_LIBRARY)
	target_link_libraries(ballerburg ${SDLMAIN_LIBRARY})
endif(SDLMAIN_LIBRARY)

install(TARGETS ballerburg RUNTIME DESTINATION ${BINDIR})
install(FILES baller.dat baller.mus DESTINATION ${DATADIR})
