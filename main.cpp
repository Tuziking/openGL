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
#include "Particle.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(vector<std::string> faces);
void loadModel(Shader shader, Model model, glm::mat4 modelMatrix, glm::mat4 view, glm::mat4 projection);
void loadSkybox(Shader shader, unsigned int skyboxVAO, unsigned int cubemapTexture, glm::mat4 view, glm::mat4 projection);
void loadSnow(Shader shader, unsigned int VAO, unsigned int texture, std::vector<Particle>& particles, glm::mat4 view, glm::mat4 projection);
unsigned int loadTexture(char const * path);
void loadLight(Shader& lightShader, glm::mat4 projection, glm::mat4 view, glm::mat4 model, Model model1);

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

// FBO and FBO texture
unsigned int fbo, fboTexture;

// light
unsigned int loadTexture(char const * path);
glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
};



# pragma region particles paraments
// VAO：顶点数组对象
// VBO：顶点缓冲对象
// program：着色器程序
// texture：纹理
unsigned int VAO, VBO, program, texture;
const int PARTICLE_NUM = 1000;
// particles：存储所有粒子
// particlesTmp：用于存储一个时间间隔后还未消亡的粒子和它们的新状态
std::vector<Particle> particles, particlesTmp;
// 每片雪花用三维空间中的一个正方形和其上的纹理表示，正方形分为两个三角形绘制，每行前三个实数表示顶点的坐标，后两个实数表示该点对应的纹理坐标
float vertices[] = {
        -0.01f, -0.01f, -0.01f, 0.0f, 0.0f,
        0.01f, -0.01f, -0.01f, 1.0f, 0.0f,
        0.01f, 0.01f, -0.01f, 1.0f, 1.0f,
        0.01f, 0.01f, -0.01f, 1.0f, 1.0f,
        -0.01f, 0.01f, -0.01f, 0.0f, 1.0f,
        -0.01f, -0.01f, -0.01f, 0.0f, 0.0f
};
# pragma endregion

#pragma region skybox faces
    vector<std::string> faces1
            {
                    "../resources/texture/skybox/px.png",
                    "../resources/texture/skybox/nx.png",
                    "../resources/texture/skybox/py.png",
                    "../resources/texture/skybox/ny.png",
                    "../resources/texture/skybox/pz.png",
                    "../resources/texture/skybox/nz.png",
            };
vector<std::string> faces2
        {
                "../resources/texture/skybox2/xp.jpg",
                "../resources/texture/skybox2/xn.jpg",
                "../resources/texture/skybox2/yp.jpg",
                "../resources/texture/skybox2/yn.jpg",
                "../resources/texture/skybox2/zp.jpg",
                "../resources/texture/skybox2/zn.jpg",
        };
#pragma endregion

struct rending {
    Model model;
    glm::vec3 pos;
    glm::vec3 scale;
};

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
//    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    // Shader ourShader("../Shaders/1.model_loading.vs", "../Shaders/1.model_loading.fs");
    Shader ourShader("../resources/shader/multiple_v.fs", "../resources/shader/multiple_f.fs");
    Shader lightingShader("../resources/shader/multiple_v.fs", "../resources/shader/multiple_f.fs");

    Model ourModel("../resources/nanosuit/nanosuit.obj");
    std::vector<rending> models;
    //chair
    Model chair("../resources/models/chair/chair.obj");
    models.push_back(rending{chair,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f)});
    //tree
    Model tree("../resources/models/tree/tree.obj");
    models.push_back(rending{tree,glm::vec3(3.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f)});
#pragma endregion

