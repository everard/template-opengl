// Copyright Nezametdinov E. Ildus 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
#include <SDL.h>
#include <GL/glew.h>

#include <chrono>
#include <memory>
#include <random>
#include <thread>
#include <iostream>

import rose.opengl.kernel;

namespace rose {

////////////////////////////////////////////////////////////////////////////////
// Window definition.
////////////////////////////////////////////////////////////////////////////////

namespace detail {

using window_deleter = decltype([](SDL_Window* window) {
    if(window != nullptr) {
        std::cout << "Deleting SDL window\n";
        SDL_DestroyWindow(window);
    }
});

} // namespace detail

using window = std::unique_ptr<SDL_Window, detail::window_deleter>;

////////////////////////////////////////////////////////////////////////////////
// OpenGL context definition.
////////////////////////////////////////////////////////////////////////////////

namespace detail {

using opengl_context_deleter = decltype([](SDL_GLContext context) {
    if(context != nullptr) {
        std::cout << "Deleting OpenGL context\n";
        SDL_GL_DeleteContext(context);
    }
});

} // namespace detail

using opengl_context = std::unique_ptr<void, detail::opengl_context_deleter>;

} // namespace rose

////////////////////////////////////////////////////////////////////////////////
// Program entry point.
////////////////////////////////////////////////////////////////////////////////

int
main() {
    // Initialize SDL subsystems.
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        return EXIT_FAILURE;
    }

    // Make sure SDL state is cleaned-up upon exit.
    struct guard {
        ~guard() {
            std::cout << "Cleaning-up SDL state\n";
            SDL_Quit();
        }
    } _;

    // Create a new window.
    auto window = rose::window{SDL_CreateWindow(
        "Main", 0, 0, 1280, 720, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL)};

    if(!window) {
        return EXIT_FAILURE;
    }

    // Specify OpenGL context parameters.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create OpenGL context.
    auto context = rose::opengl_context{SDL_GL_CreateContext(window.get())};
    if(!context) {
        return EXIT_FAILURE;
    }

    // Make the context current.
    SDL_GL_MakeCurrent(window.get(), context.get());

    // Initialize OpenGL bindings.
    if(glewInit() != GLEW_OK) {
        return EXIT_FAILURE;
    }

    // Set OpenGL state.
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glEnableVertexAttribArray(0);

    // Run event loop.
    for(bool should_run = true; should_run;) {
        // Get current frame time.
        auto t0 = std::chrono::high_resolution_clock::now();

        // Process events.
        for(SDL_Event event; SDL_PollEvent(&event) != 0;) {
            switch(event.type) {
                case SDL_QUIT:
                    should_run = false;
                    break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_q:
                            should_run = false;
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        // Process mouse button event.
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT) {
                        // Process mouse button event.
                    }

                    break;

                case SDL_MOUSEMOTION:
                    // Process mouse motion event.
                    break;

                default:
                    break;
            }
        }

        // Render the next frame.
        if(auto w = 0, h = 0; true) {
            if(SDL_GetWindowSize(window.get(), &w, &h); (w != 0) && (h != 0)) {
                // Set viewport.
                glViewport(0, 0, w, h);
                glClear(GL_COLOR_BUFFER_BIT);

                // Show the frame.
                SDL_GL_SwapWindow(window.get());
            }
        }

        // Sleep between frames to reduce CPU load.
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - t0);

        if(delta.count() < 16) {
            auto dt = 16 - delta.count();
            std::this_thread::sleep_for(std::chrono::milliseconds{dt});
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds{2});
        }
    }

    return EXIT_SUCCESS;
}
