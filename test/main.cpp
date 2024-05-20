#define STB_IMAGE_IMPLEMENTATION

#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Particle.h"

// WINDOW_WIDTH：窗口的宽度
// WINDOW_HEIGHT：窗口的高度
// PARTICLE_NUM：同时存在的粒子数量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800, PARTICLE_NUM = 1000;
// 每片雪花用三维空间中的一个正方形和其上的纹理表示，正方形分为两个三角形绘制，每行前三个实数表示顶点的坐标，后两个实数表示该点对应的纹理坐标
float vertices[] = {
        -0.01f, -0.01f, -0.01f, 0.0f, 0.0f,
        0.01f, -0.01f, -0.01f, 1.0f, 0.0f,
        0.01f, 0.01f, -0.01f, 1.0f, 1.0f,
        0.01f, 0.01f, -0.01f, 1.0f, 1.0f,
        -0.01f, 0.01f, -0.01f, 0.0f, 1.0f,
        -0.01f, -0.01f, -0.01f, 0.0f, 0.0f
};
// 顶点着色器的GLSL代码
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"

                                 "out vec2 TexCoord;\n"

                                 "uniform mat4 model;\n"
                                 "uniform mat4 view;\n"
                                 "uniform mat4 proj;\n"

                                 "void main()\n"
                                 "{\n"
                                 "    gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
                                 "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                 "}\n\0";
// 片段着色器的GLSL代码
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"

                                   "in vec2 TexCoord;\n"

                                   "uniform sampler2D ourTexture;\n"

                                   "void main()\n"
                                   "{\n"
                                   "    FragColor = texture(ourTexture, TexCoord);\n"
                                   "}\n\0";

GLFWwindow *window;
// VAO：顶点数组对象
// VBO：顶点缓冲对象
// program：着色器程序
// texture：纹理
unsigned int VAO, VBO, program, texture;
// particles：存储所有粒子
// particlesTmp：用于存储一个时间间隔后还未消亡的粒子和它们的新状态
std::vector<Particle> particles, particlesTmp;

// 窗口大小改变后调用的函数
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // 重新设置视口，使其与新的窗口大小匹配
    glViewport(0, 0, width, height);
}

// GLFW窗口、着色器、顶点数组对象、顶点缓冲对象等的初始化函数
void init() {
    // 初始化GLFW
    glfwInit();
    // 将使用的OpenGL的版本设置为3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 将OpenGL设置为核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建GLFW窗口
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SnowSimulator", NULL, NULL);
    // 如果窗口创建失败，输出相关信息并退出程序
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        exit(0);
    }
    // 将window设为当前线程的内容
    glfwMakeContextCurrent(window);
    // 设置窗口大小改变时调用的函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // 初始化GLAD，如果初始化失败，输出相关信息并退出程序
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }

    // 用于判断着色器的GLSL代码是否编译成功
    int success;

    // 创建顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 指定顶点着色器的源代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // 编译顶点着色器
    glCompileShader(vertexShader);
    // 获取顶点着色器的编译状态
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // 如果顶点着色器编译失败，输出相关信息并退出程序
    if (!success) {
        std::cout << "Failed to compile vertex shader source" << std::endl;
        exit(0);
    }

    // 创建片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 指定片段着色器的源代码
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // 编译片段着色器
    glCompileShader(fragmentShader);
    // 获取片段着色器的编译状态
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // 如果片段着色器编译失败，输出相关信息并退出程序
    if (!success) {
        std::cout << "Failed to compile fragment shader source" << std::endl;
        exit(0);
    }

    // 创建着色器程序
    program = glCreateProgram();
    // 将顶点着色器和片段着色器链接到着色器程序
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    // 获取着色器程序的链接状态
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    // 如果着色器程序链接失败，输出相关信息并退出程序
    if(!success) {
        std::cout << "Failed to link program" << std::endl;
        exit(0);
    }
    // 链接完成后，可以删除顶点着色器和片段着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    imageDate = stbi_load("../resources/texture/snowflower.jpg", &width, &height, &channelNum, 0);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageDate);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 启用OpenGL的深度检测，使绘制的图形更具真实感
    glEnable(GL_DEPTH_TEST);

    // 初始化粒子系统
    for (unsigned int i = 0; i < PARTICLE_NUM; i++)
        particles.push_back(Particle(glfwGetTime(), true));
}

void render() {
    // 设置背景色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // 将背景全置为上面设置的颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 使用着色器程序
    glUseProgram(program);

    // 初始化观察矩阵和投影矩阵，对每个粒子而言，观察矩阵和投影矩阵都是相同的
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    // 设置观察矩阵和投影矩阵
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    proj = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);
    // 将观察矩阵和投影矩阵与着色器代码中的uniform变量绑定
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

    // 将particleTmp初始化为空
    particlesTmp.clear();
    // 遍历当前的所有粒子
    for (unsigned int i = 0; i < particles.size(); i++) {
        // 初始化模型矩阵，对每个粒子而言，模型矩阵不同
        glm::mat4 model = glm::mat4(1.0f);
        // 更新粒子的状态
        particles[i].update(glfwGetTime());
        // 根据粒子的位置向量，设置模型矩阵
        model = glm::translate(model, particles[i].getX());
        // 将模型矩阵与着色器代码中的uniform变量绑定
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        // 如果粒子还未消亡，将其加入particleTmp
        if (particles[i].exist())
            particlesTmp.push_back(particles[i]);
        else
            particlesTmp.push_back(Particle(glfwGetTime(), false));

        // 绘制粒子
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // 更新粒子系统
    particles = particlesTmp;
}

int main() {
    // 调用初始化函数
    init();

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // 程序结束后的清理
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}