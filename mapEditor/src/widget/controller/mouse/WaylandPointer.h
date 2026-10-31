#pragma once

#include <cstdint>

struct wl_display;
struct wl_registry;
struct wl_pointer;
struct wl_surface;
struct wp_pointer_warp_v1;
struct zwp_relative_pointer_manager_v1;
struct zwp_relative_pointer_v1;

namespace urchin {

    class WaylandPointer {
        public:
            WaylandPointer();
            ~WaylandPointer();
            WaylandPointer(const WaylandPointer&) = delete;
            WaylandPointer& operator=(const WaylandPointer&) = delete;

            void captureWindowSurface();
            void releaseWindowSurface();
            bool warp(double, double) const;

            bool takeRelativeMotion(double&, double&);
            void resetRelativeMotion();

            static void onRegistryGlobal(void*, wl_registry*, uint32_t, const char*, uint32_t);
            static void onPointerEnter(void*, wl_pointer*, uint32_t, wl_surface*, int32_t, int32_t);
            static void onPointerLeave(void*, wl_pointer*, uint32_t, wl_surface*);
            static void onRelativeMotion(void*, zwp_relative_pointer_v1*, uint32_t, uint32_t, int32_t, int32_t, int32_t, int32_t);

        private:
            wl_display* display;
            wp_pointer_warp_v1* pointerWarp;
            zwp_relative_pointer_manager_v1* relativePointerManager;
            wl_pointer* pointer;
            zwp_relative_pointer_v1* relativePointer;

            wl_surface* focusedSurface; //surface of the application under the pointer
            uint32_t enterSerial;
            wl_surface* windowSurface; //surface of the window where the pointer is warped

            double accumulatedDeltaX;
            double accumulatedDeltaY;
    };

}
