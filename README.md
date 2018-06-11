# Introduction
The artefact produced is a 3D simulation built with DirectX11 and C++. It’s main purpose is to demonstrate various graphics techniques and elements including terrain generation, skeletal animation and advanced graphics techniques.

# Artefact Architecture
When the artefact launches, an instance of the application class is created. From here the application class creates an instance of DX11Instance, which initializes DirectX and holds pointers to important DirectX objects such as the swap chain, device and device context. The application class also creates an instance of ShaderManager, which initializes and holds instances of IShaders, which are classes used to set parameters for a shader and then render polygons with that shader. Finally, the application also creates an instance of an IScene, depending upon what scene is set to be built, which is updated every frame with pointers to DX11Instance and ShaderManager as parameters.
The IScene class holds the specific functionality and objects of each scene, however the base interface requires them all to have a camera and camera frustum, directional light and a texture manager which acts as resource management for textures. It also contains pure virtual functions for initialising, update, input and drawing. In the initalisation function, scene objects are expected to be initalised and textures loaded into the texture manager instance. In the draw function individual objects are expected to load their vertex and index buffers into the device’s input assembler and then render them with a shader through the ShaderManager, which are both made accessible through the functions parameters.

# Data Structures, Algorithms and Techniques
## Circle Hill Terrain Generation
Within the terrain generation scene, the circle hill algorithm is used to generate a realistic heightmap which is then rendered as a terrain. The process starts off by setting all values in the heightmap to 0. It then generates a random point on or near the terrain and generates a random radius for this ‘hill’. It then generates a random height which the centre of the hill is raised to. Then, based on the previously generated radius, it iterates through every point on the heightmap which is within the radius of the centre of the hill and then raises it depending on the point’s distance from the centre of the hill. Given a centerpoint (x1, y1) and a radius r, the height of the hill at the point (x2, y2) is equivalent to:

`centreHeight = r2 - ( ( x2 - x1)2 + (y2-y1)2 )`

This process is then repeated 2000 times to generate a diverse heightmap. The points on the heightmap are then normalized so they all fall within 0 and 1 because the generated height values weren’t clamped. Finally, the points are then scaled by a set amount to give the terrain tall mountains. (Smed, J. 2017)

## Voxel Terrain Rendering
Within the voxel terrain scene, a planet is generated and rendered using voxels. The difference between using standard heightmaps and voxels for terrain is that heightmaps only allow a single height value per location. Meaning with a heightmap it is impossible to have a point on a terrain, such as a cave, below another point on a terrain, such as a hill. To solve this problem, voxels were used to represent terrain as 3D data rather than 2D data. A 3D point within a voxel terrain represents a cube, which is rendered in a similar way to a normal cube.

## Deferred Shading + Shadow Mapping
Within the deferred lighting scene, the scene is lit through using the deferred shading algorithm which is designed to improve efficiency in shaders that require complex lighting. It achieves this by splitting up the rendering into a two stage process. Firstly, the geometry is rendered as normal however the pixel shader no longer does lighting calculations, rather information about that pixel is output to a render texture buffer. The second stage utilities a rewritten lighting shader that is a 2D post-process rather than using 3D objects for lighting calculations. The shader retrieves information about the pixel from the render texture buffers such as colour or normal by using the point sampler. From there it can perform the normal lighting equations using the sampled information (Oosten, J. 2015.). This results in a huge performance boost for scenes with lots of lights since the expensive lighting calculations are only computed once per light per covered pixel.

A similar technique is used for the shadow mapping scene. A two stage process is used, with the first stage rendering the depth buffer information into a render texture through rendering the scene from the point of view of the light. The second stage uses the depth information from the render texture to get the depth of any objects that cast shadows and compare it with the position of the light on a per pixel basis in the pixel shader.  If we find the light is closer to the camera then we light the pixel. If we find the object is closer to the camera then we shadow the pixel. 

## Skeletal Data Structure and Animation
Within the skeleton scene, there is a skeletal data structure being drawn with a mesh humanoid mesh around it whilst also performing a run animation through skeletal animation. The skeleton data structure has a number of vertices which are ‘weighted’ to one or more bones or joints. When the skeleton moves or changes position, so do the vertices ‘weighted’ to them. Some vertices can have more than one weight with weights defining a position relative to the bone or joint as well as a bias factor. The bias factor determines how much control the weight has over a specific vertex, since each vertex can contain more than one weight. All of the vertices weight’s bias must add up to 1.

The positions and orientations of a joint, along with its parent joint are defined in the structure. Except for the root joint, all joints must have a parent joint which is at the top of the hierarchy. If a parent bone were to be moved or rotated, the child bones would move and rotate with it. For example, if the upper arm of a skeleton was to be moved, the lower arm, hand and fingers would move with it, with children joints always being attached to parent joints (Cheng, S. 2017).

