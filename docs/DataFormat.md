# Data Format

Data will be stored in **YAML** file with name `wfc_data.yaml`

## The Format
```yaml
Version: "<Version>"
ImageFolderBasePath: "<Path>"
OBJECTS:
	- ID: "<ID>"
	  ROTATION: [<Rotation Allowed Mask>]
	  MASK: [<Joint Mask>]
	.
	.
	.
```
### Version
* **Format**: "major.minor" + a/b/f
* a, b & f stands for alpha, beta & final
* e.g `1.5f`, `2.0a`, `1.2b`, etc.

### Path
* It should be relative form the path of the program of the folder
**TYPE**: Path

### ID
* It's ID. it's your choice to use an appropriate one an Integer one
**TYPE**: Integer

### Rotation Allowed Mask
* The rotations are in 1/90° unit only integer values are allowed
* e.g: 
```yaml
ROTATION: 15 # 15 = 2^(0°/90°) + 2^(90°/90°) + 2^(180°/90°) + 2^(270°/90°) Can be Rotated in 0°, 90°, 180° & 270° configuration
ROTATION: 5	 # 5 = 2^(0°/90°) + 2^(180°/90°) Can be Rotated in 0° & 180° configuration
ROTATION: 10 # 10 = 2^(90°/90°) + 2^(270°/90°) Can be Rotated in 90° & 270° configuration
ROTATION: 1	 # 1 = 2^(0°/90°) Can only be rotated in 0° configuration
```
**TYPE**: Mask 0 to 15 (Unsigned 16 bit integer)

### Joint Mask
* Mask Id for the four sides in anti-clockwise direction starting from right
**TYPE**: Mask (2^n format) (Unsigned 64 bit integer)
* e.g. 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 ... For unique mask id
* e.g. 3, 5, 6, 7, 9, 10, 11, 12 ... for combined mask id