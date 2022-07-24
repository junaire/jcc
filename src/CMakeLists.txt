SET(SOURCES
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
