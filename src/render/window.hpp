#pragma once

#ifndef SRC_WINDOW_HPP
#define SRC_WINDOW_HPP

#include <atomic>
#include <cstdint>
#include <chrono>
#include <deque>

#include "vulkan_includes.hpp"

class Window 
{
public:
    typedef std::chrono::steady_clock FPSClock;
    typedef std::chrono::time_point<FPSClock> FPSTimePoint;
public:

    /// @brief Creates a blank window with @param windowSize as it's size
    /// and @param windowName as it's title bar title. @param windowName
    /// must be a pointer to a valid C-Style string
    /// @todo add custom window icon
    Window(vk::Extent2D windowSize, const char* windowName);
    ~Window();

    Window()                         = delete;
    Window(const Window&)            = delete;
    Window(Window&&)                 = default;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = default;

    /// @brief Returns the window's drawable area. 
    [[nodiscard, gnu::pure]] vk::Extent2D getSize() const;
    
    /// @brief returns the delta frame time
    [[nodiscard, gnu::pure]] auto getFrameTimeS() const 
        -> float;
    
    [[nodiscard]] auto shouldClose() const 
        -> bool;

    /// @brief Creates a vk::UniqueSurfaceKHR that is bound to this window
    /// and the vk::Instance provided in @param
    [[nodiscard]] auto createSurface(const vk::Instance&) const
        -> vk::UniqueSurfaceKHR;

    [[nodiscard]] bool isKeyPressed(vkfw::Key key) const;

    /// @brief General window state update function. Must be called once 
    /// per frame in order for the window to respond.
    void pollEvents();

    /// @brief Function that blocks this thread if the window is minimized
    /// until the window is un-minimized
    /// 
    /// Call this function during the implementations resize routine
    /// to ensure that this the window is only drawn to when it has a non
    /// zero size
    void blockThisThreadIfMinimized() const;

private:

    vkfw::UniqueWindow window_ptr;
    std::string window_name;
    
    FPSTimePoint last_frame_time;
    std::chrono::duration<std::int64_t, std::nano> last_frame_duration {160000000};
}; // class Window

#endif // SRC_WINDOW_HPP