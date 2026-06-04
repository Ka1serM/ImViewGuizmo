#pragma once

/*===============================================
ImViewGuizmo Single-Header Library by Marcel Kazemi

To use, do this in one (and only one) of your C++ files:
#define IMVIEWGUIZMO_IMPLEMENTATION
 #include "ImViewGuizmo.h"

In all other files, just include the header as usual:
#include "ImViewGuizmo.h"

Copyright (c) 2025 Marcel Kazemi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
======================================================*/

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>

#ifndef IMVIEWGUIZMO_VEC3
#define IMVIEWGUIZMO_USE_GLM_DEFAULTS
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define IMVIEWGUIZMO_VEC3 glm::vec3
#define IMVIEWGUIZMO_VEC4 glm::vec4
#define IMVIEWGUIZMO_QUAT glm::quat
#define IMVIEWGUIZMO_MAT4 glm::mat4
#endif

namespace ImViewGuizmo {
    using vec3_t = IMVIEWGUIZMO_VEC3;
    using vec4_t = IMVIEWGUIZMO_VEC4;
    using quat_t = IMVIEWGUIZMO_QUAT;
    using mat4_t = IMVIEWGUIZMO_MAT4;

#ifdef IMVIEWGUIZMO_USE_GLM_DEFAULTS
    namespace GizmoMath {
        inline float get_vec_comp(const vec3_t &v, int index) { return v[index]; }
        inline float get_vec_comp(const vec4_t &v, int index) { return v[index]; }

        inline vec3_t make_vec3(float x, float y, float z) { return vec3_t(x, y, z); }
        inline vec4_t make_vec4(float x, float y, float z, float w) { return vec4_t(x, y, z, w); }
        inline quat_t angleAxis(float angle, const vec3_t &axis) { return glm::angleAxis(angle, axis); }
        inline quat_t quatLookAt(const vec3_t &dir, const vec3_t &up) { return glm::quatLookAt(dir, up); }
        inline mat4_t mat4_identity() { return mat4_t(1.0f); }

        inline vec3_t cross(const vec3_t &a, const vec3_t &b) { return glm::cross(a, b); }
        inline float dot(const vec3_t &a, const vec3_t &b) { return glm::dot(a, b); }
        inline float dot(const quat_t &a, const quat_t &b) { return glm::dot(a, b); }
        inline float length(const vec3_t &v) { return glm::length(v); }
        inline float length2(const vec3_t &v) { return glm::length2(v); }
        inline vec3_t normalize(const vec3_t &v) { return glm::normalize(v); }
        inline vec3_t mix(const vec3_t &a, const vec3_t &b, float t) { return glm::mix(a, b, t); }
        inline vec3_t add_vv(const vec3_t &a, const vec3_t &b) { return a + b; }
        inline vec3_t subtract_vv(const vec3_t &a, const vec3_t &b) { return a - b; }
        inline vec3_t multiply_vf(const vec3_t &v, float f) { return v * f; }

        inline mat4_t mat4_cast(const quat_t &q) { return glm::mat4_cast(q); }
        inline mat4_t transpose(const mat4_t &m) { return glm::transpose(m); }
        inline vec3_t get_matrix_col(const mat4_t &m, int col) { return vec3_t(m[col]); }
        inline void set_matrix_col(mat4_t &m, int col, const vec4_t &v) { m[col] = v; }

        inline quat_t multiply_qq(const quat_t &a, const quat_t &b) { return a * b; }
        inline vec3_t multiply_qv(const quat_t &q, const vec3_t &v) { return q * v; }
        inline mat4_t multiply_mm(const mat4_t &a, const mat4_t &b) { return a * b; }
        inline vec4_t multiply_mv4(const mat4_t &m, const vec4_t &v) { return m * v; }
    }
#else
    // User must provide a GizmoMath namespace implementing all functions if using custom types
#endif

    // INTERFACE
    struct Style {
        float scale = 1.f;

        // Axis visuals
        float lineLength = 0.5f;
        float lineWidth = 4.0f;
        float circleRadius = 15.0f;
        float fadeFactor = 0.25f;

        // Highlight
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        float highlightWidth = 2.0f;

        // Axis
        ImU32 axisColors[3] = {
            IM_COL32(230, 51, 51, 255), // X
            IM_COL32(51, 230, 51, 255), // Y
            IM_COL32(51, 128, 255, 255) // Z
        };

        // Labels
        float labelSize = 1.0f;
        const char *axisLabels[3] = {"X", "Y", "Z"};
        ImU32 labelColor = IM_COL32(255, 255, 255, 255);

        //Big Circle
        float bigCircleRadius = 80.0f;
        ImU32 bigCircleColor = IM_COL32(255, 255, 255, 50);

        // Animation
        bool animateSnap = true;
        float snapAnimationDuration = 0.5f; // in seconds

        // Zoom/Pan Button Visuals
        float toolButtonRadius = 25.f;
        float toolButtonInnerPadding = 4.f;

        ImU32 toolButtonColor = IM_COL32(144, 144, 144, 50);
        ImU32 toolButtonHoveredColor = IM_COL32(215, 215, 215, 50);
        ImU32 toolButtonIconColor = IM_COL32(215, 215, 215, 225);

        // Object transform gizmo visuals
        float transformAxisLength = 64.0f;
        float transformPickRadius = 8.0f;
        float transformCenterRadius = 5.0f;
        float transformLineWidth = 2.5f;
        float transformActiveLineWidth = 4.0f;
        float transformArrowSize = 12.0f;
        float transformScaleHandleSize = 8.0f;
        float transformRotateRadius = 54.0f;
    };

