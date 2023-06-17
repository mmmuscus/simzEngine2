# Simz Engine 2.0

![Example Screenshot](https://github.com/mmmuscus/simzEngine2/assets/32553610/ebf4cc54-db94-4550-bc06-516375f8ad7f)

This was my Bsc Thesis project, the thesis wasn't written in english, however the abstract needed to be written in both languages, you can see it below.

## Abstract
Games are soft real time systems, with different genres of games having a differing requirenment for the smoothness of the experience. FPS games are one of the main genres which require a lot of performance. Lack of performance can cause competitive disatvantage in multiplayer games, and unfair moments in single player experiences. However one of the main selling poits of games are their graphical capabilities, so FPS games are in the tough spot of trying to achieve high qualities in both graphics and performance.

Another important factor for developing and selling games is their avabaility on multiple platforms. Because of these reasons the Vulkan API is a perfect fit to create an FPS game engine with. Vulkan is a multiplatform API, however a big advantage of it (as opposed to OpenGL another popular multiplatform graphics API), is that it allows for much more control, and thus more performance can be squeezed out of it.

The trade off for this is that creating an application in Vulkan is extremely complicated. But if we create an engine wrapping up all the uneccessary complicated parts of Vulkan suddenly we have a tool on our hands that is both powerful and easy to use.

The goal of this thesis is to create an FPS game engine based on the Vulkan graphics API and describe the proces of its creation. Since the game engine is only a tool and not a product its important to make sure its output is easily editable on the fly as well, so alongisde the game engines’ features we also make sure its flexible enought to render changes to the shown objects in real time.
