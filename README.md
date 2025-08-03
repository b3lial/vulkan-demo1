# vulkan-demo1
Coding Session 2025

---
## Build

* Release: `cmake -DCMAKE_BUILD_TYPE=Release ..`
* Debug: `cmake -DCMAKE_BUILD_TYPE=Debug ..`

---
## Execution

Must be executed in the `build` folder because that’s where the compiled shaders are located.

---

## Architecture

### 🧱 Instance & Surface

#### `VkInstance`
- Entry point into the Vulkan API.
- The Vulkan loader uses the instance to find the appropriate drivers and available extensions.

#### `VkSurfaceKHR`
- Represents the window (e.g., from GLFW) that will be drawn to.
- Vulkan uses the surface to pass rendered images to the window system.

---

### 🧠 Devices & Queues

#### `VkPhysicalDevice`
- The actual GPU in the system.
- Provides information about the hardware’s capabilities (e.g., supported formats and queues).

#### `VkDevice` (Logical Device)
- Software interface to the selected GPU.
- Enables creation of resources and sending commands to the GPU.

#### `VkQueue`
- A queue where command buffers are submitted for execution.
- Your `graphicsQueue` processes draw commands and presentation commands.

---

### 🖼️ Swapchain & Images

#### `VkSwapchainKHR`
- A set of images displayed on the screen (double/triple buffering).
- The GPU renders into these images, which are then presented.

#### `VkImage`
- Raw memory object from the swapchain.
- Contains the actual color image for the current frame.

#### `VkImageView`
- Provides access to a `VkImage`, e.g., as a color attachment in the render pass.
- Specifies how the image should be interpreted.

---

### 🎨 Render Infrastructure

#### `VkRenderPass`
- Describes how the framebuffer images are used (clear, load, store).
- Defines layout transitions and subpasses.

#### `VkFramebuffer`
- Combines one or more `VkImageView`s with a `VkRenderPass`.
- Represents the concrete target image for the current frame.

---

### 🛠️ Drawing & Pipeline

#### `VkShaderModule`
- Compiled shader code in SPIR-V format.
- Used in the pipeline to define the vertex and fragment stages.

#### `VkPipelineLayout`
- Container for resources like descriptor sets or push constants.
- Empty in our example, but still required.

#### `VkPipeline` (Graphics Pipeline)
- Comprehensive description of all drawing parameters:
  - Shader stages  
  - Vertex input  
  - Rasterizer  
  - Color processing  
  - Render target type  
- Must be compatible with the `VkRenderPass`.

---

### ✍️ Command Infrastructure

#### `VkCommandPool`
- Memory management for `VkCommandBuffer`.
- Must be associated with a specific queue family.

#### `VkCommandBuffer`
- List of GPU commands such as `vkCmdDraw`, `vkCmdBindPipeline`, etc.
- Recorded once before rendering.

---

### 🔄 Draw Loop Synchronization

#### `VkSemaphore` (imageAvailableSemaphore & renderFinishedSemaphore)
- GPU synchronization objects:
  - `imageAvailableSemaphore`: signals that an image is available.
  - `renderFinishedSemaphore`: signals that rendering is complete and the image can be presented.

---

### 🔁 Frame Flow

1. **Acquire Image (`vkAcquireNextImageKHR`)**
   - Gets the next available swapchain image.
   - Waits on the `imageAvailableSemaphore`.

2. **Submit Command Buffer (`vkQueueSubmit`)**
   - Executes the recorded draw commands.
   - Waits on the `imageAvailableSemaphore`.
   - Signals the `renderFinishedSemaphore`.

3. **Present (`vkQueuePresentKHR`)**
   - Displays the finished image in the window.
   - Waits on the `renderFinishedSemaphore`.

---

## Notes

 Usage for rendering:

```
  for (const WorldCube::Side& side : cube.getSides()) {
      glm::dvec3 corners[4];
      side.getCorners(corners, cube.getEdgeLength());

      // Upload corners as vertex buffer
      // Use WorldCube::Side::quadIndices as index buffer
      // Draw with vkCmdDrawIndexed(6 indices)
  }
```