    // Constants
    const vec3_t origin = GizmoMath::make_vec3(0.f, 0.f, 0.f);
    const vec3_t worldRight = GizmoMath::make_vec3(1.f, 0.f, 0.f); // +X
    const vec3_t worldUp = GizmoMath::make_vec3(0.f, -1.f, 0.f); // -Y
    const vec3_t worldForward = GizmoMath::make_vec3(0.f, 0.f, 1.f); // +Z
    const vec3_t axisVectors[3] = {GizmoMath::make_vec3(1, 0, 0), GizmoMath::make_vec3(0, 1, 0), GizmoMath::make_vec3(0, 0, 1)};
    static constexpr mat4_t gizmoProjectionMatrix = {
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -0.01f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    inline Style &GetStyle() {
        static Style style;
        return style;
    }

    // Gizmo Axis Struct
    struct GizmoAxis {
        int id; // 0-5 for (+X,-X,+Y,-Y,+Z,-Z), 6=center
        int axisIndex; // 0=X, 1=Y, 2=Z
        float depth; // Screen-space depth
        vec3_t direction; // 3D vector
    };

    enum ActiveTool {
        TOOL_NONE,
        TOOL_GIZMO,
        TOOL_DOLLY,
        TOOL_PAN,
        TOOL_TRANSFORM
    };

    enum TransformOperation {
        TRANSFORM_TRANSLATE,
        TRANSFORM_ROTATE,
        TRANSFORM_SCALE
    };

    struct Context {
        int hoveredAxisID = -1;
        int hoveredTransformAxisID = -1;
        bool isZoomButtonHovered = false;
        bool isPanButtonHovered = false;
        ActiveTool activeTool = TOOL_NONE;
        TransformOperation activeTransformOperation = TRANSFORM_TRANSLATE;
        int activeTransformAxisID = -1;
        ImVec2 transformDragStartMouse{0.0f, 0.0f};
        ImVec2 transformDragStartTangent{0.0f, 0.0f};
        float transformDragStartAngle = 0.0f;
        vec3_t transformStartPosition;
        quat_t transformStartRotation;
        quat_t transformStartOrientation;
        vec3_t transformStartScale;

        // Animation state
        bool isAnimating = false;
        float animationStartTime = 0.f;

        vec3_t startPos;
        vec3_t targetPos;
        vec3_t startUp;
        vec3_t targetUp;

        vec3_t animStartDir, animTargetDir;
        float animStartDist, animTargetDist;

        void Reset() {
            hoveredAxisID = -1;
            hoveredTransformAxisID = -1;
            isZoomButtonHovered = false;
            isPanButtonHovered = false;
        }
    };

    // Global context
    inline Context &GetContext() {
        static Context ctx;
        return ctx;
    }

    static float ImLengthSqr(const ImVec2 &v) { return v.x * v.x + v.y * v.y; }
    static float mix(float a, float b, float t) { return a * (1.0f - t) + b * t; }

    inline void BeginFrame() {
        static int lastFrame = -1;
        const int currentFrame = ImGui::GetFrameCount();
        if (lastFrame != currentFrame)
            lastFrame = currentFrame;
        GetContext().Reset();
    }

    inline bool IsUsing() {
        return GetContext().activeTool != TOOL_NONE;
    }

    inline bool IsTransformUsing() {
        return GetContext().activeTool == TOOL_TRANSFORM;
    }

    inline bool IsOver() {
        const Context &ctx = GetContext();
        return ctx.hoveredAxisID != -1 || ctx.hoveredTransformAxisID != -1 || ctx.isZoomButtonHovered || ctx.isPanButtonHovered;
    }

    /// @brief Renders and handles the view gizmo logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (will be modified).
    /// @param pivot The position the camera will rotate around.
    /// @param position The top-left screen position where the gizmo should be rendered.
    /// @param rotationSpeed The rotation speed when dragging the gizmo.
    /// @return True if the camera was modified, false otherwise.
    bool Rotate(vec3_t &cameraPos, quat_t &cameraRot, const vec3_t &pivot, ImVec2 position, float rotationSpeed = 0.01f);

    /// @brief Renders a zoom button and handles its logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (used for direction).
    /// @param position The top-left screen position of the button.
    /// @param zoomSpeed The speed/sensitivity of the zoom.
    /// @return True if the camera was modified, false otherwise.
    bool Dolly(vec3_t &cameraPos, const quat_t &cameraRot, ImVec2 position, float zoomSpeed = 0.05f);

    /// @brief Renders a pan button and handles its logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (used for direction).
    /// @param position The top-left screen position of the button.
    /// @param panSpeed The speed/sensitivity of the pan.
    /// @return True if the camera was modified, false otherwise.
    bool Pan(vec3_t &cameraPos, const quat_t &cameraRot, ImVec2 position, float panSpeed = 0.01f);

    /// @brief Renders and handles an object transform gizmo.
    /// @param viewProjection Matrix used to project world positions into the viewport.
    /// @param viewportMin Top-left screen position of the rendered viewport image.
    /// @param viewportSize Size of the rendered viewport image.
    /// @param operation Transform operation to display and edit.
    /// @param position Object/gizmo world position.
    /// @param rotation Object rotation.
    /// @param scale Object scale.
    /// @return True if the object transform was modified, false otherwise.
    bool Transform(
        const mat4_t &viewProjection,
        ImVec2 viewportMin,
        ImVec2 viewportSize,
        TransformOperation operation,
        vec3_t &position,
        quat_t &rotation,
        vec3_t &scale);

    bool Transform(
        const mat4_t &viewProjection,
        ImVec2 viewportMin,
        ImVec2 viewportSize,
        TransformOperation operation,
        vec3_t &position,
        quat_t &rotation,
        vec3_t &scale,
        const quat_t &orientation);
}

#ifdef IMVIEWGUIZMO_IMPLEMENTATION
namespace ImViewGuizmo {
    struct TransformAxisState {
        vec3_t worldAxis;
        ImVec2 endScreen{0.0f, 0.0f};
        ImVec2 screenDirection{0.0f, 0.0f};
        float worldPerPixel = 0.0f;
        bool visible = false;
    };

    static constexpr int TRANSFORM_RING_SEGMENTS = 64;

    struct TransformRingState {
        std::array<ImVec2, TRANSFORM_RING_SEGMENTS + 1> points{};
        std::array<bool, TRANSFORM_RING_SEGMENTS + 1> visiblePoints{};
        ImVec2 tangent{0.0f, 0.0f};
        float distance = FLT_MAX;
        bool visible = false;
    };

