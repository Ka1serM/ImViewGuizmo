# ImViewGuizmo

An **immediate-mode 3D view gizmo** for [Dear ImGui](https://github.com/ocornut/imgui), inspired by the navigation widgets in **Blender** and **Godot**.  
It provides a set of interactive viewport widgets to intuitively control your camera in 3D scenes.

<p float="left">
  <img src="https://github.com/user-attachments/assets/44271b01-fe6b-4b21-ac45-6299eacb3e44" height="300" />
  <img src="https://github.com/user-attachments/assets/41afbe61-4ecf-4b87-9927-8f03db4559a6" height="300" />
</p>

## ✨ Features
- Immediate-mode API following Dear ImGui principles.  
- Three built-in widgets for camera control:
  - **Rotate** – click and drag the gizmo to orbit the camera around a pivot.  
  - **Dolly** – zoom the camera in/out with a dedicated button and drag interaction.  
  - **Pan** – move the camera laterally using a pan widget.  
- Axis-aligned snapping with **smooth animation** for precise orientation.  
- Fully customizable styles: colors, sizes, labels, highlight effects, and animation settings.  
- Single-header implementation with minimal dependencies.

## 🚀 Usage

These examples assume you have a camera with `position` (glm::vec3 or your math library equivalent) and `rotation` (glm::quat or equivalent).  

### Rotate

Renders the main orbit gizmo. Drag to rotate the camera or click an axis to snap to a predefined view.

```c++
if (ImViewGuizmo::Rotate(camera_position, camera_rotation, gizmoPos))
{
    // Camera was modified, update your camera system
}
```

### Zoom

Renders a zoom button. Click and drag vertically to move the camera forward/backward along its local forward axis.

```c++
if (ImViewGuizmo::Zoom(camera_position, camera_rotation, zoomButtonPos))
{
    // Camera was modified
}
```

### Pan

Renders a pan button. Click and drag to move the camera parallel to the view plane.

```c++
if (ImViewGuizmo::Pan(camera_position, camera_rotation, panButtonPos))
{
    // Camera was modified
}
```

### Checking Interaction

You can check whether the gizmo or its tools are being used:

```c++
if (ImViewGuizmo::IsUsing())
    // A gizmo tool is currently active

if (ImViewGuizmo::IsOver())
    // Mouse is hovering the gizmo or any tool button
```

## 🎨 Styling

Customize the appearance of the gizmo via `ImViewGuizmo::GetStyle()`:

```c++
auto& style = ImViewGuizmo::GetStyle();
style.scale = 1.5f;
style.axisColors[0] = IM_COL32(255, 0, 0, 255); // X-axis
style.labelColor = IM_COL32(255, 255, 0, 255);
style.animateSnap = true;
style.snapAnimationDuration = 0.5f;
```

You can adjust line lengths, circle radii, highlight effects, tool button sizes, and more.

## 📦 Installation

ImViewGuizmo is a **single-header library**. Include it like this:

```c++
// In exactly one source file:
#define IMVIEWGUIZMO_IMPLEMENTATION
#include "ImViewGuizmo.h"

// In all other files:
#include "ImViewGuizmo.h"
```

## 🔧 Requirements
- [Dear ImGui](https://github.com/ocornut/imgui)
Optional:
- [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm) - used by default for vector and quaternion math, but you can replace it with your own math library if desired.

Here’s a clearer and more structured explanation of how to use your own math library with **ImViewGuizmo**:  

---

## ⚙️ Using Your Own Math Library

By default, ImViewGuizmo uses **GLM** for all vector, quaternion, and matrix math. However, GLM is **entirely optional**. You can substitute your own math library by providing the types and functions ImViewGuizmo expects.

### Step 1: Define Your Math Types
You need types for:

| Concept          | Default Type    | Your Replacement Example |
|-----------------|----------------|-------------------------|
| 3D vector       | `glm::vec3`    | `MyVec3`               |
| 4D vector       | `glm::vec4`    | `MyVec4`               |
| Quaternion      | `glm::quat`    | `MyQuat`               |
| 4×4 matrix      | `glm::mat4`    | `MyMat4`               |


### Step 2: Provide Required Math Functions
ImViewGuizmo internally uses a set of **basic math functions** for vectors, quaternions, and matrices. You must implement equivalents for your types:

**Vector Operations**
- `length(v)` — magnitude of a vector  
- `length2(v)` — squared magnitude  
- `normalize(v)` — unit vector  
- `dot(a, b)` — dot product  
- `cross(a, b)` — cross product (for vec3)  
- `mix(a, b, t)` — linear interpolation  
- Arithmetic operators: `+`, `-`, `*` (scalar multiplication)

**Quaternion Operations**
- `angleAxis(angle, axis)` — create a quaternion from axis/angle  
- `quatLookAt(direction, up)` — create a rotation quaternion to look in a direction  
- `dot(q1, q2)` — quaternion dot product  
- `*` operator for quaternion × quaternion  
- `*` operator for quaternion × vector  

**Matrix Operations**
- `mat4_identity()` — identity matrix  
- `mat4_cast(quat)` — convert quaternion to rotation matrix  
- `multiply_mm(a, b)` — matrix × matrix  
- `multiply_mv4(matrix, vec4)` — matrix × vector4  
- `transpose(matrix)` — transpose a matrix  
- `get_matrix_col(matrix, index)` — get a column of a matrix  
- `set_matrix_col(matrix, index, vec4)` — set a column of a matrix  

These functions are grouped in the `GizmoMath` namespace internally.

### Step 3: Tell ImViewGuizmo About Your Types
Before including `ImViewGuizmo.h`, define macros to replace the default GLM types:

```c++
#define IMVIEWGUIZMO_VEC3 MyVec3
#define IMVIEWGUIZMO_VEC4 MyVec4
#define IMVIEWGUIZMO_QUAT MyQuat
#define IMVIEWGUIZMO_MAT4 MyMat4

#define IMVIEWGUIZMO_LENGTH(v) MyLength(v)
#define IMVIEWGUIZMO_NORMALIZE(v) MyNormalize(v)
#define IMVIEWGUIZMO_DOT(a,b) MyDot(a,b)
#define IMVIEWGUIZMO_CROSS(a,b) MyCross(a,b)
#define IMVIEWGUIZMO_MIX(a,b,t) MyMix(a,b,t)
#define IMVIEWGUIZMO_ANGLEAXIS(angle,axis) MyAngleAxis(angle,axis)
#define IMVIEWGUIZMO_QUATLOOKAT(dir,up) MyQuatLookAt(dir,up)

#include "ImViewGuizmo.h"
```

## 📜 License

ImViewGuizmo is licensed under the MIT License. See [LICENSE](/LICENSE) for details.
