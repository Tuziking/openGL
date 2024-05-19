#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#include "imgUI/imgui.h"
#include "imgUI/imgui_impl_glfw.h"
#include "imgUI/imgui_impl_opengl3.h"
#include "UI/GUI.h"
#include "imgUI/imgui_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 窗口的宽和高
const unsigned int SCR_WIDTH = 2000;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// GUI Menu
float scale = 0.0f; // 用于存储滑动条的值

// FBO and texture
unsigned int fbo, fboTexture;

int main()
{

    const char* glsl_version = "#version 330";

// 初始化glfw的窗口
#pragma region glfw init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// 判断是否是苹果电脑，如果是的话，需要加上下面这行代码
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 基于控制权
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#pragma endregion

// 初始化Shader 和 model
#pragma region shader and model
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    Shader ourShader("../Shaders/1.model_loading.vs", "../Shaders/1.model_loading.fs");
    Model ourModel("../resources/nanosuit/nanosuit.obj");
#pragma endregion

// 导入纹理
#pragma region texture

    // 创建 FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建 FBO 纹理
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    // 创建 Renderbuffer 对象用于深度和模板测试
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

// 初始化ImGui
#pragma region ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);
    // 获取 io, 设置ImGui的内容
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("../resources/fonts/kaiu.ttf", 24, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    //允許停靠
    io.ConfigFlags  |= ImGuiConfigFlags_DockingEnable;
    //視口設置無裝飾
    io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
    //允許視口停靠
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //停靠於背景
    io.ConfigFlags |= ImGuiCol_DockingEmptyBg;


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.36f, 0.36f, 102.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 171.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 255.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.45f, 0.45f, 0.45f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.55f, 0.55f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.95f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
    colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.40f, 0.13f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 1.00f, 0.85f, 0.35f);

    style.WindowRounding = 4;
    style.FrameRounding = 4;
    style.ChildRounding = 3;
    style.ScrollbarRounding = 7;
    style.GrabRounding = 12;
    style.TabRounding = 8;
    style.PopupRounding = 6;
//    float f = 0.0f;
#pragma endregion
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

// 循环渲染程序
#pragma region loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);


        /********************* 绘制imgUI的内容 **********************/
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        ImGui::ShowDemoWindow();
        DrawGUI();

        // 渲染3D界面的视图
        ImGui::Begin("Scene");
        ImGui::Image((void*)(intptr_t)fboTexture, ImVec2(SCR_WIDTH, SCR_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        // debug的界面 显示用户当前位置
        DrawDebugUI(camera.Position);
        // 菜单界面
        DrawMenuUI(camera, scale, clear_color);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window){
    // 获取当前活动的ImGui窗口名称
    ImGuiWindow* currentWindow = ImGui::GetCurrentContext()->NavWindow;
    if (currentWindow && strcmp(currentWindow->Name, "Scene") == 0) {
        if(ImGui::IsKeyPressed(ImGuiKey_Escape)){
            glfwSetWindowShouldClose(window, true);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (ImGui::IsKeyPressed(ImGuiKey_S))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (ImGui::IsKeyPressed(ImGuiKey_A))
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (ImGui::IsKeyPressed(ImGuiKey_D))
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            camera.ProcessMouseMovement(mouseDelta.x, -mouseDelta.y);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }

        float mouseWheel = ImGui::GetIO().MouseWheel;
        if (mouseWheel != 0)
            camera.ProcessMouseScroll(mouseWheel);
    }
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
