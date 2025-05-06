# vulkan-demo1
Coding Session 2025

---
## Build

* Release: `cmake -DCMAKE_BUILD_TYPE=Release ..`
* Debug: `cmake -DCMAKE_BUILD_TYPE=Debug ..`

---
## Execution

Muss im `build` ordner ausgeführt werden, weil sich dort die kompilierten shader befinden.

---

## Architecture

### 🧱 Instanz & Oberfläche

#### `VkInstance`
- Einstiegspunkt in die Vulkan-API.
- Der Vulkan-Loader verwendet die Instanz, um passende Treiber und verfügbare Extensions zu finden.

#### `VkSurfaceKHR`
- Repräsentiert das Fenster (z. B. von GLFW), auf das gezeichnet wird.
- Vulkan verwendet die Surface, um gerenderte Bilder an das Window-System zu übergeben.

---

### 🧠 Geräte & Queues

#### `VkPhysicalDevice`
- Reale GPU im System.
- Liefert Infos über die Fähigkeiten der Hardware (z. B. unterstützte Formate und Queues).

#### `VkDevice` (Logical Device)
- Software-Schnittstelle zur ausgewählten GPU.
- Ermöglicht das Erstellen von Ressourcen und das Senden von Befehlen an die GPU.

#### `VkQueue`
- Eine Warteschlange, in die Command Buffers zur Ausführung gelegt werden.
- Deine `graphicsQueue` verarbeitet Zeichenbefehle und Präsentationsbefehle.

---

### 🖼️ Swapchain & Bilder

#### `VkSwapchainKHR`
- Eine Reihe von Bildern, die auf dem Bildschirm angezeigt werden (Double/Triple Buffering).
- Die GPU rendert in diese Bilder, die anschließend präsentiert werden.

#### `VkImage`
- Rohes Speicherobjekt der Swapchain.
- Enthält das tatsächliche Farbbild für den jeweiligen Frame.

#### `VkImageView`
- Ermöglicht den Zugriff auf ein `VkImage`, z. B. als Color Attachment im Render Pass.
- Gibt an, wie das Bild interpretiert werden soll.

---

### 🎨 Render-Infrastruktur

#### `VkRenderPass`
- Beschreibt, wie die Framebuffer-Bilder verwendet werden (Clear, Load, Store).
- Legt Layout-Übergänge und Subpasses fest.

#### `VkFramebuffer`
- Kombiniert ein oder mehrere `VkImageView`s mit einem `VkRenderPass`.
- Stellt das konkrete Zielbild für den aktuellen Frame dar.

---

### 🛠️ Zeichnung & Pipeline

#### `VkShaderModule`
- Kompilierter Shader-Code im SPIR-V-Format.
- Wird in der Pipeline eingebunden und beschreibt die Vertex- und Fragment-Stufen.

#### `VkPipelineLayout`
- Container für Ressourcen wie Descriptor Sets oder Push Constants.
- In unserem Beispiel leer, aber notwendig.

#### `VkPipeline` (Graphics Pipeline)
- Umfassende Beschreibung aller Zeichenparameter:
  - Shader-Stufen
  - Vertex-Eingabe
  - Rasterizer
  - Farbverarbeitung
  - Ziel-Rendertyp
- Muss zum `VkRenderPass` passen.

---

### ✍️ Command Infrastructure

#### `VkCommandPool`
- Speicherverwaltung für `VkCommandBuffer`.
- Muss mit einer bestimmten Queue-Familie assoziiert sein.

#### `VkCommandBuffer`
- Liste von GPU-Befehlen wie `vkCmdDraw`, `vkCmdBindPipeline` usw.
- Wird vor dem Rendern einmalig aufgenommen (recorded).

---

### 🔄 Draw Loop Synchronisierung

#### `VkSemaphore` (imageAvailableSemaphore & renderFinishedSemaphore)
- GPU-Synchronisationsobjekte:
  - `imageAvailableSemaphore`: signalisiert, dass ein Bild zur Verfügung steht.
  - `renderFinishedSemaphore`: signalisiert, dass Rendering abgeschlossen ist und das Bild präsentiert werden kann.

---

### 🔁 Ablauf eines Frames

1. **Acquire Image (`vkAcquireNextImageKHR`)**
   - Holt das nächste freie Swapchain-Image.
   - Wartet auf das `imageAvailableSemaphore`.

2. **Submit Command Buffer (`vkQueueSubmit`)**
   - Führt die aufgenommenen Zeichenbefehle aus.
   - Wartet auf das `imageAvailableSemaphore`.
   - Signalisiert das `renderFinishedSemaphore`.

3. **Präsentieren (`vkQueuePresentKHR`)**
   - Gibt das fertig gerenderte Bild an das Fenster aus.
   - Wartet auf das `renderFinishedSemaphore`.
