#ifndef SNOWSIMULATOR_PARTICLE_H
#define SNOWSIMULATOR_PARTICLE_H


class Particle {
private:
    // 上次更新状态的时间
    float t0;
    // x：位置向量
    // v：速度向量
    // a：加速度向量
    glm::vec3 x, v, a;
    // 用于生成在范围[left, right]内的一个随机实数
    float randFloat(float left, float right);

public:
    // 根据生成时间、是否为初始化模式来生成一个粒子
    Particle(float t, bool init);
    // 获取位置向量
    glm::vec3 getX();
    // 判断是否未消亡
    bool exist();
    // 更新状态
    void update(float t);
};


#endif