The skeleton is built from two MD5 files. The first defines the skeleton’s bind-pose through a list of meshes and their vertices and the second defines frame details such as the position and orientation of joint per frame. However the animation file doesn’t have many animation details for many frames, which means if we tried to display the animations through a keyframe animation system it would appear choppy. To solve this the frames are interpolated between based on the current time between two frames (the frame that's already passed, and the frame thats coming up), resulting in a smooth animation. To find the position of an interpolated joint the following equation is used:

`interpolatedJoint.position = joint0.pos + (interpolation * (joint1.pos - joint0.pos))`

## Terrain Optimisation
Within the terrain LOD scene, a RAW heightmap is read in from file and rendered as terrain. It is a big terrain (1024x1024) therefore various optimisations take place to ensure it runs smoothly. The first optimisation is distance based normal mapping which takes place in the terrains shader. When the shader determines a pixel is far enough away from the camera it switches from using a highly detailed normal map to a less detailed normal map. The reason this is possible is because at that distance the details on the detailed normal map can’t entirely be seen and rendering it is a waste of resources. The depth, or distance from camera, of a pixel is determined by dividing the Z pixel depth by the homogeneous W coordinate as shown below:

`depthValue = input.depthPosition.z / input.depthPosition.w;`

Another optimisation the terrain uses is terrain partitioning, where the terrain is split into cells and rendered individually rather than as a single whole terrain. Additional checks are in place to determine if a cell is within the camera’s view frustum and if it’s not it can be culled, saving rendering resources.

Terrain partitioning also allows for quick determination of the height at any point on the terrain. This is because all other cells that the point is not in can be culled and a triangle-line intersection test with just the triangles in the one cell can determine which triangle the point is at and it’s height.

# Critical Evaluation
The circle hill algorithm was used instead of the diamond-square algorithm and fault-line displacement algorithm for the main reason it produced smoother and more natural looking terrain. The diamond-square algorithm wasn’t used was because the terrain generated had noticeable vertical and horizontal creases, which is a well known issue, that Gavin Miller says is due to “the most significant perturbation taking place in a rectangular grid” (Miller, G. 1986.). The fault-line algorithm had a similar issue, in that the area along the fault-line was unnaturally steep.

When first rendering the voxel terrain there were many performance bottlenecks that need to be solved. These problems stemmed from the fact that there was a huge number of cubes being rendered, 64x64x64 cubes in total, which resulted in <1 frames per second. To solve this a number of changes were made to the voxel terrain. Firstly, in an effort to drastically reduce the number of draw calls the terrain had to to do, the terrain was split up into smaller terrain chunk structures, such as 16x16x16. Like the cubes originally, the terrain chunk had the job of rendering each voxel in the terrain, however it only did it with a single draw call. To achieve this, when the chunk is initialized, every voxel in the chunk is checked to see if it is active and if it is, it’s index is passed to a build function. This build function uses this voxel’s index to calculate it’s world position and uses that to add a copy of the base voxel’s translated vertices to a vertex buffer within the chunk. Effectively, by adding the vertices of each active voxel to a single vertex and index buffer, only a single draw call is needed for all the voxels within that chunk. However the tradeoff is that this additional building needs to happen at initialization and every time a single voxel in the chunk changes state, which could be a performance bottleneck in itself down the line. But it does mean that there is less rendering overhead allowing for more voxels to be rendered. 

In addition to reducing the rendering overhead for voxels, there were a few other easy optimisations that were made. Such as not rendering an individual voxel if it is occluded by neighbouring active voxels, mot rendering chunks that had no active voxels and not rendering chunks that were completely surrounded by other chunks. One further improvement would be to merge faces for neighbour blocks that share the same block type, resulting in less vertices in the chunks vertex buffer that needed to be rendered.

One issue that remained unfixed with the shadow mapping was aliasing, where the shadow maps had jagged edges. This is caused by differing shadow map sampling rates across the scene, with the default way to fix it being increasing the shadow map resolution. However this is can be a big computational cost in memory so a more preferred solution is to use a technique called percentage closer filtering. This technique involves sampling the pixels nearest the border between light and shadow and averaging out the results to get a factor level that can be used to smooth out the border, removing the jagged look (Isidoro, J. 2006.). 

# User Guide
There are multiple scenes within the artefact that can be swapped between. This requires changing an enum in Application/Application.h:14, with each enum having a brief description of what's in each scene by the enum. Within all scenes there is a base set of movement controls for the camera; the arrow keys move and rotate the camera; ‘A’ moves upward and ‘Z’ moves downward; ‘PageUp’ looks up and ‘PageDown’ looks down. In the the terrain scenes ‘F3’ detaches the camera from the ground/skeleton. 

# References
Cheng, S. 2017. Human Skeleton System Animation. https://bib.irb.hr/datoteka/890911.Final_0036473606_56.pdf

Isidoro, J. 2006. Shadow Mapping: GPU-based Tips and Techniques. https://developer.amd.com/wordpress/media/2012/10/Isidoro-ShadowMapping.pdf

Miller, G. 1986. The definition and rendering of terrain maps. https://dl.acm.org/citation.cfm?doid=15886.15890

Oosten, J. 2015. Forward vs Deferred vs Forward+ Rendering with DirectX 11. https://www.3dgep.com/forward-plus/

Smed, J and Hakonen, H. 2017. Algorithms and Networking for Computer Games. https://books.google.co.uk/books?id=vU8nDwAAQBAJ&pg=PA87&lpg=PA87&dq=circle+hill+algorithm+terrain&source=bl&ots=XCr9z6WRL9&sig=OAkb2NL5_LSy7FU9N_1CC4vy4Co&hl=en&sa=X&ved=0ahUKEwj4heWyzeDaAhVDKcAKHVRMBCwQ6AEIODAC#v=onepage&q=circle%20hill%20algorithm%20terrain&f=false



