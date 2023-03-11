#ifndef SRC_WINDOW_HPP
#define SRC_WINDOW_HPP

#include <atomic>
#include <cstdint>
#include <chrono>
#include <deque>

#include "vulkan/includes.hpp"

/// @brief Abstraction over a system Window
/// Allows for easy creation of a window surface
/// Allows easy delta time grabbing
class Window 
{
public:

    /// @brief Creates a blank window with @param windowSize as it's size
    /// and @param windowName as it's title bar title. @param windowName
    /// must be a pointer to a valid C-Style string
    /// @todo add custom window icon
    Window(vk::Extent2D size, std::string name);
    ~Window();

    Window()                         = delete;
    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    [[nodiscard]] vk::Extent2D size() const;
    [[nodiscard]] float getDeltaTimeSeconds() const;
    [[nodiscard]] bool shouldClose() const;
    [[nodiscard]] bool isKeyPressed(vkfw::Key key) const;
    [[nodiscard]] auto getMouseDelta() -> std::pair<double, double>;

    [[nodiscard]] vk::UniqueSurfaceKHR createSurface(vk::Instance) const;

    void attachCursor() const;
    void detachCursor() const;

    void pollEvents();

    void blockThisThreadIfMinimized() const;

private:

    vkfw::UniqueWindow window_ptr;
    std::string window_name;
    

    std::pair<double, double> previous_mouse_pos;

    std::chrono::time_point<std::chrono::steady_clock> last_frame_time;
    std::chrono::duration<std::int64_t, std::nano>    last_frame_duration {160000000};
    mutable std::atomic<std::uint_fast8_t> ignore_frames {3};
    mutable std::atomic<bool> is_currently_focused;
    mutable std::atomic<bool> is_camera_mobile;
}; // class Window

#endif // SRC_WINDOW_HPP