    inline bool ProjectTransformPoint(
        const mat4_t &viewProjection,
        const vec3_t &worldPosition,
        const ImVec2 viewportMin,
        const ImVec2 viewportSize,
        ImVec2 &screenPosition) {
        const vec4_t clip = GizmoMath::multiply_mv4(
            viewProjection,
            GizmoMath::make_vec4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f));
        if (clip.w <= 0.0001f) {
            return false;
        }

        const float ndcX = clip.x / clip.w;
        const float ndcY = clip.y / clip.w;
        const float ndcZ = clip.z / clip.w;
        if (ndcZ < 0.0f || ndcZ > 1.0f) {
            return false;
        }

        screenPosition = {
            viewportMin.x + (ndcX * 0.5f + 0.5f) * viewportSize.x,
            viewportMin.y + (ndcY * 0.5f + 0.5f) * viewportSize.y
        };
        return true;
    }

    inline ImVec2 NormalizeScreenVector(const ImVec2 vector) {
        const float length = sqrtf(vector.x * vector.x + vector.y * vector.y);
        if (length <= 0.0001f) {
            return {0.0f, 0.0f};
        }

        return {vector.x / length, vector.y / length};
    }

    inline float DistanceToSegment(const ImVec2 point, const ImVec2 start, const ImVec2 end) {
        const ImVec2 segment{end.x - start.x, end.y - start.y};
        const ImVec2 relative{point.x - start.x, point.y - start.y};
        const float lengthSquared = segment.x * segment.x + segment.y * segment.y;
        if (lengthSquared <= 0.0001f) {
            const float dx = point.x - start.x;
            const float dy = point.y - start.y;
            return sqrtf(dx * dx + dy * dy);
        }

        const float t = std::clamp((relative.x * segment.x + relative.y * segment.y) / lengthSquared, 0.0f, 1.0f);
        const ImVec2 closest{start.x + segment.x * t, start.y + segment.y * t};
        const float dx = point.x - closest.x;
        const float dy = point.y - closest.y;
        return sqrtf(dx * dx + dy * dy);
    }

    inline void AddTransformArrowHead(
        ImDrawList &drawList,
        const ImVec2 tip,
        const ImVec2 direction,
        const ImU32 color,
        const float size) {
        const ImVec2 normal{-direction.y, direction.x};
        const ImVec2 base{tip.x - direction.x * size, tip.y - direction.y * size};
        drawList.AddTriangleFilled(
            tip,
            {base.x + normal.x * size * 0.42f, base.y + normal.y * size * 0.42f},
            {base.x - normal.x * size * 0.42f, base.y - normal.y * size * 0.42f},
            color
        );
    }

    inline void AddTransformScaleHandle(
        ImDrawList &drawList,
        const ImVec2 center,
        const ImU32 color,
        const float size) {
        const float halfSize = size * 0.5f;
        drawList.AddRectFilled(
            {center.x - halfSize, center.y - halfSize},
            {center.x + halfSize, center.y + halfSize},
            color
        );
    }

    inline float ScreenAngleAround(const ImVec2 center, const ImVec2 point) {
        return atan2f(point.y - center.y, point.x - center.x);
    }

    inline TransformAxisState MakeTransformAxisState(
        const mat4_t &viewProjection,
        const ImVec2 viewportMin,
        const ImVec2 viewportSize,
        const vec3_t &position,
        const vec3_t &axis,
        const ImVec2 originScreen,
        const float desiredScreenLength) {
        TransformAxisState state{};
        state.worldAxis = axis;

        ImVec2 oneUnitScreen{0.0f, 0.0f};
        state.visible = ProjectTransformPoint(
            viewProjection,
            GizmoMath::add_vv(position, axis),
            viewportMin,
            viewportSize,
            oneUnitScreen);
        if (!state.visible) {
            return state;
        }

        const ImVec2 screenVector{oneUnitScreen.x - originScreen.x, oneUnitScreen.y - originScreen.y};
        const float projectedLength = sqrtf(screenVector.x * screenVector.x + screenVector.y * screenVector.y);
        if (projectedLength < 0.0001f) {
            state.visible = false;
            return state;
        }

        state.screenDirection = NormalizeScreenVector(screenVector);
        state.worldPerPixel = 1.0f / projectedLength;
        state.endScreen = {
            originScreen.x + state.screenDirection.x * desiredScreenLength,
            originScreen.y + state.screenDirection.y * desiredScreenLength
        };
        return state;
    }

    inline TransformRingState MakeTransformRingState(
        const mat4_t &viewProjection,
        const ImVec2 viewportMin,
        const ImVec2 viewportSize,
        const vec3_t &position,
        const vec3_t &axis,
        const ImVec2 originScreen,
        const ImVec2 mousePosition,
        const float desiredScreenRadius) {
        TransformRingState state{};

        const vec3_t normal = GizmoMath::normalize(axis);
        const vec3_t helper = fabsf(GizmoMath::dot(normal, axisVectors[1])) < 0.9f ? axisVectors[1] : axisVectors[0];
        vec3_t tangentA = GizmoMath::cross(helper, normal);
        if (GizmoMath::length2(tangentA) <= 0.0001f) {
            tangentA = GizmoMath::cross(axisVectors[2], normal);
        }
        tangentA = GizmoMath::normalize(tangentA);
        const vec3_t tangentB = GizmoMath::normalize(GizmoMath::cross(normal, tangentA));

        float pixelsPerWorld = 0.0f;
        int pixelSamples = 0;
        ImVec2 tangentAScreen{0.0f, 0.0f};
        if (ProjectTransformPoint(viewProjection, GizmoMath::add_vv(position, tangentA), viewportMin, viewportSize, tangentAScreen)) {
            const ImVec2 delta{tangentAScreen.x - originScreen.x, tangentAScreen.y - originScreen.y};
            pixelsPerWorld += sqrtf(delta.x * delta.x + delta.y * delta.y);
            ++pixelSamples;
        }

        ImVec2 tangentBScreen{0.0f, 0.0f};
        if (ProjectTransformPoint(viewProjection, GizmoMath::add_vv(position, tangentB), viewportMin, viewportSize, tangentBScreen)) {
            const ImVec2 delta{tangentBScreen.x - originScreen.x, tangentBScreen.y - originScreen.y};
            pixelsPerWorld += sqrtf(delta.x * delta.x + delta.y * delta.y);
            ++pixelSamples;
        }

        if (pixelSamples == 0 || pixelsPerWorld <= 0.0001f) {
            return state;
        }

        pixelsPerWorld /= static_cast<float>(pixelSamples);
        const float worldRadius = desiredScreenRadius / std::max(1.0f, pixelsPerWorld);
        int visibleSegmentCount = 0;

        for (int i = 0; i <= TRANSFORM_RING_SEGMENTS; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(TRANSFORM_RING_SEGMENTS);
            const float angle = t * 6.28318530718f;
            const vec3_t ringOffset = GizmoMath::add_vv(
                GizmoMath::multiply_vf(tangentA, cosf(angle) * worldRadius),
                GizmoMath::multiply_vf(tangentB, sinf(angle) * worldRadius));

            state.visiblePoints[i] = ProjectTransformPoint(
                viewProjection,
                GizmoMath::add_vv(position, ringOffset),
                viewportMin,
                viewportSize,
                state.points[i]);
        }

        for (int i = 0; i < TRANSFORM_RING_SEGMENTS; ++i) {
            if (!state.visiblePoints[i] || !state.visiblePoints[i + 1]) {
                continue;
            }

            ++visibleSegmentCount;
            const float distance = DistanceToSegment(mousePosition, state.points[i], state.points[i + 1]);
            if (distance < state.distance) {
                state.distance = distance;
                state.tangent = NormalizeScreenVector({
                    state.points[i + 1].x - state.points[i].x,
                    state.points[i + 1].y - state.points[i].y
                });
            }
        }

        state.visible = visibleSegmentCount > 4;
        return state;
    }

