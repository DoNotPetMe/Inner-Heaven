add_executable(InnerHeavenInjector
    src/main.cpp
    src/process.cpp
    src/injection.cpp
)

target_include_directories(InnerHeavenInjector PRIVATE src)
