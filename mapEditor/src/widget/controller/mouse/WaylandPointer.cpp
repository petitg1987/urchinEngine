#include <cstring>
#include <QtGui/QGuiApplication>
#include <wayland-client.h>
#include <UrchinCommon.h>

#include "widget/controller/mouse/WaylandPointer.h"
#include "pointer-warp-v1-client-protocol.h"
#include "relative-pointer-unstable-v1-client-protocol.h"

namespace urchin {

    namespace {
        //all listener callbacks must be defined (libwayland aborts on a null callback): unused events are ignored with empty lambdas
        const wl_registry_listener REGISTRY_LISTENER = {
            .global = WaylandPointer::onRegistryGlobal,
            .global_remove = [](void*, wl_registry*, uint32_t) { /* not used */ }
        };

        const wl_pointer_listener POINTER_LISTENER = {
            .enter = WaylandPointer::onPointerEnter,
            .leave = WaylandPointer::onPointerLeave,
            .motion = [](void*, wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t) { /* handled by Qt */ },
            .button = [](void*, wl_pointer*, uint32_t, uint32_t, uint32_t, uint32_t) { /* handled by Qt */ },
            .axis = [](void*, wl_pointer*, uint32_t, uint32_t, wl_fixed_t) { /* handled by Qt */ },
            .frame = [](void*, wl_pointer*) { /* handled by Qt */ },
            .axis_source = [](void*, wl_pointer*, uint32_t) { /* handled by Qt */ },
            .axis_stop = [](void*, wl_pointer*, uint32_t, uint32_t) { /* handled by Qt */ },
            .axis_discrete = [](void*, wl_pointer*, uint32_t, int32_t) { /* handled by Qt */ },
            .axis_value120 = [](void*, wl_pointer*, uint32_t, int32_t) { /* handled by Qt */ },
            .axis_relative_direction = [](void*, wl_pointer*, uint32_t, uint32_t) { /* handled by Qt */ }
        };

        const zwp_relative_pointer_v1_listener RELATIVE_POINTER_LISTENER = {
            .relative_motion = WaylandPointer::onRelativeMotion
        };
    }

    WaylandPointer::WaylandPointer() :
            display(nullptr),
            pointerWarp(nullptr),
            relativePointerManager(nullptr),
            pointer(nullptr),
            relativePointer(nullptr),
            focusedSurface(nullptr),
            enterSerial(0),
            windowSurface(nullptr),
            accumulatedDeltaX(0.0),
            accumulatedDeltaY(0.0) {
        const auto* waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
        if (!waylandApp || !waylandApp->display() || !waylandApp->seat()) {
            Logger::instance().logWarning("Wayland native interface not available: mouse cannot be warped");
            return;
        }
        display = waylandApp->display();

        //bind globals with a dedicated queue to not interfere with Qt event dispatching
        wl_event_queue* queue = wl_display_create_queue(display);
        auto* displayWrapper = static_cast<wl_display*>(wl_proxy_create_wrapper(display));
        wl_proxy_set_queue(reinterpret_cast<wl_proxy*>(displayWrapper), queue);
        wl_registry* registry = wl_display_get_registry(displayWrapper);
        wl_proxy_wrapper_destroy(displayWrapper);
        wl_registry_add_listener(registry, &REGISTRY_LISTENER, this);
        wl_display_roundtrip_queue(display, queue);
        wl_registry_destroy(registry);

        //bound proxies inherit the registry queue: move them to the Qt queue (seat queue) before destroying the dedicated queue
        wl_event_queue* qtQueue = wl_proxy_get_queue(reinterpret_cast<wl_proxy*>(waylandApp->seat()));
        if (pointerWarp) {
            wl_proxy_set_queue(reinterpret_cast<wl_proxy*>(pointerWarp), qtQueue);
        }
        if (relativePointerManager) {
            wl_proxy_set_queue(reinterpret_cast<wl_proxy*>(relativePointerManager), qtQueue);
        }
        wl_event_queue_destroy(queue);

        if (!pointerWarp) {
            Logger::instance().logWarning("Wayland compositor doesn't support 'wp_pointer_warp_v1' protocol: mouse cannot be warped");
        }
        if (!relativePointerManager) {
            Logger::instance().logWarning("Wayland compositor doesn't support 'zwp_relative_pointer_manager_v1' protocol");
        }
        if (!pointerWarp || !relativePointerManager) {
            return;
        }

        //own pointer (on the Qt queue): get the entered surface and the enter serial required by the warp request
        pointer = wl_seat_get_pointer(waylandApp->seat());
        wl_pointer_add_listener(pointer, &POINTER_LISTENER, this);

        relativePointer = zwp_relative_pointer_manager_v1_get_relative_pointer(relativePointerManager, pointer);
        zwp_relative_pointer_v1_add_listener(relativePointer, &RELATIVE_POINTER_LISTENER, this);
    }

