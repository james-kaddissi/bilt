# bilt

bilt is a 3D graphics engine written in C using Vulkan. I will be adding to this over time as I learn Vulkan. This is for personal learning, experimenting, and project building.

Here is a brief explanation of the structure of the engine. The directory "test-engine" is the game placeholder. It is an arbitrary game I'm using to test the engine. So this portion of the repository would be replaced with whatever game is being built. The "bilt-engine" directory contains the actual engine. Inside its split into the main layer, and the platform layer. The platform layer provides the platform specific code needed to make the main layer function independent of platform. Development of the engine AND of games on the engine is entirely cross platform.