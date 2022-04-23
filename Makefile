CORE_INCLUDE_PATH = -IC:/Unix/glfw-3.3.4/include/ -IC:/Unix/glew-2.1.0/include/ -IC:/Unix/glm/

INCLUDE_PATH = -IC:/Unix/glfw-3.3.4/include/ -IC:/Unix/glew-2.1.0/include/ -IC:/Unix/glm/ -IC:/Unix/boost_1_78_0/
LIB_PATH = -LC:/Unix/glfw-3.3.4/build/src -LC:/Unix/glew-2.1.0/lib/Release/x64/
LIBS = -lglfw3 -lglu32 -lglew32 -lopengl32 -lgdi32
BHV_SRCS = behavior/bhv_projectile.cpp behavior/bhv_poofw.cpp behavior/bhv_poof.cpp behavior/bhv_puffw.cpp behavior/bhv_puff.cpp behavior/bhv_whiff.cpp behavior/bhv_player.cpp behavior/bhv_tile.cpp
SRCS = main.cpp ${BHV_SRCS} behavior/implementations.cpp sprite.cpp physics.cpp input.cpp core.cpp entity.cpp behavior.cpp glutil.cpp util.cpp
OUT = out

#build executable (MinGW requires both glfw and gdi32 libraries linked)
out:
	g++ -Wall ${INCLUDE_PATH} ${LIB_PATH} ${SRCS} ${LIBS} -o ${OUT}

#build and run executable
run: out
	./out