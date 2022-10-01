# Scene serialization Specification

Scene serialization uses the [YAML 1.2 spec](http://www.yaml.org/spec/1.2/spec.html)

## Root
```yaml
scene:
  - camera: <Camera>
    # alias 'cam', 'c'

  - shapes:
    # alias 'shape', 'objects', 'object', 'objs', 'obj', 's'
      - <Shape>

  - lights:
    # alias 'light', 'l'
      - <Light>

  - materials:
    # alias 'material', 'mats', 'mat', 'm'
      <Material id>: <Material>

  - environment_tex:
    # alias 'environment', 'env_tex', 'env', 'e'
      <Resource id> # allowed: jpg, png, hdr

resources:
  # Mapping of all resources with an identifier id and its path, 
  # the id is used to refer to them
  # the path can be relative to the scene file or 
  # relative to the application binary or absolute
  <Resource id>: <path>
```

## Camera
```yaml
transform: <Transform>
fov: <float degrees>
near: <float x>0 >
far: <float x>0 >
```

## Shapes
```yaml
!plane
name: "Display name"
transform: <Transform>
material: <Material id> # allowed samplers: color, texture
```
```yaml
!sphere
name: "Display name"
transform: <Transform>
material: <Material id> # allowed samplers: color, texture
```
```yaml
!cube
name: "Display name"
transform: <Transform>
material: <Material id> # allowed samplers: color, texture
```
```yaml
!voxel
name: "Display name"
transform: <Transform>
material: <Material id> # allowed samplers: color, palette
voxel_grid: <Resource id> # allowed: .vox
```

## Light
```yaml
!diractional
color: <Color>
intensity: <float>
direction: <Vector>
```
```yaml
!point
color: <Color>
intensity: <float>
position: <Vector>
```

## Material
```yaml
name: "Display name"
color: <Sampler>
ambient: <float 0-1>
diffuse: <float 0-1>
specular: <float 0-1>
reflection: <float 0-1>
```

## Sampler
```yaml
!color
color: <Color>
```
```yaml
!texture
texture: <Resource id> # allowed: png, jpg, hdr
filter: <Enum> # values: nearest, linear
wrap: <Enum> # values: repeat, mirrored_repeat, clamp
```
```yaml
!palette
palette: <Resource id> # allowed: .vox
```

## Transform
```yaml
position: <Vector>
rotation: <Quaternion>
scale: <Vector>
```

## Color
```yaml
r: <float 0-1>
g: <float 0-1>
b: <float 0-1>

# Same as:
{r: <float 0-1>, g: <float 0-1>, b: <float 0-1>}
```
or
```yaml
- <float 0-1> # r
- <float 0-1> # g
- <float 0-1> # b

# Same as:
[<float 0-1>, <float 0-1>, <float 0-1>]
```
or
```yaml
<binary 3 bytes>

# example: 0x1C00FF
# example: 1835263
```

## Vector
```yaml
x: <float>
y: <float>
z: <float>

# Same as:
{ x: <float>, y: <float>, z: <float> }
```
or
```yaml
- <float> # x
- <float> # y
- <float> # z

# Same as:
[<float>, <float>, <float>]
```
## Quaternion
```yaml
x: <float>
y: <float>
z: <float>
w: <float>

# Same as:
{ x: <float>, y: <float>, z: <float>, w: <float> }
```
or
```yaml
- <float> # x
- <float> # y
- <float> # z
- <float> # w

# Same as:
[<float>, <float>, <float>, <float>]
```