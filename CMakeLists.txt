if(APPLE)

    cmake_minimum_required(VERSION 3.16)
    project(bluetooth VERSION 4.2)

    find_package(Albert REQUIRED)

    albert_plugin(
        LINK PRIVATE "objc" "-framework IOBluetooth"
        QT Core
    )

    target_compile_options(${PROJECT_NAME} PRIVATE "-fobjc-arc")

endif()
