cmake_minimum_required(VERSION 3.16)

project(course_part1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appcourse_part1
    main.cpp
    domain/author.cpp
    domain/author.hpp
    domain/writing.cpp
    domain/writing.hpp
    application/authors_file_storage.hpp
    application/authors_repository.hpp
    application/authors_service.cpp
    application/authors_service.hpp
    application/operation_result.hpp
    application/writings_repository.hpp
    application/writings_file_storage.hpp
    application/writings_service.cpp
    application/writings_service.hpp
    infrastructure/csv_authors_file_storage.cpp
    infrastructure/csv_authors_file_storage.hpp
    infrastructure/csv_writings_file_storage.cpp
    infrastructure/csv_writings_file_storage.hpp
    infrastructure/in_memory_authors_repository.cpp
    infrastructure/in_memory_authors_repository.hpp
    infrastructure/in_memory_writings_repository.cpp
    infrastructure/in_memory_writings_repository.hpp

    presentation/main_window.hpp presentation/main_window.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appcourse_part1 PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appcourse_part1
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appcourse_part1
    PRIVATE
        Qt6::Widgets
)

target_include_directories(appcourse_part1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS appcourse_part1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
