#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <glm/gtc/random.hpp>
#include <chrono>

#include "core.hpp"
#include "input.hpp"

#include "behavior/implementations.hpp"

//glfw error handler
void glfw_err_handler(int code, const char* desc) {
    std::cout 
        << "ERR: glfw_err_handler(): got error code " 
        << code
        << ", description: " 
        << desc
        << std::endl;
    exit(0);
}

void collision(Core &core) {
    //0: tiles, 1: effects, 2: players, 3: enemies
    std::vector<int> group2;
    std::vector<int> group3;

    //populate vectors with current members of groups
    for (int i = 0; i < core.maxid(); i++) {
        auto bhv = core.getbhv(i);
        if (bhv != nullptr) {
            switch (bhv->type()) {
                case 2:
                    group2.push_back(i);
                    break;
                case 3:
                    group3.push_back(i);
                    break;
                default:
                    break;
            }
        }
    }

    //check for collision for each member of group2
    for (unsigned i = 0; i < group2.size(); i++) {
        for (unsigned j = 0; j < group3.size(); j++) {
            Physics* const A = dynamic_cast<Physics* const>(core.getbhv(group2[i]));
            Physics* const B = dynamic_cast<Physics* const>(core.getbhv(group3[j]));
            if (Physics::detect_collision(A, B)) {
                //call collision handlers of both entities
                A->collision(group3[j]);
                B->collision(group2[i]);
            }
        }
    }
}

int main() {
    //--- system setup ---
    //set GLFW error callback, initialize GLFW, and enable debug output on context
    glfwSetErrorCallback(glfw_err_handler);
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    //create window
    int windowwidth = 768;
    int windowheight = 768;
    GLFWwindow* win_h = glfwCreateWindow(windowwidth, windowheight, "title", NULL, NULL);
    glfwMakeContextCurrent(win_h);

    //create input
    Input input(win_h);
    input.setsticky(true);

    //create core
    Core::init();
    Core core;
    core.settexarray(256, 256, 5);
    core.settexture(Image("atlas_tile.png"), 0, 0, 0);
    core.settexture(Image("atlas_player.png"), 0, 0, 1);
    core.settexture(Image("atlas_enemies.png"), 0, 0, 2);
    core.settexture(Image("atlas_effects.png"), 0, 0, 3);
    core.settexture(Image("atlas_projectiles.png"), 0, 0, 4);

    //pixel scale: win.x/ortho.x, win.y/ortho.y (set to 2 in this case. for non-integer scales, this causes tearing)
    float orthowidth = (float)windowwidth / 2.0f;
    float orthoheight = (float)windowheight / 2.0f;
    float orthodepth = 16.0f;
    core.setview(glm::lookAt(glm::vec3(orthowidth, 0.0f, -1.0f), glm::vec3(orthowidth, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    core.setproj(glm::ortho(orthowidth, 0.0f, 0.0f, orthoheight, 0.0f, orthodepth));

    //add allocators of behaviors to core
    Implementations::loadall(core);
    //--- --- ---

    //--- generate map ---
    int tilesize = 16;
    int mapxoffset = 3;
    int mapyoffset = 3;
    int mapwidth = 18;
    int mapheight = 18;

    int id, texoffset;
    for (int i = mapxoffset; i < mapxoffset + mapwidth; i++) {
        for (int j = mapyoffset - 1; j < mapyoffset + mapheight; j++) {
            //push tile and set position
            id = core.push("Tile");
            core.gettransf(id)->pos = glm::vec3(float((tilesize / 2) + (i * tilesize)), float((tilesize / 2) + (j * tilesize)), 0.0f);

            //set texture based on position
            texoffset = 16 * int((j % 2 == 0) ^ ((i % 2 == 0) ^ (j == mapyoffset - 1))) + ((j == mapyoffset - 1) ? 32 : 0);
            core.get(id)->tc.frame.set(texoffset, 0, 0, tilesize - 1, tilesize - 1);
        }
    }
    //--- --- ---

    int id_player = core.push("Player");
    core.gettransf(id_player)->pos = glm::vec3(192.0f, 192.0f, 0.0f);

    core.write2fv("inputdir", glm::vec2(0.0f, 0.0f));
    core.write2fv("mousepos", glm::vec2(0.0f, 0.0f));
    core.writei("m1", 0);
    core.writei("m2", 0);
    core.writei("playerid", id_player);
    core.writef("time", 0.0f);

    core.gettransf(core.push("Whiff"))->pos = glm::vec3(0.0f, 384.0f, 0.0f);
    core.gettransf(core.push("Whiff"))->pos = glm::vec3(384.0f, 384.0f, 0.0f);
    core.gettransf(core.push("Whiff"))->pos = glm::vec3(0.0f, 0.0f, 0.0f);
    core.gettransf(core.push("Whiff"))->pos = glm::vec3(384.0f, 0.0f, 0.0f);

    srand(time(0));

    glfwSwapInterval(1);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> time;
    while (!glfwWindowShouldClose(win_h)) {
        time = std::chrono::high_resolution_clock::now() - start;
        glfwPollEvents();
        input.update();

        core.write2fv("inputdir", input.inputdir());
        core.write2fv("mousepos", glm::vec2(input.mousepos().x / 2.0f, (768.0f - input.mousepos().y) / 2.0f));
        core.writei("m1", input.get_m1());
        core.writei("m2", input.get_m2());
        core.writef("time", time.count());

        //queue all entities and execute them
        core.queueall();
        core.exec();
        //perform collision detection
        collision(core);
        core.exec();

        core.draw();
        
        glfwSwapBuffers(win_h);
    };

    //terminate GLFW
    glfwDestroyWindow(win_h);
    glfwTerminate();

    return 0;
}

/*
        if (x == 1.0f) {
            float *transform_mem = (float*)core._transform_buf.copy_mem();
            for (int i = 0; i < 16 * 4; i++) {
                if (i % 4 == 0)
                    std::cout << std::endl;
                if (i % 16 == 0)
                    std::cout << std::endl;
                std::cout << transform_mem[i] << "\t";
            }
            delete transform_mem;
        }
*/