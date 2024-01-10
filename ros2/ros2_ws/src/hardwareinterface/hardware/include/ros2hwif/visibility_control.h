
#ifndef ROS2HWIF_VISIBILITY_CONTROL_HPP_
#define ROS2HWIF_VISIBILITY_CONTROL_HPP_




#define ROS2HWIF_EXPORT __attribute__((visibility("default")))
#define ROS2HWIF_IMPORT
#if __GNUC__ >= 4
#define ROS2HWIF_PUBLIC __attribute__((visibility("default")))
#define ROS2HWIF_LOCAL __attribute__((visibility("hidden")))
#else
#define ROS2HWIF_PUBLIC
#define ROS2HWIF_LOCAL
#endif
#define ROS2HWIF_PUBLIC_TYPE
#endif

#endif  //
