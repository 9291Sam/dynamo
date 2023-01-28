#include <numeric>

#include <sebib/seblog.hpp>

#include "window.hpp"

Window::Window(WindowSize windowSize, const char* windowName)
{
    seb::assertFatal(vkfw::init(), "Failed to initialize window");


    vkfw::setWindowHints(vkfw::WindowHints {.clientAPI {vkfw::ClientAPI::None}});

    auto [result, maybeWindow] = 
        vkfw::createWindowUnique(windowSize.width, windowSize.height, windowName);

    seb::assertFatal(result, "Failed to create window");


    this->window_ptr = std::move(maybeWindow);
}

Window::~Window() 
{
    seb::assertWarn(
        this->window_ptr->destroy(),
        "Failed to destroy VKFW instance"
    );

    seb::assertWarn(
        vkfw::terminate(),
        "Failed to terminate VKFW instance"
    );
}

auto Window::getFrameBufferSize() const
    -> WindowSize
{
    auto [result, maybeTupleOfWindowSize] = this->window_ptr->getFramebufferSize();

    seb::assertFatal(
        result,
        "Failed to poll frame buffer size"
    );

    auto [width, height] = maybeTupleOfWindowSize;

    return WindowSize {
        .width  {static_cast<std::uint32_t>(width)},
        .height {static_cast<std::uint32_t>(height)},
    };
}

auto Window::getFrameTimeS() const
    -> float
{
    return static_cast<float>(this->last_frame_duration.count()) / 1'000'000'000.0f;
}

auto Window::shouldClose() const 
    -> bool
{
    auto [result, maybeShouldClose] = this->window_ptr->shouldClose();
    
    seb::assertFatal(
        result, 
        "Failed to check window should close state"
    );

    return maybeShouldClose;
}

auto Window::createSurface(const vk::Instance& instance) const
    -> vk::UniqueSurfaceKHR
{
    return vkfw::createWindowSurfaceUnique(instance, *this->window_ptr); 
}

void Window::pollEvents() 
{
    seb::assertFatal(
        vkfw::pollEvents(),
        "Failed to poll window events"
    );

    FPSTimePoint currentTime = FPSClock::now();

    this->last_frame_duration = currentTime - this->last_frame_time;

    this->last_frame_time = currentTime;  
}

void Window::blockThisThreadIfMinimized() const
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(
            static_cast<GLFWwindow*>(this->window_ptr.get()), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(
            static_cast<GLFWwindow*>(this->window_ptr.get()), &width, &height);
        glfwWaitEvents();

        // This lessens the impact of this function busy waiting
        std::this_thread::yield();
    }
}

bool Window::isKeyPressed(vkfw::Key key) const
{
    int keyState = glfwGetKey(
        static_cast<GLFWwindow*>(*this->window_ptr), 
        static_cast<int>(key)
    );

    const char* maybeError;

    glfwGetError(&maybeError);

    if (maybeError == nullptr)
    {
        return keyState == GLFW_PRESS;
    }

    seb::panic("Glfw error: {}", maybeError);
}