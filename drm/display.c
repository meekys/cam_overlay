#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

#include "../common.h"

int device;
drmModeCrtc* crtc;
struct gbm_device* gbm;
struct gbm_surface* gbm_surface;

uint32_t overlayPlaneId;
struct gbm_bo* previousBo = NULL;
uint32_t previousFb;

drmModeRes* findResources(const char* card)
{
    log_verbose("open(%s)", card);
    device = open(card, O_RDWR | O_CLOEXEC);
    if (!device)
        return NULL;

    log_verbose("drmModeGetResources(%i)", device);
    drmModeRes* resources = drmModeGetResources(device);
    if (resources)
        return resources;

    // No resources on device
    close(device);
    device = 0;
    return NULL;
}

drmModeConnector* findConnector(drmModeRes* resources)
{
    // iterate the connectors
    for (int i = 0; i < resources->count_connectors; i++)
    {
        log_verbose("drmModeGetConnector(%i, %i)", device, i);
        drmModeConnector* connector = drmModeGetConnector(device, resources->connectors[i]);

        // pick the first valid connected connector
        if (connector->connection == DRM_MODE_CONNECTED)
            return connector;

        drmModeFreeConnector(connector);
    }

    // No Connector found
    return NULL;
}

drmModeEncoder* findEncoder(drmModeConnector* connector)
{
    if (connector->encoder_id)
    {
        log_verbose("drmModeGetEncoder(%i, %i)", device, connector->encoder_id);
        return drmModeGetEncoder(device, connector->encoder_id);
    }

    // No Encoder found
    return NULL;
}

drmModeCrtc* findCrtc(drmModeEncoder* encoder)
{
    if (encoder->crtc_id)
    {
        log_verbose("drmModeGetCrtc(%i, %i)", device, encoder->crtc_id);
        return drmModeGetCrtc(device, encoder->crtc_id);
    }

    // No Crtc found
    return NULL;
}

uint32_t findOverlayPlane(drmModeRes* resources, drmModeCrtc* crtc, uint32_t pixelFormat)
{
    uint32_t crtcMask = 0;
    for (uint32_t index = 0; index < resources->count_crtcs; index++)
    {
        if (resources->crtcs[index] == crtc->crtc_id)
        {
             crtcMask = (1 << index);
             break;
        }
    }

    log_verbose("drmModeGetPlaneResources(%i)", device);
    drmModePlaneRes* planeResources = drmModeGetPlaneResources(device);
    if (!planeResources)
        return 0;

    uint32_t planeId = 0;
    for (uint32_t planeIndex = 0; planeIndex < planeResources->count_planes; planeIndex++)
    {
        log_verbose("drmModeGetPlane(%i, [%d])", device, planeIndex);
        drmModePlane* plane = drmModeGetPlane(device, planeResources->planes[planeIndex]);
        if (!plane)
            continue;

        if ((plane->possible_crtcs & crtcMask) == 0)
        {
            drmModeFreePlane(plane);
            continue;
        }

        uint32_t formatIndex = 0;
        for (; formatIndex < plane->count_formats; formatIndex++)
        {
            if (plane->formats[formatIndex] == pixelFormat)
                break;
        }

        if (formatIndex >= plane->count_formats)
        {
            drmModeFreePlane(plane);
            continue;
        }

        log_verbose("drmModeObjectGetProperties(%i, [%d], DRM_MODE_OBJECT_PLANE)", device, planeIndex);
        drmModeObjectProperties* properties = drmModeObjectGetProperties(device, plane->plane_id, DRM_MODE_OBJECT_PLANE);
        for (uint32_t propertyIndex = 0; propertyIndex < properties->count_props; propertyIndex++)
        {
            log_verbose("drmModeGetProperty(%i, [%d])", device, propertyIndex);
            drmModePropertyPtr property = drmModeGetProperty(device, properties->props[propertyIndex]);
            if (!property)
                continue;

            log_verbose("property->name: %s", property->name);
            if (strcmp(property->name, "type"))
            {
                drmModeFreeProperty(property);
                continue;
            }

            uint64_t overlayValue;
            bool hasValue = false;
            for (uint32_t enumIndex = 0; enumIndex < property->count_enums; enumIndex++)
            {
                struct drm_mode_property_enum* propertyEnum = &property->enums[enumIndex];
                log_verbose("propertyEnum->name: %s", propertyEnum->name);
                if (!strcmp(propertyEnum->name, "Overlay"))
                {
                    overlayValue = propertyEnum->value;
                    log_verbose("propertyEnum->value: %"PRIu64, overlayValue);
                    hasValue = true;
                    break;
                }
            }

            drmModeFreeProperty(property);

            if (hasValue && overlayValue == properties->prop_values[propertyIndex])
            {
                log_verbose("Using plane id %d [%d]", plane->plane_id, planeIndex);
                planeId = plane->plane_id;
                break;
            }
        }

        drmModeFreeObjectProperties(properties);
        drmModeFreePlane(plane);

        if (planeId)
            break;
    }

    drmModeFreePlaneResources(planeResources);

    return planeId;
}