    inline bool Transform(
        const mat4_t &viewProjection,
        const ImVec2 viewportMin,
        const ImVec2 viewportSize,
        const TransformOperation operation,
        vec3_t &position,
        quat_t &rotation,
        vec3_t &scale,
        const quat_t &orientation) {
        ImGuiIO &io = ImGui::GetIO();
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        auto &ctx = GetContext();
        const Style &style = GetStyle();
        bool wasModified = false;

        ImVec2 originScreen{0.0f, 0.0f};
        if (!ProjectTransformPoint(viewProjection, position, viewportMin, viewportSize, originScreen)) {
            return false;
        }

        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const bool mouseInViewport =
                io.MousePos.x >= viewportMin.x &&
                io.MousePos.y >= viewportMin.y &&
                io.MousePos.x <= viewportMin.x + viewportSize.x &&
                io.MousePos.y <= viewportMin.y + viewportSize.y;

        std::array<TransformAxisState, 3> axisStates{};
        std::array<TransformRingState, 3> ringStates{};
        for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
            const vec3_t axis = GizmoMath::multiply_qv(orientation, axisVectors[axisIndex]);
            axisStates[axisIndex] = MakeTransformAxisState(
                viewProjection,
                viewportMin,
                viewportSize,
                position,
                axis,
                originScreen,
                style.transformAxisLength * style.scale);

            if (operation == TRANSFORM_ROTATE) {
                ringStates[axisIndex] = MakeTransformRingState(
                    viewProjection,
                    viewportMin,
                    viewportSize,
                    position,
                    axis,
                    originScreen,
                    io.MousePos,
                    style.transformRotateRadius * style.scale);
            }
        }

