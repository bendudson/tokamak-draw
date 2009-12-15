/*****************************************************************
 *
 *****************************************************************/

#ifndef __TOKAMAK_DRAW_H__
#define __TOKAMAK_DRAW_H__

#define PI 3.141592653589793

/* Structure storing a viewpoint */
typedef struct {
  /* POSITION OF CAMERA - SPHERICAL COORDS */
  double theta;   /* Angle of rotation around y axis  (toroidal angle) 0 -> 2pi*/
  double R;       /* Major radius */
  double phi;     /* Poloidal angle -pi/2 -> pi/2 */

  double x, y, z;  /* Location the camera is looking at */

  double cx, cy, cz; /* Camera location in cartesian coords
			(updated AFTER camera moved) */
}TCamera;

typedef struct {
  float r, g, b;
  float alpha;
}TColor;

#endif /* __TOKAMAK_DRAW_H__ */
