/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora's mesh-related hexahedron element routines
 *
 *  Copyright (C) 2014--2017 C.P. Camusso.
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include <wasora.h>

#include <math.h>
#include <gsl/gsl_linalg.h>


// --------------------------------------------------------------
// hexahedro de 20 nodos
// --------------------------------------------------------------
int mesh_twenty_node_hexaedron_init(void) {
  
  element_type_t *element_type;
  gauss_t *gauss;

  element_type = &wasora_mesh.element_type[ELEMENT_TYPE_HEXAHEDRON20];
  element_type->name = strdup("hexahedron");
  element_type->id = ELEMENT_TYPE_HEXAHEDRON20;
  element_type->dim = 3;
  element_type->order = 2;
  element_type->nodes = 20;
  element_type->faces = 6;
  element_type->nodes_per_face = 8;
  element_type->h = mesh_twenty_node_hexahedron_h;
  element_type->dhdr = mesh_twenty_node_hexahedron_dhdr;
  element_type->point_in_element = mesh_point_in_hexahedron;
  element_type->element_volume = mesh_hexahedron_vol;

  // dos juegos de puntos de gauss
  element_type->gauss = calloc(2, sizeof(gauss_t));
  
  // el primero es el default
  // ---- ocho puntos de Gauss sobre el elemento unitario ----  
    gauss = &element_type->gauss[GAUSS_POINTS_CANONICAL];
    mesh_alloc_gauss(gauss, element_type, 8);

    gauss->w[0] = 8 * 1.0/8.0;
    gauss->r[0][0] = -1.0/M_SQRT3;
    gauss->r[0][1] = -1.0/M_SQRT3;
    gauss->r[0][2] = -1.0/M_SQRT3;

    gauss->w[1] = 8 * 1.0/8.0;
    gauss->r[1][0] = +1.0/M_SQRT3;
    gauss->r[1][1] = -1.0/M_SQRT3;
    gauss->r[1][2] = -1.0/M_SQRT3;

    gauss->w[2] = 8 * 1.0/8.0;
    gauss->r[2][0] = +1.0/M_SQRT3;
    gauss->r[2][1] = +1.0/M_SQRT3;
    gauss->r[2][2] = -1.0/M_SQRT3;

    gauss->w[3] = 8 * 1.0/8.0;
    gauss->r[3][0] = -1.0/M_SQRT3;
    gauss->r[3][1] = +1.0/M_SQRT3;
    gauss->r[3][2] = -1.0/M_SQRT3;

    gauss->w[4] = 8 * 1.0/8.0;
    gauss->r[4][0] = -1.0/M_SQRT3;
    gauss->r[4][1] = -1.0/M_SQRT3;
    gauss->r[4][2] = +1.0/M_SQRT3;

    gauss->w[5] = 8 * 1.0/8.0;
    gauss->r[5][0] = +1.0/M_SQRT3;
    gauss->r[5][1] = -1.0/M_SQRT3;
    gauss->r[5][2] = +1.0/M_SQRT3;

    gauss->w[6] = 8 * 1.0/8.0;
    gauss->r[6][0] = +1.0/M_SQRT3;
    gauss->r[6][1] = +1.0/M_SQRT3;
    gauss->r[6][2] = +1.0/M_SQRT3;

    gauss->w[7] = 8 * 1.0/8.0;
    gauss->r[7][0] = -1.0/M_SQRT3;
    gauss->r[7][1] = +1.0/M_SQRT3;
    gauss->r[7][2] = +1.0/M_SQRT3;
  
    mesh_init_shape_at_gauss(gauss, element_type);
    
  // ---- un punto de Gauss sobre el elemento unitario ----  
    gauss = &element_type->gauss[GAUSS_POINTS_SINGLE];
    mesh_alloc_gauss(gauss, element_type, 1);
  
    gauss->w[0] = 8 * 1.0;
    gauss->r[0][0] = 0;
    gauss->r[0][1] = 0;

    mesh_init_shape_at_gauss(gauss, element_type);  
  
  return WASORA_RUNTIME_OK;
}
/*
Reference https://www.code-aster.org/V2/doc/v11/en/man_r/r3/r3.01.01.pdf

                                              Coordinates
Node here(gmsh)       Node reference        r        s        t
 0                       1                 -1       -1       -1
 1                       2                  1       -1       -1
 2                       3                  1        1       -1
 3                       4                 -1        1       -1
 4                       5                 -1       -1        1
 5                       6                  1       -1        1
 6                       7                  1        1        1
 7                       8                 -1        1        1
 8                       9                  0       -1       -1
11                      10                  1        0       -1
13                      11                  0        1       -1
 9                      12                 -1        0       -1
10                      13                 -1       -1        0
12                      14                  1       -1        0
14                      15                  1        1        0
15                      16                 -1        1        0
16                      17                  0       -1        1
18                      18                  1        0        1
19                      19                  0        1        1
17                      20                 -1        0        1
*/

