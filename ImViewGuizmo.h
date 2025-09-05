#pragma once

/*===============================================
ImViewGuizmo Single-Header Library - Copyright (c) Marcel Kazemi

To use, do this in one (and only one) of your C++ files:
#define IMVIEWGUIZMO_IMPLEMENTATION
 #include "ImViewGuizmo.h"

In all other files, just include the header as usual:
#include "ImViewGuizmo.h"

MIT License

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

#define GLM_ENABLE_EXPERIMENTAL
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


namespace ImViewGuizmo {

    using namespace glm;

    // INTERFACE
    
    struct Style {
        float scale = 1.f;
        
        // Axis visuals
        float lineLength       = 0.5f;
        float lineWidth        = 4.0f;
        float circleRadius     = 15.0f;
        float fadeFactor       = 0.25f;

        // Highlight
        ImU32 highlightColor   = IM_COL32(255, 255, 0, 255);
        float highlightWidth   = 2.0f;
        
        // Axis
        ImU32 axisColors[3] = {
            IM_COL32(230, 51, 51, 255),   // X
            IM_COL32(51, 230, 51, 255),   // Y
            IM_COL32(51, 128, 255, 255)   // Z
        };

        // Labels
        float labelSize = 1.0f;
        const char* axisLabels[3] = {"X", "Y", "Z"};
        ImU32 labelColor =  IM_COL32(255, 255, 255, 255);

        //Big Circle
        float bigCircleRadius  = 80.0f;
        ImU32 bigCircleColor = IM_COL32(255, 255, 255, 50);

        // Animation
        bool animateSnap = true;
        float snapAnimationDuration = 0.3f; // in seconds
    };

    inline Style& GetStyle() {
        static Style style;
        return style;
    }

    // Gizmo Axis Struct
    struct GizmoAxis {
        int id;         // 0-5 for (+X,-X,+Y,-Y,+Z,-Z), 6=center
        int axisIndex;  // 0=X, 1=Y, 2=Z
        float depth;    // Screen-space depth
        vec3 direction; // 3D vector
    };

    static constexpr float baseSize = 256.f;
    static constexpr vec3 origin = {0.0f, 0.0f, 0.0f};
    static constexpr vec3 worldRight        = vec3(1.0f,  0.0f,  0.0f); // +X
    static constexpr vec3 worldUp      = vec3(0.0f, -1.0f,  0.0f); // -Y
    static constexpr vec3 worldForward = vec3(0.0f,  0.0f,  1.0f); // +Z
    static constexpr vec3 axisVectors[3] = {{1,0,0}, {0,1,0}, {0,0,1}};

    struct Context {
        int hoveredAxisID = -1;
        bool isDragging = false;
        
        // Animation state
        bool isAnimating = false;
        float animationStartTime = 0.f;
        vec3 startPos;
        vec3 targetPos;
        vec3 startUp;
        vec3 targetUp;

        bool IsHovering() const { return hoveredAxisID != -1; }
        void Reset() { hoveredAxisID = -1; isDragging = false; }
    };

    // Global context
    inline Context& GetContext() {
        static Context ctx;
        return ctx;
    }

    inline bool IsUsing() {
        const Context& ctx = GetContext();
        return ctx.hoveredAxisID != -1 || ctx.isDragging;
    }

    inline bool IsHovering() {
        return GetContext().hoveredAxisID != -1;
    }

    /// @brief Renders and handles the view gizmo logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (will be modified).
    /// @param position The top-left screen position where the gizmo should be rendered.
    /// @param snapDistance The distance the camera will snap to when an axis is clicked.
    /// @param mouseSpeed The rotation speed when dragging the gizmo.
    /// @return True if the camera was modified, false otherwise.
    bool Manipulate(vec3& cameraPos, quat& cameraRot, ImVec2 position, float snapDistance =  5.f, float mouseSpeed = 0.005f);

} // namespace ImViewGuizmo


#ifdef IMVIEWGUIZMO_IMPLEMENTATION
namespace ImViewGuizmo {

    bool Manipulate(vec3& cameraPos, quat& cameraRot, ImVec2 position, float snapDistance, float mouseSpeed)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        Context& ctx = GetContext();
        Style& style = GetStyle();
        bool wasModified = false;

        // Animation logic
        if (ctx.isAnimating) {
            float elapsedTime = static_cast<float>(ImGui::GetTime()) - ctx.animationStartTime;
            float t = std::min(1.0f, elapsedTime / style.snapAnimationDuration);
            // Apply a simple ease-out curve
            t = 1.0f - (1.0f - t) * (1.0f - t);

            // Interpolate position along an arc by nlerping direction and lerping distance.
            // This ensures the camera orbits the origin.
            vec3 currentDir;
            if (length(ctx.startPos) > 0.0001f && length(ctx.targetPos) > 0.0001f) {
                vec3 startDir = normalize(ctx.startPos);
                vec3 targetDir = normalize(ctx.targetPos);
                currentDir = normalize(mix(startDir, targetDir, t));
            } else
                currentDir = normalize(mix(vec3(0,0,1), normalize(ctx.targetPos), t)); // Fallback
            float startDistance = length(ctx.startPos);
            float targetDistance = length(ctx.targetPos);
            float currentDistance = mix(startDistance, targetDistance, t);
            cameraPos = currentDir * currentDistance;
            
            // Interpolate the "up" vector to prevent twisting.
            vec3 currentUp = normalize(mix(ctx.startUp, ctx.targetUp, t));
            // Recalculate the rotation every frame to ensure it's perfectly aimed.
            cameraRot = quatLookAt(currentDir, currentUp);

            wasModified = true;

            if (t >= 1.0f) {
                // Snap to final values to avoid floating point inaccuracies
                cameraPos = ctx.targetPos;
                cameraRot = quatLookAt(normalize(ctx.targetPos), ctx.targetUp);
                ctx.isAnimating = false;
            }
        }
        
        // Pre-calculate all scaled dimensions here for clarity and efficiency.
        const float gizmoDiameter = baseSize * style.scale;
        const float scaledCircleRadius = style.circleRadius * style.scale;
        const float scaledBigCircleRadius = style.bigCircleRadius * style.scale;
        const float scaledLineWidth = style.lineWidth * style.scale;
        const float scaledHighlightWidth = style.highlightWidth * style.scale;
        const float scaledHighlightRadius = (style.circleRadius + 2.0f) * style.scale;
        const float scaledFontSize = ImGui::GetFontSize() * style.scale * style.labelSize;

        // Generate Matrices & Axis Data
        mat4 worldMatrix = translate(mat4(1.0f), cameraPos) * mat4_cast(cameraRot);
        mat4 viewMatrix = inverse(worldMatrix);

        mat4 gizmoViewMatrix = mat4(mat3(viewMatrix));
        mat4 gizmoProjectionMatrix = ortho(1.f, -1.f, -1.f, 1.f, -100.0f, 100.0f);
        mat4 gizmoMvp = gizmoProjectionMatrix * gizmoViewMatrix;

        std::vector<GizmoAxis> axes;
        for (int i = 0; i < 3; ++i) {
            axes.push_back({i * 2, i, (gizmoViewMatrix * vec4(axisVectors[i], 0)).z, axisVectors[i]});
            axes.push_back({i * 2 + 1, i, (gizmoViewMatrix * vec4(-axisVectors[i], 0)).z, -axisVectors[i]});
        }

        std::sort(axes.begin(), axes.end(), [](const GizmoAxis& a, const GizmoAxis& b) {
            return a.depth < b.depth;
        });

        auto worldToScreen = [&](const vec3& worldPos) -> ImVec2 {
            const vec4 clipPos = gizmoMvp * vec4(worldPos, 1.0f);
            if (clipPos.w == 0.0f) return {-FLT_MAX, -FLT_MAX};
            const vec3 ndc = vec3(clipPos) / clipPos.w;
            return {
                position.x + ndc.x * (gizmoDiameter / 2.f),
                position.y - ndc.y * (gizmoDiameter / 2.f)
            };
        };

        // 2D Selection Logic
        ctx.hoveredAxisID = -1;
        // Only update hover state if not actively dragging or animating
        if (!ctx.isDragging && !ctx.isAnimating) {
            const float halfGizmoSize = gizmoDiameter / 2.f;
            ImVec2 mousePos = io.MousePos;
            float distToCenterSq = ImLengthSqr(ImVec2(mousePos.x - position.x, mousePos.y - position.y));

            if (distToCenterSq < (halfGizmoSize + scaledCircleRadius) * (halfGizmoSize + scaledCircleRadius))
            {
                const float minDistanceSq = scaledCircleRadius * scaledCircleRadius;
                for (const auto& axis : axes) {
                    if (axis.depth < -0.1f)
                        continue;
                    ImVec2 handlePos = worldToScreen(axis.direction * style.lineLength);
                    if (ImLengthSqr(ImVec2(handlePos.x - mousePos.x, handlePos.y - mousePos.y)) < minDistanceSq)
                        ctx.hoveredAxisID = axis.id;
                }
                if (ctx.hoveredAxisID == -1) {
                    ImVec2 centerPos = worldToScreen(origin);
                    if (ImLengthSqr(ImVec2(centerPos.x - mousePos.x, centerPos.y - mousePos.y)) < scaledBigCircleRadius * scaledBigCircleRadius)
                        ctx.hoveredAxisID = 6;
                }
            }
        }

        // Draw Geometry
        if (ctx.hoveredAxisID == 6 || ctx.isDragging)
            drawList->AddCircleFilled(worldToScreen(origin), scaledBigCircleRadius, style.bigCircleColor);

        for (const auto& [id, axis_index, depth, direction] : axes) {
            float factor = mix(style.fadeFactor, 1.0f, (depth + 1.0f) * 0.5f);
            ImColor axis_color_im = style.axisColors[axis_index];
            float h, s, v;
            ImGui::ColorConvertRGBtoHSV(axis_color_im.Value.x, axis_color_im.Value.y, axis_color_im.Value.z, h, s, v);
            ImU32 final_color = ImColor::HSV(h, s, v * factor);
            ImVec2 handlePos = worldToScreen(direction * style.lineLength);
            drawList->AddLine(worldToScreen(origin), handlePos, final_color, scaledLineWidth);
            drawList->AddCircleFilled(handlePos, scaledCircleRadius, final_color);
            if (ctx.hoveredAxisID == id)
                drawList->AddCircle(handlePos, scaledHighlightRadius, style.highlightColor, 0, scaledHighlightWidth);
        }

        // Draw Text Overlay
        ImFont* font = ImGui::GetFont();
        for (const auto& axis : axes) {
            if (axis.depth < -0.1f)
                continue;
            ImVec2 textPos = worldToScreen(axis.direction * style.lineLength);
            const char* label = style.axisLabels[axis.axisIndex];
            ImVec2 textSize = font->CalcTextSizeA(scaledFontSize, FLT_MAX, 0.f, label);
            drawList->AddText(font, scaledFontSize,{textPos.x - textSize.x * 0.5f, textPos.y - textSize.y * 0.5f}, style.labelColor, label);
        }

        // Drag logic
        if (ImGui::IsMouseDown(0)) {
            if (!ctx.isDragging && ctx.hoveredAxisID == 6) {
                ctx.isDragging = true;
                ctx.isAnimating = false; // Interrupt animation on drag start
            }
            if (ctx.isDragging) {
                float yawAngle = -io.MouseDelta.x * mouseSpeed;
                float pitchAngle = -io.MouseDelta.y * mouseSpeed;
                quat yawRotation = angleAxis(yawAngle, worldUp);
                vec3 rightAxis = cameraRot * worldRight;
                quat pitchRotation = angleAxis(pitchAngle, rightAxis);
                quat totalRotation = yawRotation * pitchRotation;
                cameraPos = totalRotation * cameraPos;
                cameraRot = totalRotation * cameraRot;
                wasModified = true;
            }
        } else
            ctx.isDragging = false;

        // Snap logic
        if (ImGui::IsMouseReleased(0) && ctx.hoveredAxisID >= 0 && ctx.hoveredAxisID <= 5 && !ImGui::IsMouseDragging(0)) {
            int axisIndex = ctx.hoveredAxisID / 2;
            float sign = (ctx.hoveredAxisID % 2 == 0) ? -1.0f : 1.0f;
            vec3 targetDir = sign * axisVectors[axisIndex];
            vec3 targetPosition = targetDir * snapDistance;

            vec3 up = worldUp; 
            if (axisIndex == 1) up = worldForward;
            vec3 targetUp = -up;
            
            quat targetRotation = quatLookAt(targetDir, targetUp);

            if (style.animateSnap && style.snapAnimationDuration > 0.0f) {
                bool pos_is_different = length2(cameraPos - targetPosition) > 0.0001f;
                bool rot_is_different = (1.0f - abs(dot(cameraRot, targetRotation))) > 0.0001f;

                if (pos_is_different || rot_is_different) {
                    ctx.isAnimating = true;
                    ctx.animationStartTime = static_cast<float>(ImGui::GetTime());
                    ctx.startPos = cameraPos;
                    ctx.targetPos = targetPosition;
                    ctx.startUp = cameraRot * vec3(0.0f, 1.0f, 0.0f); // Get current up vector
                    ctx.targetUp = targetUp;
                }
            } else {
                cameraRot = targetRotation;
                cameraPos = targetPosition;
                wasModified = true;
            }
        }

        return wasModified;
    }

} // namespace ImViewGuizmo
#endif // IMVIEWGUIZMO_IMPLEMENTATION
