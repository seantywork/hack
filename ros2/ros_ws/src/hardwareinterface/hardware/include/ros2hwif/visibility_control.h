
#ifndef ROS2HWIF_VISIBILITY_CONTROL_HPP_
#define ROS2HWIF_VISIBILITY_CONTROL_HPP_


#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define AGV_BASE_EXPORT __attribute__((dllexport))
#define AGV_BASE_IMPORT __attribute__((dllimport))
#else
#define AGV_BASE_EXPORT __declspec(dllexport)
#define AGV_BASE_IMPORT __declspec(dllimport)
#endif
#ifdef AGV_BASE_BUILDING_DLL
#define AGV_BASE_PUBLIC AGV_BASE_EXPORT
#else
#define AGV_BASE_PUBLIC AGV_BASE_IMPORT
#endif
#define AGV_BASE_PUBLIC_TYPE AGV_BASE_PUBLIC
#define AGV_BASE_LOCAL
#else
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

#endif  
