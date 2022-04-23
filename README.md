# Render Engine

A simple rendering engine using an OpenGL backend, facilitating simple scene management and an execution model. In very early development.

## System

The system features a backend of a set of data buffers, which are fed into a compute shader stage and composed into a single buffer for rendering. The frontend of the system provides means for adding and removing objects (entities) (composed of model data/indices, a transform, texture coordinates, and a "behavior") to the system in constant time, whose data are fed to the buffers.

Behaviors are inheritable interfaces for code that operates on its owner entity's data, that can be run using the system's internal execution model. "Add-on" interfaces for very basic physics and "sprite" functionality are also present.

### Additional Notes

This is a small project both for personal use and for demonstration of the OpenGL API, and graphics
processing. It may not be maintained regularly.

### License

This software is licensed under the GNU public use license.