double mesh_twenty_node_hexahedron_h(int j, gsl_vector *gsl_r) {
  double r;
  double s;
  double t;

  r = gsl_vector_get(gsl_r, 0);
  s = gsl_vector_get(gsl_r, 1);
  t = gsl_vector_get(gsl_r, 2);

  switch (j) {
    case 0:
      return (1-r)*(1-s)*(1-t)*(-2-r-s-t)/8.0;
      break;
    case 1:
      return (r+1)*(1-s)*(1-t)*(-2+r-s-t)/8.0;
      break;
    case 2:
      return (r+1)*(s+1)*(1-t)*(-2+r+s-t)/8.0;
      break;
    case 3:
      return (1-r)*(s+1)*(1-t)*(-2-r+s-t)/8.0;
      break;
    case 4:
      return (1-r)*(1-s)*(1+t)*(-2-r-s+t)/8.0;
      break;
    case 5:
      return (r+1)*(1-s)*(t+1)*(-2+r-s+t)/8.0;
      break;
    case 6:
      return (r+1)*(1+s)*(t+1)*(-2+r+s+t)/8.0;
      break;
    case 7:
      return (1-r)*(s+1)*(t+1)*(-2-r+s+t)/8.0;
      break;
    case 8:
      return (1-r*r)*(1-s)*(1-t)/4.0;
      break;
    case 9:
      return (1-r)*(1-s*s)*(1-t)/4.0;
      break;
    case 10:
      return (1-r)*(1-s)*(1-t*t)/4.0;
      break;
    case 11:
      return (r+1)*(1-s*s)*(1-t)/4.0;
      break;
    case 12:
      return (r+1)*(1-s)*(1-t*t)/4.0;
      break;
    case 13:
      return (1-r*r)*(s+1)*(1-t)/4.0;
      break;
    case 14:
      return (r+1)*(s+1)*(1-t*t)/4.0;
      break;
    case 15:
      return (1-r)*(s+1)*(1-t*t)/4.0;
      break;
    case 16:
      return (1-r*r)*(1-s)*(t+1)/4.0;
      break;
    case 17:
      return (1-r)*(1-s*s)*(t+1)/4.0;
      break;
    case 18:
      return (r+1)*(1-s*s)*(t+1)/4.0;
      break;
    case 19:
      return (1-r*r)*(s+1)*(t+1)/4.0;
      break;
  }

  return 0;

}