// 雪花粒子初始化
# pragma region particles init
// 用于判断着色器的GLSL代码是否编译成功
    int success;
    Shader snowShader("../Shaders/snow_shader.vs", "../Shaders/snow_shader.fs");

    // 生成顶点数组对象和顶点缓冲对象
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定顶点数组对象
    glBindVertexArray(VAO);
    // 绑定顶点缓冲对象，并将顶点信息传入其中
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 按照vertices中数据的定义对顶点数组对象进行设置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // width：纹理文件的宽度
    // height：纹理文件的高度
    // channelNum：纹理文件的颜色通道数
    int width, height, channelNum;
    // 纹理文件的图像数据
    unsigned char *imageDate;
    // 使用stb_image.h库中读取纹理文件的相关属性和图像数据
    imageDate = stbi_load("../resources/texture/snowflower.png", &width, &height, &channelNum, STBI_rgb_alpha);
    // 生成纹理
    glGenTextures(1, &texture);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置纹理的属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 将从文件中读取的纹理数据传入
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageDate);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imageDate);
    // 启用OpenGL的深度检测，使绘制的图形更具真实感
    glEnable(GL_DEPTH_TEST);

    // 初始化粒子系统
    for (unsigned int i = 0; i < PARTICLE_NUM; i++)
        particles.push_back(Particle(glfwGetTime(), true));
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

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
// 初始化天空盒
#pragma region skybox init
Shader skyboxShader("../Shaders/skybox.vs", "../Shaders/skybox.fs");
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // load textures
    // -------------
    unsigned int cubemapTexture = loadCubemap(faces2);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
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
#pragma region render model
        // 1. 绑定自定义帧缓冲区 fbo
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // 渲染光源
        loadLight(lightingShader, projection, view, model, ourModel);

        // 渲染 3D 模型
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        loadModel(ourShader, ourModel, model, view, projection);

        model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        loadModel(ourShader, ourModel, model, view, projection);

        // 渲染3D模型列表
        for(rending& r : models) {
            model = glm::translate(model, r.pos);
            model = glm::scale(model, r.scale);
            loadModel(ourShader, r.model, model, view, projection);
        }

        // 渲染雪花粒子
        loadSnow(snowShader, VAO, texture, particles, view, projection);

        // 渲染天空盒
        // 在最后渲染天空盒
        loadSkybox(skyboxShader, skyboxVAO, cubemapTexture, view, projection);

        // 2. 解除帧缓冲区绑定，返回默认帧缓冲区
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
# pragma endregion

#pragma region ImGui render
        /***************** 渲染 ImGui 界面 ************************/
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        DrawGUI();

        // 绘制3D渲染的界面
        ImGui::Begin("Scene");
        // 将 fbo 的纹理传递给 ImGui 显示
        ImGui::Image((void*)(intptr_t)fboTexture, ImVec2(SCR_WIDTH, SCR_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        // 绘制底部数据展示界面
        DrawDebugUI(camera.Position);
        // 绘制菜单界面
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
# pragma endregion
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
// 程序结束后的清理
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window){
    // 获取当前活动的ImGui窗口名称
    ImGuiWindow* currentWindow = ImGui::GetCurrentContext()->NavWindow;
    if (currentWindow && strcmp(currentWindow->Name, "Scene") == 0) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void loadModel(Shader shader,
               Model model,
               glm::mat4 modelMatrix,
               glm::mat4 view,
               glm::mat4 projection){
    shader.use();
    shader.setMat4("model", modelMatrix);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    model.Draw(shader);
}

void loadSkybox(Shader skyboxShader,
                unsigned int skyboxVAO,
                unsigned int cubemapTexture,
                glm::mat4 view,
                glm::mat4 projection) {
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

void loadSnow(Shader snowShader,
              unsigned int VAO,
              unsigned int texture,
              std::vector<Particle>& particles,
              glm::mat4 view,
              glm::mat4 projection){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // 禁止写入深度缓冲区
        glDepthFunc(GL_ALWAYS); // 设置深度测试函数为 GL_ALWAYS，始终通过深度测试

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
        snowShader.use();
        snowShader.setMat4("view", view);
        snowShader.setMat4("proj", projection);
        particlesTmp.clear();
        for (unsigned int i = 0; i < particles.size(); i++) {
            glm::mat4 model1 = glm::mat4(1.0f);
            particles[i].update(glfwGetTime());
            model1 = glm::translate(model1, particles[i].getX());

            snowShader.setMat4("model", model1);
            if (particles[i].exist())
                particlesTmp.push_back(particles[i]);
            else
                particlesTmp.push_back(Particle(glfwGetTime(), false));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
        particles = particlesTmp;
        // 恢复深度测试函数和深度写入状态
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
}

void loadLight(Shader& lightingShader, glm::mat4 projection, glm::mat4 view, glm::mat4 model, Model model1){
    lightingShader.use();
    lightingShader.setVec3("viewPos", camera.Position);
    lightingShader.setFloat("material.shininess", 32.0f);
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.09f);
    lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.09f);
    lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.09f);
    lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", 0.09f);
    lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight
    lightingShader.setVec3("spotLight.position", camera.Position);
    lightingShader.setVec3("spotLight.direction", camera.Front);
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("model", model);

    model1.Draw(lightingShader);
}

