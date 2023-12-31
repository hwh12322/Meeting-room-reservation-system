#ifndef BOOKINGS_H
#define BOOKINGS_H


#include <string>
enum class TimeSlot {
    Morning,   // 上午
    Afternoon  // 下午
};

enum class BookingStatus {
    Pending,   // 待审批
    Approved,  // 通过
    Rejected,  // 不通过
    Cancelled  // 已取消
};



#endif