double mesh_twenty_node_hexahedron_dhdr(int j, int m, gsl_vector *gsl_r) {
  double r;
  double s;
  double t;

  r = gsl_vector_get(gsl_r, 0);
  s = gsl_vector_get(gsl_r, 1);
  t = gsl_vector_get(gsl_r, 2);

  switch (j) {
    case 0:
      switch(m) {
        case 0:
          return -0.125*(1-s)*(1-t)*(-t-s-r-2)-0.125*(1-r)*(1-s)*(1-t);
        break;
        case 1:
          return -0.125*(1-r)*(1-t)*(-t-s-r-2)-0.125*(1-r)*(1-s)*(1-t);
        break;
        case 2:
          return -0.125*(1-r)*(1-s)*(-t-s-r-2)-0.125*(1-r)*(1-s)*(1-t);
        break;
      }
    break;
    case 1:
      switch(m) {
        case 0:
          return  0.125*(1-s)*(1-t)*(-t-s+r-2)+0.125*(r+1)*(1-s)*(1-t);
        break;
        case 1:
          return -0.125*(r+1)*(1-t)*(-t-s+r-2)-0.125*(r+1)*(1-s)*(1-t);
        break;
        case 2:
          return -0.125*(r+1)*(1-s)*(-t-s+r-2)-0.125*(r+1)*(1-s)*(1-t);
          return ;
        break;
      }
    break;
    case 2:
      switch(m) {
        case 0:
          return  0.125*(s+1)*(1-t)*(-t+s+r-2)+0.125*(r+1)*(s+1)*(1-t);
        break;
        case 1:
          return  0.125*(r+1)*(1-t)*(-t+s+r-2)+0.125*(r+1)*(s+1)*(1-t);
        break;
        case 2:
          return -0.125*(r+1)*(1+s)*(-t+s+r-2)-0.125*(r+1)*(s+1)*(1-t);
          return ;
        break;
      }
    break;
    case 3:
      switch(m) {
        case 0:
          return -0.125*(s+1)*(1-t)*(-t+s-r-2)-0.125*(1-r)*(s+1)*(1-t);
        break;
        case 1:
          return  0.125*(1-r)*(1-t)*(-t+s-r-2)+0.125*(1-r)*(s+1)*(1-t);
        break;
        case 2:
          return -0.125*(1-r)*(s+1)*(-t+s-r-2)-0.125*(1-r)*(s+1)*(1-t);
        break;
      }
    break;
    case 4:
      switch(m) {
        case 0:
          return -0.125*(1-s)*(t+1)*(t-s-r-2)-0.125*(1-r)*(1-s)*(t+1);
        break;
        case 1:
          return -0.125*(1-r)*(t+1)*(t-s-r-2)-0.125*(1-r)*(1-s)*(t+1);
        break;
        case 2:
          return  0.125*(1-r)*(1-s)*(t-s-r-2)+0.125*(1-r)*(1-s)*(t+1);
        break;
      }
    break;
    case 5:
      switch(m) {
        case 0:
          return  0.125*(1-s)*(t+1)*(t-s+r-2)+0.125*(r+1)*(1-s)*(t+1);
        break;
        case 1:
          return -0.125*(r+1)*(t+1)*(t-s+r-2)-0.125*(r+1)*(1-s)*(t+1);
        break;
        case 2:
          return  0.125*(r+1)*(1-s)*(t-s+r-2)+0.125*(r+1)*(1-s)*(t+1);
        break;
      }
    break;
    case 6:
      switch(m) {
        case 0:
          return 0.125*(s+1)*(t+1)*(t+s+r-2)+0.125*(r+1)*(s+1)*(t+1);
        break;
        case 1:
          return 0.125*(r+1)*(t+1)*(t+s+r-2)+0.125*(r+1)*(s+1)*(t+1);
        break;
        case 2:
          return 0.125*(r+1)*(s+1)*(t+s+r-2)+0.125*(r+1)*(s+1)*(t+1);
        break;
      }
    case 7:
      switch(m) {
        case 0:
          return -0.125*(s+1)*(t+1)*(t+s-r-2)-0.125*(1-r)*(s+1)*(t+1);
        break;
        case 1:
          return  0.125*(1-r)*(t+1)*(t+s-r-2)+0.125*(1-r)*(s+1)*(t+1);
        break;
        case 2:
          return  0.125*(1-r)*(s+1)*(t+s-r-2)+0.125*(1-r)*(s+1)*(t+1);
        break;
      }
    case 8:
      switch(m) {
        case 0:
          return -0.5*r*(1-s)*(1-t);
        break;
        case 1:
          return -(1-r*r)*(1-t)/4.0;
        break;
        case 2:
          return -(1-r*r)*(1-s)/4.0;
        break;
      }
    case 9:
      switch(m) {
        case 0:
          return -(1-s*s)*(1-t)/4.0;
        break;
        case 1:
          return -(1-r)*s*(1-t)/2.0;
        break;
        case 2:
          return -(1-r)*(1-s*s)/4.0;
        break;
      }
    break;
    case 10:
      switch(m) {
        case 0:
          return -(1-s)*(1-t*t)/4.0;
        break;
        case 1:
          return -(1-r)*(1-t*t)/4.0;
        break;
        case 2:
          return -(1-r)*(1-s)*t/2.0;
        break;
      }
    break;
    case 11:
      switch(m) {
        case 0:
          return (1-s*s)*(1-t)/4.0;
        break;
        case 1:
          return -(r+1)*s*(1-t)/2.0;
        break;
        case 2:
          return -(r+1)*(1-s*s)/4.0;
        break;
      }
    break;
    case 12:
      switch(m) {
        case 0:
          return (1-s)*(1-t*t)/4.0;
        break;
        case 1:
          return -(r+1)*(1-t*t)/4.0;
        break;
        case 2:
          return -(r+1)*(1-s)*t/2.0;
        break;
      }
    break;
    case 13:
      switch(m) {
        case 0:
          return -r*(s+1)*(1-t)/2.0;
        break;
        case 1:
          return (1-r*r)*(1-t)/4.0;
        break;
        case 2:
          return -(1-r*r)*(s+1)/4.0;
        break;
      }
    break;
    case 14:
      switch(m) {
        case 0:
          return (s+1)*(1-t*t)/4.0;
        break;
        case 1:
          return (r+1)*(1-t*t)/4.0;
        break;
        case 2:
          return -(r+1)*(s+1)*t/2.0;
        break;
      }
    break;
    case 15:
      switch(m) {
        case 0:
          return -(s+1)*(1-t*t)/4.0;
        break;
        case 1:
          return (1-r)*(1-t*t)/4.0;
        break;
        case 2:
          return -(1-r)*(s+1)*t/2.0;
        break;
      }
    break;
    case 16:
      switch(m) {
        case 0:
          return -r*(1-s)*(t+1)/2.0;
        break;
        case 1:
          return -(1-r*r)*(t+1)/4.0;
        break;
        case 2:
          return (1-r*r)*(1-s)/4.0;
        break;
      }
    break;
    case 17:
      switch(m) {
        case 0:
          return -(1-s*s)*(t+1)/4.0;
        break;
        case 1:
          return -(1-r)*s*(t+1)/2.0;
        break;
        case 2:
          return (1-r)*(1-s*s)/4.0;
        break;
      }
    break;
    case 18:
      switch(m) {
        case 0:
          return (1-s*s)*(t+1)/4.0;
        break;
        case 1:
          return -(r+1)*s*(t+1)/2.0;
        break;
        case 2:
          return (r+1)*(1-s*s)/4.0;
        break;
      }
    break;
    case 19:
      switch(m) {
        case 0:
          return -r*(s+1)*(t+1)/2.0;
        break;
        case 1:
          return (1-r*r)*(t+1)/4.0;
        break;
        case 2:
          return (1-r*r)*(s+1)/4.0;
        break;
      }
    break;
  }

  return 0;


}

