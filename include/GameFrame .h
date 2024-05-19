//
// Created by 钱波 on 24-5-19.
//

#include "GLFW/glfw3.h"
#include "imgUI/imgui_impl_opengl3.h"
#include "imgUI/imgui_impl_glfw.h"

#ifndef OPENGL_GAMEFRAME_H
#define OPENGL_GAMEFRAME_H

#endif //OPENGL_GAMEFRAME_H
namespace GameFrame
{
    //-------------------------glfw window creation-------------------------------------
    GLFWwindow* Create_glfw_Window()
    {
        GLFWwindow* window = glfwCreateWindow(1200, 800, "OpenGLTest", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            //return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetScrollCallback(window, InputManager::scroll_callback);
        glfwSetCursorPosCallback(window, InputManager::mouse_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return window;
    }
    //-------------------------imgui creation-------------------------------------
    void RenderMainImGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Edit");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("Use 'Left Alter' to focus on window");
            //自定义GUI内容
            ImGui::End();
        }
        ImGui::Render();
    }

}
