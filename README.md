# maya-ray-intersect
A simple C++ plugin for Autodesk Maya offering a basic ray intersection command and node.

## Overview
rayIntersect is split into two parts: a command and a node. Both take a ray origin, ray direction, and mesh. An infinite ray is constructed from the given origin and direction, and the nearest intersection with the given mesh is provided as worldspace coordinates. 

If no intersection is found, the transform is set to the origin.

 

The `rayIntersect` command requires that you select the origin, direction, and mesh *in that order* and outputs an empty group at the resulting intersection, or the origin if no intersection is found.

The result *is not live*. If either the origin or direction are moved, the command must be run again to recieve an up-to-date intersection.

 
The `rayIntersectNode` node takes worldspace coordinates as its origin and direction input, and a mesh as its collision mesh input. It outputs worldspace coordinates at the nearest intersection with the given mesh, or the origin if no intersection is found.

The resulting coordinates *are live*, and will update in real-time as either the origin or direction objects are moved in the scene.

## Installation
1. Copy the provided files into a directory of your choice
2. Compile with CMake
3. Copy the resulting file into a directory within MAYA_PLUG_IN_PATH
