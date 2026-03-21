#pragma once

#include <cstdint>

enum EsfStatus : uint8_t {
    /* 通用错误 */
    ESF_SUCCESS = 0,    // 正常
    ESF_TIMEOUT,        // 超时
    ESF_NULLPTR_ERROR,  // 传入空指针
    ESF_MULTIPLE_ERROR, // 多种错误
    /* 队列相关错误 */
    ESF_QUEUE_FULL,       // 待加入元素的队列已满
    ESF_QUEUE_EMPTY,      // 待弹出元素的队列已空
    ESF_QUEUE_PUSH_ERROR, // 加入元素时出现错误
    ESF_QUEUE_POP_ERROR,  // 弹出元素时出现错误
    /* 通信相关错误 */
    ESF_CONNECTIVITY_ERROR,         // 通信出现错误
    ESF_CONNECTIVITY_SEND_ERROR,    // 发送消息时出现错误
    ESF_CONNECTIVITY_RECEIVE_ERROR, // 接收消息时出现错误
    /* 电机相关错误 */
    ESF_MOTOR_HELPER_REGISTER_ERROR,   // 电机注册到 helper 时出现错误
    ESF_MOTOR_NOT_REGISTERD_TO_HELPER, // 电机未注册到该 helper
    ESF_MOTOR_HELPER_DATA_INVALIED,    // (存在通信协议校验的情况下) helper 接收到的电机反馈帧校验未通过
};