    WaylandPointer::~WaylandPointer() {
        if (relativePointer) {
            zwp_relative_pointer_v1_destroy(relativePointer);
        }
        if (pointer) {
            if (wl_proxy_get_version(reinterpret_cast<wl_proxy*>(pointer)) >= WL_POINTER_RELEASE_SINCE_VERSION) {
                wl_pointer_release(pointer);
            } else {
                wl_pointer_destroy(pointer);
            }
        }
        if (relativePointerManager) {
            zwp_relative_pointer_manager_v1_destroy(relativePointerManager);
        }
        if (pointerWarp) {
            wp_pointer_warp_v1_destroy(pointerWarp);
        }
    }

    void WaylandPointer::onRegistryGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t) {
        auto* self = static_cast<WaylandPointer*>(data);
        if (std::strcmp(interface, wp_pointer_warp_v1_interface.name) == 0) {
            self->pointerWarp = static_cast<wp_pointer_warp_v1*>(wl_registry_bind(registry, name, &wp_pointer_warp_v1_interface, 1));
        } else if (std::strcmp(interface, zwp_relative_pointer_manager_v1_interface.name) == 0) {
            self->relativePointerManager = static_cast<zwp_relative_pointer_manager_v1*>(wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, 1));
        }
    }

    void WaylandPointer::onPointerEnter(void* data, wl_pointer*, uint32_t serial, wl_surface* surface, int32_t, int32_t) {
        auto* self = static_cast<WaylandPointer*>(data);
        self->focusedSurface = surface;
        self->enterSerial = serial;
    }

    void WaylandPointer::onPointerLeave(void* data, wl_pointer*, uint32_t, wl_surface* surface) {
        auto* self = static_cast<WaylandPointer*>(data);
        if (self->focusedSurface == surface) {
            self->focusedSurface = nullptr;
        }
    }

    void WaylandPointer::onRelativeMotion(void* data, zwp_relative_pointer_v1*, uint32_t, uint32_t, wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t, wl_fixed_t) {
        auto* self = static_cast<WaylandPointer*>(data);
        self->accumulatedDeltaX += wl_fixed_to_double(dx);
        self->accumulatedDeltaY += wl_fixed_to_double(dy);
    }

    /**
     * Must be called when the pointer enters the window: the surface under the pointer is the window surface.
     * Wayland pointer events are dispatched before Qt delivers its enter event: the surface is already known at this point.
     */
    void WaylandPointer::captureWindowSurface() {
        if (focusedSurface) {
            windowSurface = focusedSurface;
        }
    }

    /**
     * Must be called before the window surface is destroyed
     */
    void WaylandPointer::releaseWindowSurface() {
        windowSurface = nullptr;
    }

    /**
     * Warp the pointer on the window surface, even if the pointer is on another surface of the application (enter serial is valid for any surface of the application)
     * @param x X position in logical coordinates relative to the window surface
     * @param y Y position in logical coordinates relative to the window surface
     */
    bool WaylandPointer::warp(double x, double y) const {
        if (!pointerWarp || !windowSurface || !focusedSurface) {
            return false;
        }
        wp_pointer_warp_v1_warp_pointer(pointerWarp, windowSurface, pointer, wl_fixed_from_double(x), wl_fixed_from_double(y), enterSerial);
        wl_display_flush(display);
        return true;
    }

    /**
     * Provide the pointer movement (logical coordinates) accumulated since the last call
     * @return True when the relative motion is available
     */
    bool WaylandPointer::takeRelativeMotion(double& deltaX, double& deltaY) {
        if (!relativePointer) {
            return false;
        }
        deltaX = accumulatedDeltaX;
        deltaY = accumulatedDeltaY;
        resetRelativeMotion();
        return true;
    }

    void WaylandPointer::resetRelativeMotion() {
        accumulatedDeltaX = 0.0;
        accumulatedDeltaY = 0.0;
    }

}