extern void init_display(STATE_T *state, int display_unused, int layer)
{
    drmModeRes* resources = findResources("/dev/dri/card0");
    if (!resources)
        resources = findResources("/dev/dri/card1");
    if (!resources)
        fail_exit("Failed to get drm resource on /dev/dri/card0 or /dev/dri/card1");

    drmModeConnector* connector = findConnector(resources);
    if (!connector)
    {
        drmModeFreeResources(resources);
        fail_exit("Failed to find drm connector");
    }

    state->screen_width = connector->modes[0].hdisplay;
    state->screen_height = connector->modes[0].vdisplay;

    drmModeEncoder* encoder = findEncoder(connector);
    if (!encoder)
    {
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        fail_exit("No encoder found");
    }

    crtc = findCrtc(encoder);
    if (!crtc)
    {
        drmModeFreeEncoder(encoder);
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        fail_exit("No crtc found");
    }

    overlayPlaneId = findOverlayPlane(resources, crtc, GBM_FORMAT_ARGB8888);
    if (!overlayPlaneId)
    {
        drmModeFreeEncoder(encoder);
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        fail_exit("No valid overlay plane found");
    }

    drmModeFreeEncoder(encoder);
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources);

    gbm = gbm_create_device(device);
    if (!gbm)
        fail_exit("Failed to create gbm device");

    state->device = gbm;
}

extern NativeWindowType init_window(STATE_T *state, int display_unused, int layer)
{
    gbm_surface = gbm_surface_create(gbm, state->screen_width, state->screen_height, GBM_BO_FORMAT_ARGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    return gbm_surface;
}

extern void flip_display(STATE_T *state)
{
    struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbm_surface);
    uint32_t handles[4] = { gbm_bo_get_handle(bo).u32, 0, 0, 0 };
    uint32_t stride[4] = { gbm_bo_get_stride(bo), 0, 0, 0};
    uint32_t offset[4] = { 0, 0, 0, 0 };
    uint32_t fb;

    drmModeAddFB2(device, state->screen_width, state->screen_height, GBM_FORMAT_ARGB8888, &handles[0], &stride[0], &offset[0], &fb, 0);
    drmModeSetPlane(device, overlayPlaneId, crtc->crtc_id, fb, 0, 0, 0, state->screen_width, state->screen_height, 0, 0, state->screen_width << 16, state->screen_height << 16);

    if (previousBo)
    {
        drmModeRmFB(device, previousFb);
        gbm_surface_release_buffer(gbm_surface, previousBo);
    }

    previousBo = bo;
    previousFb = fb;
}

extern void close_display(STATE_T *state)
{
    log_verbose("drmModeSetPlane");
    drmModeSetPlane(device, overlayPlaneId, crtc->crtc_id, 0, 0, 0, 0, state->screen_width, state->screen_height, 0, 0, state->screen_width << 16, state->screen_height << 16);

    log_verbose("drmModeFreeCrtc");
    drmModeFreeCrtc(crtc);

    if (previousBo)
    {
        drmModeRmFB(device, previousFb);
        gbm_surface_release_buffer(gbm_surface, previousBo);
    }

    log_verbose("gbm_surface_destroy");
    gbm_surface_destroy(gbm_surface);
//    log_verbose("gbm_device_destroy");
//    gbm_device_destroy(gbm);
}
