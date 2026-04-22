#ifndef VIP_MOUNT_HELPER_H
#define VIP_MOUNT_HELPER_H

#include "Player.h"

namespace VipMountHelper
{
    // Изучение премиум маунтов при получении премиума
    // Если у игрока есть премиум - изучает маунты (31700, 18991, 18992)
    // Если премиума нет - удаляет эти маунты
    void VipMountLearn(Player* player);
}

#endif // VIP_MOUNT_HELPER_H