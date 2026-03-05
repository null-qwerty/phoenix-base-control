#pragma once

#include <cstdint>

enum EsfReturnType : uint8_t {
    ESF_SUCCESS = 0,
    ESF_TIMEOUT,
    ESF_QUEUE_FULL,
    ESF_QUEUE_EMPTY,
    ESF_CONNECTIVITY_ERROR,
    ESF_CONNECTIVITY_SEND_ERROR,
    ESF_CONNECTIVITY_RECEIVE_ERROR,
};