        if (canInteract && mouseInViewport && ctx.activeTool == TOOL_NONE) {
            if (operation == TRANSFORM_ROTATE) {
                float bestDistance = style.transformPickRadius * style.scale;
                for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
                    if (!ringStates[axisIndex].visible) {
                        continue;
                    }

                    if (ringStates[axisIndex].distance <= bestDistance) {
                        bestDistance = ringStates[axisIndex].distance;
                        ctx.hoveredTransformAxisID = axisIndex;
                    }
                }
            } else {
                for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
                    const auto &state = axisStates[axisIndex];
                    if (state.visible && DistanceToSegment(io.MousePos, originScreen, state.endScreen) <= style.transformPickRadius * style.scale) {
                        ctx.hoveredTransformAxisID = axisIndex;
                    }
                }
            }
        }

        if (canInteract && ctx.activeTool == TOOL_NONE && ctx.hoveredTransformAxisID != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ctx.activeTool = TOOL_TRANSFORM;
            ctx.activeTransformOperation = operation;
            ctx.activeTransformAxisID = ctx.hoveredTransformAxisID;
            ctx.transformDragStartMouse = io.MousePos;
            ctx.transformDragStartTangent = operation == TRANSFORM_ROTATE
                                                ? ringStates[ctx.hoveredTransformAxisID].tangent
                                                : ImVec2{0.0f, 0.0f};
            ctx.transformDragStartAngle = ScreenAngleAround(originScreen, io.MousePos);
            ctx.transformStartPosition = position;
            ctx.transformStartRotation = rotation;
            ctx.transformStartOrientation = orientation;
            ctx.transformStartScale = scale;
            ctx.isAnimating = false;
        }

        if (ctx.activeTool == TOOL_TRANSFORM && ctx.activeTransformAxisID >= 0 && ctx.activeTransformAxisID < 3) {
            const int axisIndex = ctx.activeTransformAxisID;
            const auto &state = axisStates[axisIndex];
            const bool activeTransformVisible = ctx.activeTransformOperation == TRANSFORM_ROTATE
                                                    ? ringStates[axisIndex].visible
                                                    : state.visible;
            if (activeTransformVisible) {
                if (ctx.activeTransformOperation == TRANSFORM_TRANSLATE) {
                    const ImVec2 mouseDelta{
                        io.MousePos.x - ctx.transformDragStartMouse.x,
                        io.MousePos.y - ctx.transformDragStartMouse.y
                    };
                    const float projectedPixels = mouseDelta.x * state.screenDirection.x + mouseDelta.y * state.screenDirection.y;
                    position = GizmoMath::add_vv(
                        ctx.transformStartPosition,
                        GizmoMath::multiply_vf(state.worldAxis, projectedPixels * state.worldPerPixel));
                    wasModified = true;
                } else if (ctx.activeTransformOperation == TRANSFORM_SCALE) {
                    const ImVec2 mouseDelta{
                        io.MousePos.x - ctx.transformDragStartMouse.x,
                        io.MousePos.y - ctx.transformDragStartMouse.y
                    };
                    const float projectedPixels = mouseDelta.x * state.screenDirection.x + mouseDelta.y * state.screenDirection.y;
                    const float scaleDelta = projectedPixels * 0.01f;
                    scale = ctx.transformStartScale;
                    if (axisIndex == 0) scale.x = std::max(0.001f, ctx.transformStartScale.x + scaleDelta);
                    if (axisIndex == 1) scale.y = std::max(0.001f, ctx.transformStartScale.y + scaleDelta);
                    if (axisIndex == 2) scale.z = std::max(0.001f, ctx.transformStartScale.z + scaleDelta);
                    wasModified = true;
                } else if (ctx.activeTransformOperation == TRANSFORM_ROTATE) {
                    const ImVec2 mouseDelta{
                        io.MousePos.x - ctx.transformDragStartMouse.x,
                        io.MousePos.y - ctx.transformDragStartMouse.y
                    };
                    const float radius = std::max(1.0f, style.transformRotateRadius * style.scale);
                    const float angleDelta = (mouseDelta.x * ctx.transformDragStartTangent.x + mouseDelta.y * ctx.transformDragStartTangent.y) / radius;
                    const vec3_t axis = GizmoMath::multiply_qv(ctx.transformStartOrientation, axisVectors[axisIndex]);
                    rotation = GizmoMath::multiply_qq(GizmoMath::angleAxis(angleDelta, axis), ctx.transformStartRotation);
                    wasModified = true;
                }

                ImGui::SetNextFrameWantCaptureMouse(true);
                ImGui::SetMouseCursor(operation == TRANSFORM_ROTATE ? ImGuiMouseCursor_ResizeAll : ImGuiMouseCursor_ResizeAll);
            }
        } else if (ctx.hoveredTransformAxisID != -1) {
            ImGui::SetNextFrameWantCaptureMouse(true);
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }

        const float centerRadius = style.transformCenterRadius * style.scale;
        drawList->AddCircleFilled(originScreen, centerRadius, IM_COL32(245, 245, 245, 255));
        drawList->AddCircle(originScreen, centerRadius, IM_COL32(20, 20, 20, 200), 16, 1.5f * style.scale);

        if (operation == TRANSFORM_ROTATE) {
            for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
                const auto &ring = ringStates[axisIndex];
                if (!ring.visible) {
                    continue;
                }

                const bool active = ctx.activeTool == TOOL_TRANSFORM && ctx.activeTransformAxisID == axisIndex;
                const bool hovered = ctx.hoveredTransformAxisID == axisIndex;
                const ImU32 color = active || hovered ? IM_COL32(255, 255, 255, 255) : style.axisColors[axisIndex];
                const float width = active || hovered ? style.transformActiveLineWidth * style.scale : style.transformLineWidth * style.scale;
                for (int segmentIndex = 0; segmentIndex < TRANSFORM_RING_SEGMENTS; ++segmentIndex) {
                    if (ring.visiblePoints[segmentIndex] && ring.visiblePoints[segmentIndex + 1]) {
                        drawList->AddLine(ring.points[segmentIndex], ring.points[segmentIndex + 1], color, width);
                    }
                }
            }
        } else {
            for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
                const auto &state = axisStates[axisIndex];
                if (!state.visible) {
                    continue;
                }

                const bool active = ctx.activeTool == TOOL_TRANSFORM && ctx.activeTransformAxisID == axisIndex;
                const bool hovered = ctx.hoveredTransformAxisID == axisIndex;
                const ImU32 color = active || hovered ? IM_COL32(255, 255, 255, 255) : style.axisColors[axisIndex];
                const float width = active || hovered ? style.transformActiveLineWidth * style.scale : style.transformLineWidth * style.scale;

                drawList->AddLine(originScreen, state.endScreen, color, width);
                if (operation == TRANSFORM_SCALE) {
                    AddTransformScaleHandle(*drawList, state.endScreen, color, style.transformScaleHandleSize * style.scale);
                } else {
                    AddTransformArrowHead(*drawList, state.endScreen, state.screenDirection, color, style.transformArrowSize * style.scale);
                }
            }
        }

        if (!io.MouseDown[0] && ctx.activeTool == TOOL_TRANSFORM) {
            ctx.activeTool = TOOL_NONE;
            ctx.activeTransformAxisID = -1;
        }

        return wasModified;
    }

    inline bool Transform(
        const mat4_t &viewProjection,
        const ImVec2 viewportMin,
        const ImVec2 viewportSize,
        const TransformOperation operation,
        vec3_t &position,
        quat_t &rotation,
        vec3_t &scale) {
        return Transform(viewProjection, viewportMin, viewportSize, operation, position, rotation, scale, rotation);
    }

    /// @brief Renders and handles the view gizmo logic.
    inline bool Rotate(vec3_t &cameraPos, quat_t &cameraRot, const vec3_t &pivot, ImVec2 position, float rotationSpeed) {
        auto &io = ImGui::GetIO();
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        auto &ctx = GetContext();
        auto &style = GetStyle();
        bool wasModified = false;

        // Animation
        if (ctx.isAnimating) {
            float elapsedTime = static_cast<float>(ImGui::GetTime()) - ctx.animationStartTime;
            float t = std::min(1.0f, elapsedTime / style.snapAnimationDuration);
            t = 1.0f - (1.0f - t) * (1.0f - t); // ease-out quad

            vec3_t currentDir = GizmoMath::normalize(GizmoMath::mix(ctx.animStartDir, ctx.animTargetDir, t));
            float currentDistance = mix(ctx.animStartDist, ctx.animTargetDist, t);
            cameraPos = GizmoMath::add_vv(pivot, GizmoMath::multiply_vf(currentDir, currentDistance));

            vec3_t currentUp = GizmoMath::normalize(GizmoMath::mix(ctx.startUp, ctx.targetUp, t));
            cameraRot = GizmoMath::quatLookAt(currentDir, currentUp);

            wasModified = true;

            if (t >= 1.0f) {
                cameraPos = ctx.targetPos;
                cameraRot = GizmoMath::quatLookAt(ctx.animTargetDir, ctx.targetUp);
                ctx.isAnimating = false;
            }
        }

        // Gizmo sizes
        const float gizmoDiameter = 256.f * style.scale;
        const float halfGizmoSize = gizmoDiameter / 2.f;
        const float scaledCircleRadius = style.circleRadius * style.scale;
        const float scaledBigCircleRadius = style.bigCircleRadius * style.scale;
        const float scaledLineWidth = style.lineWidth * style.scale;
        const float scaledHighlightWidth = style.highlightWidth * style.scale;
        const float scaledHighlightRadius = (style.circleRadius + 2.0f) * style.scale;
        const float scaledFontSize = ImGui::GetFontSize() * style.scale * style.labelSize;

        // Gizmo view matrix (transpose of rotation)
        mat4_t rotMat4 = GizmoMath::mat4_cast(cameraRot);

        vec3_t rcol0 = GizmoMath::get_matrix_col(rotMat4, 0);
        vec3_t rcol1 = GizmoMath::get_matrix_col(rotMat4, 1);
        vec3_t rcol2 = GizmoMath::get_matrix_col(rotMat4, 2);

        mat4_t gizmoViewMatrix(1.0f);
        gizmoViewMatrix[0][0] = rcol0.x;
        gizmoViewMatrix[0][1] = rcol1.x;
        gizmoViewMatrix[0][2] = rcol2.x;
        gizmoViewMatrix[0][3] = 0.0f;
        gizmoViewMatrix[1][0] = rcol0.y;
        gizmoViewMatrix[1][1] = rcol1.y;
        gizmoViewMatrix[1][2] = rcol2.y;
        gizmoViewMatrix[1][3] = 0.0f;
        gizmoViewMatrix[2][0] = rcol0.z;
        gizmoViewMatrix[2][1] = rcol1.z;
        gizmoViewMatrix[2][2] = rcol2.z;
        gizmoViewMatrix[2][3] = 0.0f;
        gizmoViewMatrix[3][0] = 0.0f;
        gizmoViewMatrix[3][1] = 0.0f;
        gizmoViewMatrix[3][2] = 0.0f;
        gizmoViewMatrix[3][3] = 1.0f;

        mat4_t gizmoMvp = GizmoMath::multiply_mm(gizmoProjectionMatrix, gizmoViewMatrix);

        // Axes (same ordering/depth calc as glm version)
        std::array<GizmoAxis, 6> axes;

        vec3_t xAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 0);
        vec3_t yAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 1);
        vec3_t zAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 2);

        axes[0] = {0, 0, xAxisInView.z, axisVectors[0]};
        axes[1] = {1, 0, -xAxisInView.z, GizmoMath::multiply_vf(axisVectors[0], -1.0f)};
        axes[2] = {2, 1, yAxisInView.z, axisVectors[1]};
        axes[3] = {3, 1, -yAxisInView.z, GizmoMath::multiply_vf(axisVectors[1], -1.0f)};
        axes[4] = {4, 2, zAxisInView.z, axisVectors[2]};
        axes[5] = {5, 2, -zAxisInView.z, GizmoMath::multiply_vf(axisVectors[2], -1.0f)};

        std::sort(axes.begin(), axes.end(), [](const GizmoAxis &a, const GizmoAxis &b) {
            return a.depth < b.depth;
        });

        // world->screen
        auto worldToScreen = [&](const vec3_t &worldPos) -> ImVec2 {
            const vec4_t clipPos = GizmoMath::multiply_mv4(gizmoMvp, GizmoMath::make_vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f));
            const float w = clipPos.w;
            if (fabsf(w) < 1e-6f)
                return {-FLT_MAX, -FLT_MAX};
            const float ndcX = clipPos.x / w;
            const float ndcY = clipPos.y / w;
            return {position.x + ndcX * halfGizmoSize, position.y - ndcY * halfGizmoSize};
        };

        const ImVec2 originScreenPos = worldToScreen(origin);

        // Hover detection
        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        if (canInteract && ctx.activeTool == TOOL_NONE && !ctx.isAnimating) {
            ImVec2 mousePos = io.MousePos;
            float distToCenterSq = ImLengthSqr(ImVec2(mousePos.x - position.x, mousePos.y - position.y));

            if (distToCenterSq < (halfGizmoSize + scaledCircleRadius) * (halfGizmoSize + scaledCircleRadius)) {
                const float minDistanceSq = scaledCircleRadius * scaledCircleRadius;
                for (const auto &axis: axes) {
                    if (axis.depth < -0.1f)
                        continue;

                    ImVec2 handlePos = worldToScreen(GizmoMath::multiply_vf(axis.direction, style.lineLength));
                    if (ImLengthSqr(ImVec2(handlePos.x - mousePos.x, handlePos.y - mousePos.y)) < minDistanceSq)
                        ctx.hoveredAxisID = axis.id;
                }
                if (ctx.hoveredAxisID == -1 &&
                    ImLengthSqr(ImVec2(originScreenPos.x - mousePos.x, originScreenPos.y - mousePos.y)) < scaledBigCircleRadius * scaledBigCircleRadius)
                    ctx.hoveredAxisID = 6;
            }
        }

        // Drawing
        if (ctx.hoveredAxisID == 6 || ctx.activeTool == TOOL_GIZMO)
            drawList->AddCircleFilled(originScreenPos, scaledBigCircleRadius, style.bigCircleColor);

        ImFont *font = ImGui::GetFont();
        for (const auto &axis: axes) {
            float colorFactor = mix(style.fadeFactor, 1.0f, (axis.depth + 1.0f) * 0.5f);
            ImVec4 baseColor = ImGui::ColorConvertU32ToFloat4(style.axisColors[axis.axisIndex]);
            baseColor.w *= colorFactor;
            ImU32 final_color = ImGui::ColorConvertFloat4ToU32(baseColor);

            const ImVec2 handlePos = worldToScreen(GizmoMath::multiply_vf(axis.direction, style.lineLength));

            ImVec2 lineDir = {handlePos.x - originScreenPos.x, handlePos.y - originScreenPos.y};
            float lineLengthVal = sqrtf(lineDir.x * lineDir.x + lineDir.y * lineDir.y) + 1e-6f;
            lineDir.x /= lineLengthVal;
            lineDir.y /= lineLengthVal;
            ImVec2 lineEndPos = {handlePos.x - lineDir.x * scaledCircleRadius, handlePos.y - lineDir.y * scaledCircleRadius};

            drawList->AddLine(originScreenPos, lineEndPos, final_color, scaledLineWidth);
            drawList->AddCircleFilled(handlePos, scaledCircleRadius, final_color);

            if (ctx.hoveredAxisID == axis.id)
                drawList->AddCircle(handlePos, scaledHighlightRadius, style.highlightColor, 0, scaledHighlightWidth);

            float textFactor = std::max(0.0f, std::min(1.0f, 1.0f + axis.depth * 2.5f));
            if (textFactor > 0.01f) {
                ImVec4 textColor = ImGui::ColorConvertU32ToFloat4(style.labelColor);
                textColor.w *= textFactor;
                const char *label = style.axisLabels[axis.axisIndex];
                ImVec2 textSize = font->CalcTextSizeA(scaledFontSize, FLT_MAX, 0.f, label);
                drawList->AddText(font, scaledFontSize, {handlePos.x - textSize.x * 0.5f, handlePos.y - textSize.y * 0.5f}, ImGui::ColorConvertFloat4ToU32(textColor), label);
            }
        }

        // Drag start
        if (canInteract && io.MouseDown[0] && ctx.activeTool == TOOL_NONE && ctx.hoveredAxisID == 6) {
            ctx.activeTool = TOOL_GIZMO;
            ctx.isAnimating = false;
        }

        // Active tool rotation
        if (ctx.activeTool == TOOL_GIZMO) {
            float yawAngle = -io.MouseDelta.x * rotationSpeed;
            float pitchAngle = -io.MouseDelta.y * rotationSpeed;

            quat_t yawRotation = GizmoMath::angleAxis(yawAngle, worldUp);
            vec3_t rightAxis = GizmoMath::multiply_qv(cameraRot, worldRight);
            quat_t pitchRotation = GizmoMath::angleAxis(pitchAngle, rightAxis);
            quat_t totalRotation = GizmoMath::multiply_qq(yawRotation, pitchRotation);

            vec3_t relativeCamPos = GizmoMath::subtract_vv(cameraPos, pivot);
            cameraPos = GizmoMath::add_vv(pivot, GizmoMath::multiply_qv(totalRotation, relativeCamPos));
            cameraRot = GizmoMath::multiply_qq(totalRotation, cameraRot);

            wasModified = true;
        }

        // Snap on release
        if (canInteract && ImGui::IsMouseReleased(0) && ctx.hoveredAxisID >= 0 && ctx.hoveredAxisID <= 5 && ctx.activeTool == TOOL_NONE) {
            int axisIndex = ctx.hoveredAxisID / 2;
            float sign = (ctx.hoveredAxisID % 2 == 0) ? -1.0f : 1.0f;
            vec3_t targetDir = GizmoMath::multiply_vf(axisVectors[axisIndex], sign);

            float currentDistance = GizmoMath::length(GizmoMath::subtract_vv(cameraPos, pivot));
            vec3_t targetPosition = GizmoMath::add_vv(pivot, GizmoMath::multiply_vf(targetDir, currentDistance));

            vec3_t dirNormalized = GizmoMath::normalize(targetDir);

            vec3_t targetUp = -worldUp;
            // If dir is nearly parallel to worldUp, pick a different up to avoid flipping
            if (fabsf(GizmoMath::dot(dirNormalized, targetUp)) > 0.999f)
                if (dirNormalized.y > 0.0f) // facing "up"
                    targetUp = worldForward;
                else // facing "down"
                    targetUp = -worldForward;

            quat_t targetRotation = GizmoMath::quatLookAt(targetDir, targetUp);

            if (style.animateSnap && style.snapAnimationDuration > 0.0f) {
                bool pos_is_different = GizmoMath::length2(GizmoMath::subtract_vv(cameraPos, targetPosition)) > 0.0001f;
                bool rot_is_different = (1.0f - fabsf(GizmoMath::dot(cameraRot, targetRotation))) > 0.0001f;

                if (pos_is_different || rot_is_different) {
                    ctx.isAnimating = true;
                    ctx.animationStartTime = static_cast<float>(ImGui::GetTime());
                    ctx.startPos = cameraPos;
                    ctx.targetPos = targetPosition;
                    ctx.startUp = GizmoMath::multiply_qv(cameraRot, GizmoMath::multiply_vf(worldUp, -1.0f));
                    ctx.targetUp = targetUp;

                    ctx.animStartDist = GizmoMath::length(GizmoMath::subtract_vv(ctx.startPos, pivot));
                    ctx.animTargetDist = GizmoMath::length(GizmoMath::subtract_vv(ctx.targetPos, pivot));

                    if (ctx.animStartDist > 0.0001f)
                        ctx.animStartDir = GizmoMath::normalize(GizmoMath::subtract_vv(ctx.startPos, pivot));
                    else
                        ctx.animStartDir = worldForward;

                    if (ctx.animTargetDist > 0.0001f)
                        ctx.animTargetDir = GizmoMath::normalize(GizmoMath::subtract_vv(ctx.targetPos, pivot));
                }
            } else {
                cameraRot = targetRotation;
                cameraPos = targetPosition;
                wasModified = true;
            }
        }

        if (!io.MouseDown[0] && ctx.activeTool != TOOL_NONE)
            ctx.activeTool = TOOL_NONE;

        return wasModified;
    }

    /// @brief Renders a zoom button and handles its logic.
    inline bool Dolly(vec3_t &cameraPos, const quat_t &cameraRot, const ImVec2 position, const float zoomSpeed) {
        const ImGuiIO &io = ImGui::GetIO();
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        auto &ctx = GetContext();
        const Style &style = GetStyle();
        bool wasModified = false;

        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const float radius = style.toolButtonRadius * style.scale;
        const ImVec2 center = {position.x + radius, position.y + radius};

        bool isHovered = false;
        if (canInteract && (ctx.activeTool == TOOL_NONE || ctx.activeTool == TOOL_DOLLY)) {
            if (ImLengthSqr({io.MousePos.x - center.x, io.MousePos.y - center.y}) < radius * radius) {
                isHovered = true;
            }
        }
        ctx.isZoomButtonHovered = isHovered;

        if (canInteract && (isHovered || ctx.activeTool == TOOL_DOLLY)) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        }

        if (canInteract && isHovered && io.MouseDown[0] && ctx.activeTool == TOOL_NONE) {
            ctx.activeTool = TOOL_DOLLY;
            ctx.isAnimating = false;
        }

        if (ctx.activeTool == TOOL_DOLLY && io.MouseDelta.y != 0.0f) {
            vec3_t forwardMovement = GizmoMath::multiply_vf(
                GizmoMath::multiply_qv(cameraRot, worldForward),
                -io.MouseDelta.y * zoomSpeed
            );
            cameraPos = GizmoMath::add_vv(cameraPos, forwardMovement);
            wasModified = true;
        }

        const ImU32 bgColor = (ctx.activeTool == TOOL_DOLLY || isHovered) ? style.toolButtonHoveredColor : style.toolButtonColor;
        drawList->AddCircleFilled(center, radius, bgColor);

        const float p = style.toolButtonInnerPadding * style.scale;
        const float th = 2.0f * style.scale;
        const ImU32 iconColor = style.toolButtonIconColor;
        constexpr float iconScale = 0.5f;
        const float scaledP = p * iconScale;
        const float scaledRadius = radius * iconScale;

        ImVec2 glassCenter = {center.x - scaledP / 2.0f, center.y - scaledP / 2.0f};
        const float glassRadius = scaledRadius - scaledP;
        drawList->AddCircle(glassCenter, glassRadius, iconColor, 0, th);

        const ImVec2 handleStart = {center.x + scaledRadius / 2.0f, center.y + scaledRadius / 2.0f};
        const ImVec2 handleEnd = {center.x + scaledRadius - (p * iconScale), center.y + scaledRadius - (p * iconScale)};
        drawList->AddLine(handleStart, handleEnd, iconColor, th);

        const float plusHalfSize = glassRadius * 0.5f;
        drawList->AddLine({glassCenter.x, glassCenter.y - plusHalfSize}, {glassCenter.x, glassCenter.y + plusHalfSize}, iconColor, th);
        drawList->AddLine({glassCenter.x - plusHalfSize, glassCenter.y}, {glassCenter.x + plusHalfSize, glassCenter.y}, iconColor, th);

        return wasModified;
    }

    /// @brief Renders a pan button and handles its logic.
    inline bool Pan(vec3_t &cameraPos, const quat_t &cameraRot, const ImVec2 position, const float panSpeed) {
        const ImGuiIO &io = ImGui::GetIO();
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        auto &ctx = GetContext();
        const Style &style = GetStyle();
        bool wasModified = false;

        const vec3_t worldRight = GizmoMath::make_vec3(1.0f, 0.0f, 0.0f);
        const vec3_t worldUp = GizmoMath::make_vec3(0.0f, -1.0f, 0.0f);
        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const float radius = style.toolButtonRadius * style.scale;
        const ImVec2 center = {position.x + radius, position.y + radius};

        bool isHovered = false;
        if (canInteract && (ctx.activeTool == TOOL_NONE || ctx.activeTool == TOOL_PAN)) {
            if (ImLengthSqr({io.MousePos.x - center.x, io.MousePos.y - center.y}) < radius * radius) {
                isHovered = true;
            }
        }
        ctx.isPanButtonHovered = isHovered;

        if (canInteract && (isHovered || ctx.activeTool == TOOL_PAN)) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }

        if (canInteract && isHovered && io.MouseDown[0] && ctx.activeTool == TOOL_NONE) {
            ctx.activeTool = TOOL_PAN;
            ctx.isAnimating = false;
        }

        if (ctx.activeTool == TOOL_PAN && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)) {
            vec3_t rightMovement = GizmoMath::multiply_vf(GizmoMath::multiply_qv(cameraRot, worldRight), -io.MouseDelta.x * panSpeed);
            vec3_t upMovement = GizmoMath::multiply_vf(GizmoMath::multiply_qv(cameraRot, worldUp), io.MouseDelta.y * panSpeed);
            cameraPos = GizmoMath::add_vv(cameraPos, rightMovement);
            cameraPos = GizmoMath::add_vv(cameraPos, upMovement);
            wasModified = true;
        }

        const ImU32 bgColor = (isHovered || ctx.activeTool == TOOL_PAN) ? style.toolButtonHoveredColor : style.toolButtonColor;
        drawList->AddCircleFilled(center, radius, bgColor);

        const ImU32 iconColor = style.toolButtonIconColor;
        const float th = 2.0f * style.scale;
        const float size = radius * 0.5f;
        const float arm = size * 0.25f;

        const ImVec2 topTip = {center.x, center.y - size};
        drawList->AddLine({topTip.x - arm, topTip.y + arm}, topTip, iconColor, th);
        drawList->AddLine({topTip.x + arm, topTip.y + arm}, topTip, iconColor, th);
        const ImVec2 botTip = {center.x, center.y + size};
        drawList->AddLine({botTip.x - arm, botTip.y - arm}, botTip, iconColor, th);
        drawList->AddLine({botTip.x + arm, botTip.y - arm}, botTip, iconColor, th);
        const ImVec2 leftTip = {center.x - size, center.y};
        drawList->AddLine({leftTip.x + arm, leftTip.y - arm}, leftTip, iconColor, th);
        drawList->AddLine({leftTip.x + arm, leftTip.y + arm}, leftTip, iconColor, th);
        const ImVec2 rightTip = {center.x + size, center.y};
        drawList->AddLine({rightTip.x - arm, rightTip.y - arm}, rightTip, iconColor, th);
        drawList->AddLine({rightTip.x - arm, rightTip.y + arm}, rightTip, iconColor, th);

        return wasModified;
    }
} // namespace ImViewGuizmo
#endif // IMVIEWGUIZMO_IMPLEMENTATION
