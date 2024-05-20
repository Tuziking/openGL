#include <bits/stdc++.h>

#include "glm/glm.hpp"

#include "Particle.h"

// 生成的随机实数之间的最小间隔的倒数
const int MAX = 1000;
// Y的最小值
const float Y_BOUND = -5;

Particle::Particle(float t0, bool init) {
    this->t0 = t0;
    // 初始位置的x和z坐标都在范围内随机生成，y坐标根据是否为初始化模式分为随机生成和指定为5两种情况
    x = glm::vec3(randFloat(-5, 5), init ? randFloat(-5, 5) : 5, randFloat(-5, 0));
    // 设置初始速度向量
    v = glm::vec3(0, -1, 0);
    // 随机生成初始加速度向量
    a = glm::vec3(randFloat(-2, 2), randFloat(-2, 2), randFloat(-2, 2));
}

glm::vec3 Particle::getX() {
    return x;
}

bool Particle::exist() {
    return getX().y > Y_BOUND;
}

void Particle::update(float t) {
    // 根据匀加速运动的性质更新位置向量和速度向量
    x = x + v * (t - t0) + 0.5f * a * (t - t0) * (t - t0);
    v = v + a * (t - t0);
    // 重新随机生成加速度向量
    a = glm::vec3(randFloat(-2, 2), randFloat(-2, 2), randFloat(-2, 2));
    // 重置更新实验
    t0 = t;
}

float Particle::randFloat(float left, float right) {
    return rand() % (MAX + 1) / (float) MAX * (right - left) + left;
}
