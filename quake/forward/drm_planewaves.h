/*
 * drm_halfspace.h
 *
 *  Created on: May 14, 2015
 *      Author: eafit
 */

#ifndef DRM_PLANEWAVES_H_
#define DRM_PLANEWAVES_H_

typedef enum {
	SV1 = 0, P1
} pwtype_t;

typedef enum {
	RICK = 0, THST
} fnctype_t;

void    drm_planewaves_init ( int32_t myID, const char *parametersin );
int32_t drm_planewaves_initparameters ( const char *parametersin );
void    PlaneWaves_solver_init( int32_t myID, mesh_t *myMesh, mysolver_t *mySolver);
void    compute_addforce_PlaneWaves ( mesh_t     *myMesh,
                                mysolver_t *mySolver,
                                double      theDeltaT,
                                int         step,
                                fmatrix_t (*theK1)[8], fmatrix_t (*theK2)[8]);

void DRM_ForcesinElement ( mesh_t     *myMesh,
		                   mysolver_t *mySolver,
		                   fmatrix_t (*theK1)[8], fmatrix_t (*theK2)[8],
		                   int *f_nodes, int *e_nodes, int32_t   eindex, double tt, int Nnodes_e, int Nnodes_f );

void   getRicker    ( fvector_t *myDisp, double zp, double t, double Vs );
double Ricker_displ ( double zp, double Ts, double t, double fc, double Vs  );

double time_shift ( double Vs, double Vp ) ;
void   get_reflection_coeff ( double *A1, double *B1, double Vs, double Vp  );
void   Ricker_inclinedPW ( fvector_t *myDisp, double xp, double yp, double zp, double t, double Vs, double Vp  );


#endif /* DRM_PLANEWAVES_H_ */
