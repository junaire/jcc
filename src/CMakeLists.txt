SET(SOURCES
	ast.cc
	main.cc
	lexer.cc
	parser.cc
)

add_executable(
	jcc
	${SOURCES}
)
target_include_directories(
	jcc
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    jcc
    ${CONAN_LIBS}
)
