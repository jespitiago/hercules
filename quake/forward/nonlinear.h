/* -*- C -*- */

/* @copyright_notice_start
 *
 * This file is part of the CMU Hercules ground motion simulator developed
 * by the CMU Quake project.
 *
 * Copyright (C) Carnegie Mellon University. All rights reserved.
 *
 * This program is covered by the terms described in the 'LICENSE.txt' file
 * included with this software package.
 *
 * This program comes WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 'LICENSE.txt' file for more details.
 *
 *  @copyright_notice_end
 */

#ifndef Q_NONLINEAR_H
#define Q_NONLINEAR_H

/* -------------------------------------------------------------------------- */
/*                        Structures and definitions                          */
/* -------------------------------------------------------------------------- */
typedef enum {

    RATE_DEPENDANT = 0, RATE_INDEPENDANT

} plasticitytype_t;


typedef enum {
    /*
     * The Linear option is intended for calculating nonlinear associated
     * entities (e.g., strains, stresses, k, J2) while keeping all elements
     * elastic. No operation is performed at compute_addforce_nl. This option
     * allows one to initially evaluate the levels of deformation and serves
     * for comparisons with the corresponding elastoplastic runs.
     */
    LINEAR = 0, VONMISES_EP, VONMISES_FA, VONMISES_FAM, VONMISES_BAE, VONMISES_BAH, VONMISES_GQH, DRUCKERPRAGER, MOHR_COULOMB

} materialmodel_t;

typedef struct vect1_t {
	double x;
	double y;
	double z;
} vect1_t;


typedef struct qvect_t {
	 vect1_t qf[8];
} qvect_t;


typedef struct vector_t {

	double ep;

} vector_t;


typedef struct tensor_t {

    double xx;
    double yy;
    double zz;
    double xy;
    double yz;
    double xz;

} tensor_t;

typedef struct qpvectors_t {

    double qv[8];

} qpvectors_t;

typedef struct qptensors_t {

    tensor_t qp[8];

} qptensors_t;



typedef struct nlconstants_t {

    double lambda;
    double mu;

    double alpha;        /*  yield function constants in Drucker-Prager model*/
    double beta;         /*  constant of the plastic potential flow law */
    double gamma;        /*  constant in the hardening function in Drucker-Prager model */

    double c;            /* soil cohesion */
    double phi;          /* angle of internal friction */
    double dil_angle;    /* angle of dilatancy */

    double h;             /*  variable used for the isotropic hardening function.
                              vonMises H=0. Hardening is considered Kinematic in vonMises criterion
                              In Drucker-Prager H = gamma(c + h*ep)  */
                          /*  In MohrCoulomb    H =     2(c + h*ep)cos(phi)  */

    double Sstrain0;      /* Defines the elastic range of the vonMises model. Sy=G*Sstrain0   */
    double psi0;          /* Used to define the kinematic modulus in vonMises_FAO and the first loading regime of vonMises_FAM.
                             Also used to define h=psi0*mu  in vonMisesBAE  */

    double m;             /* For vonMisesBAE H=psi0*mu*kappa^m     */

    double thetaGQH[5];      /* GQH  parameters   */


    double fs[8];         /* F(sigma) */
    double dLambda[8];    /* yield control */
    double strainrate;
    double sensitivity;

    double sigmaZ_st;    /* static vertical stress */

    double maxFs;
    double avgFs;

} nlconstants_t;

typedef struct nlsolver_t {

	nlconstants_t *constants;
	qptensors_t   *stresses;
	qptensors_t   *strains;
	qptensors_t   *strains1;      // total strains at t-1
	qptensors_t   *pstrains1;
	qptensors_t   *pstrains2;
	qptensors_t   *alphastress1;
	qptensors_t   *alphastress2;
	qpvectors_t   *ep1;         /* effective plastic strains */
	qpvectors_t   *ep2;

	/* New variables for vonMisesFAM with Sy=0   */
	qpvectors_t   *psi_n;         /* Hkin/mu ratio. Used in the first loading of the vonMoises nonlinear hardening model (vMNH)   */
	qpvectors_t   *LoUnlo_n;      /* loading sign at t-1  */
	qpvectors_t   *Sv_n;          /* virgin surface radius at t-1 */
	qpvectors_t   *Sv_max;        /* Max virgin surface radius */

	/* New variables for Borja & Aimes models  */
	qpvectors_t   *kappa;         /* to define the hardening functions : Exponential. H = psi0*mu*kappa^m
                                                                         Hyperbolic.  H = 3mu * kappa^2 / (1+2*kappa)   */
	qptensors_t   *Sref;          /* Reference stress */

} nlsolver_t;

/*typedef struct nlstation_t {

    tensor_t stress;
    tensor_t strain;
    tensor_t pstrain1;
    tensor_t pstrain2;
    double   ep;

} nlstation_t;*/

typedef struct bottomelement_t {

    int32_t element_id;
    double  nodal_force[4];

} bottomelement_t;

/* -------------------------------------------------------------------------- */
/*                                 Utilities                                  */
/* -------------------------------------------------------------------------- */


double get_geostatic_total_time();


int isThisElementNonLinear(mesh_t *myMesh, int32_t eindex);

double interpolate_property_value(double vsRequest, double *propVector);

double get_cohesion(double vs);
double get_phi(double vs);
double get_dilatancy(double vs);

double get_alpha(double vs, double phi);
double get_beta(double vs);
double get_gamma(double vs, double phi);

double get_hardmod(double vs);


double interp_phi (double vsvp, double li, double lf, double phi_i, double phi_f);
double get_gamma_eff (double vs30, double zo);

/* -------------------------------------------------------------------------- */
/*       Initialization of parameters, structures and memory allocations      */
/* -------------------------------------------------------------------------- */

void nonlinear_init ( int32_t     myID,
                      const char *parametersin,
                      double      theDeltaT,
                      double      theEndT );

int32_t nonlinear_initparameters ( const char *parametersin,
                                   double      theDeltaT,
                                   double      theEndT );

void nonlinear_elements_count(int32_t myID, mesh_t *myMesh);
void nonlinear_elements_mapping(int32_t myID, mesh_t *myMesh);
void bottom_elements_count(int32_t myID, mesh_t *myMesh, double depth);
void bottom_elements_mapping(int32_t myID, mesh_t *myMesh, double depth);
void nonlinear_print_stats(int32_t *nonlinElementsCount,
                           int32_t *nonlinStationsCount,
                           int32_t *bottomElementsCount,
                           int32_t  theGroupSize);

void nonlinear_stats(int32_t myID, int32_t theGroupSize);
void nonlinear_solver_init(int32_t myID, mesh_t *myMesh, double depth);

/* -------------------------------------------------------------------------- */
/*                   Auxiliary tensor manipulation methods                    */
/* -------------------------------------------------------------------------- */

double    tensor_I1(tensor_t tensor);
double    tensor_octahedral(double I1);
tensor_t  tensor_deviator(tensor_t tensor, double oct);
double    tensor_J2(tensor_t dev);
double    tensor_J3(tensor_t dev);
double    combtensor_J2(tensor_t A, tensor_t B);
tensor_t  scaled_tensor(tensor_t A, double lambda);

void point_dxi      ( double *dx, double *dy, double *dz,
                      double  lx, double  ly, double  lz,
                      double h, int i );
void compute_qp_dxi ( double *dx, double *dy, double *dz,
                      int i, int j, double h);

tensor_t init_tensor     ( );
void     init_tensorptr  ( tensor_t *tensor );

tensor_t point_strain    ( fvector_t *u, double lx, double ly, double lz,
                           double h);
tensor_t point_stress    ( tensor_t strain, double mu, double lambda);
tensor_t elastic_strains (tensor_t stress, double mu, double kappa);

tensor_t subtrac_tensors ( tensor_t A, tensor_t B);
tensor_t copy_tensor     ( tensor_t original);
tensor_t add_tensors     (tensor_t A, tensor_t B);
tensor_t zero_tensor     ();
double   ddot_tensors    (tensor_t A, tensor_t B);
tensor_t isotropic_tensor(double lambda);

qptensors_t compute_qp_stresses ( qptensors_t strains, double mu, double lambda);
qptensors_t subtrac_qptensors   ( qptensors_t A, qptensors_t B);

double   compute_hardening           ( double gamma, double c, double Sy, double h, double ep_bar, double phi, double psi );
double   compute_yield_surface_stateII ( double J3, double J2, double I1, double alpha, double phi, tensor_t Sigma );

double   compute_dLambdaII           ( nlconstants_t constants, double fs, double eff_ps, double J2, double I1, double J2_st, double I1_st, double *po);
tensor_t compute_dfds                ( tensor_t dev, double J2, double beta);
tensor_t compute_pstrain2            ( nlconstants_t constants, tensor_t pstrain1, tensor_t tstrain,
							           tensor_t dfds, double dLambda, double dt, double J2, double I1,
							           double J2_st, double I1_st, double po );

void material_update ( nlconstants_t constants, tensor_t e_n, tensor_t e_n1, tensor_t ep, tensor_t eta_n, double ep_barn, tensor_t sigma0, double dt,
		               tensor_t *epl, tensor_t *eta, tensor_t *sigma, double *ep_bar, double *fs, double *psi_n, double *loadunl_n, double *Tao_n,
		               double *Tao_max, double *kp, tensor_t *sigma_ref,  int *flagTolSubSteps, int *flagNoSubSteps, double *ErrBA );

void MatUpd_vMFA (double J2_pr, tensor_t dev_pr, double psi, double c, tensor_t eta_n, tensor_t e_n1, double mu, double Lambda, double Sy,
		tensor_t *epl, tensor_t ep, double *ep_bar, double ep_barn, tensor_t *eta, tensor_t *sigma, tensor_t stresses,
		double *fs, double *psi_n, double *loadunl_n, double *Tao_n, double *Tao_max );

void MatUpd_vMBA (double Su, double G, double Lambda, double psi, double m, double *kappa,
		         tensor_t e_n, tensor_t e_n1, tensor_t *sigma_ref, tensor_t *sigma,
		         double substepTol, int *FlagSubSteps, int *FlagNoSubSteps, double *ErrMax) ;

double getHardening (double kappa, double psi, double m, double G);
double get_kappa    ( tensor_t Sdev, tensor_t Sref, double Tol, double Su, double kn, double G );
double get_kappaUnLo( tensor_t Sn, tensor_t De, double Tol, double Su, double kn, double G, double psi, double m, double *Err );
double get_kappaUnLoading_II( tensor_t Sn, tensor_t De, double Tol, double Su, double kn, double G, double psi, double m, double *Err );
void   EvalSubStep    (tensor_t  sigma_n, tensor_t De, tensor_t De_dev, double De_vol, double Dt, tensor_t *sigma_ref,
		               tensor_t *sigma_up, double kappa_n, double G, double Lambda, double Su,
		               double psi, double m, double substepTol, double *kappa_up, double *ErrB, double *ErrS);


tensor_t ApproxGravity_tensor(double Szz, double phi, double h, double lz, double rho);


int get_displacements ( mysolver_t *solver, elem_t *elemp, fvector_t *u );

/* -------------------------------------------------------------------------- */
/*                              Stability methods                             */
/* -------------------------------------------------------------------------- */

void check_yield_limit      ( mesh_t *myMesh, int32_t eindex, double vs,
                              double fs, double k, int qp);
void check_strain_stability ( double dLambda, double dt, mesh_t *myMesh,
                              int32_t eindex, double vs, double fs,
                              double k, int qp);
void check_nan_displ        (mesh_t *myMesh, int32_t eindex, fvector_t *u, qptensors_t   *stresses, qptensors_t   *pstrains, double h );

/* -------------------------------------------------------------------------- */
/*                   Nonlinear core computational methods                     */
/* -------------------------------------------------------------------------- */

void     specDecomp(tensor_t sigma, vect1_t *n1, vect1_t *n2, vect1_t *n3, vect1_t *eig_values);
void     tql2(double V[3][3], double* d, double* e);
void     tred2(double V[3][3], double *d, double *e);
void     BOX85_l(double ep_bar_n,vect1_t sigma_ppal_trial,double Phi, double Psi, double H, double c0, double K, double G, vect1_t *sigma_ppal, double *ep_bar_n1);
void     BOX86_l(double ep_bar_n,vect1_t sigma_ppal_trial,double Phi, double Psi, double H, double c0, double K, double G, double id, vect1_t *sigma_ppal, double *ep_bar_n1);
void     BOX87_l(double ep_bar_n,double p_trial,double Phi, double Psi, double H, double c0, double K, vect1_t *sigma_ppal, double *ep_bar_n1);
tensor_t specRecomp(vect1_t eig_val, vect1_t n1, vect1_t n2, vect1_t n3);
double   get_ShearTensionLimits (double phi, double coh, double S1, double S3);
void     TensionCutoff_Return( double k, double mu, double phi, double coh, vect1_t sigma_ppal_pr, vect1_t *SigmaUP );
int      CornerZones( vect1_t Sigma, double S_p, vect1_t* SigmaUP, double Phi_pr[5]);


void compute_addforce_gravity( mesh_t     *myMesh,
                               mysolver_t *mySolver,
                               int         step,
                               double      dt );

void compute_bottom_reactions ( mesh_t     *myMesh,
                                mysolver_t *mySolver,
                                fmatrix_t (*theK1)[8],
                                fmatrix_t (*theK2)[8],
                                int         step,
                                double      dt );

void geostatic_displacements_fix( mesh_t     *myMesh,
                                  mysolver_t *mySolver,
                                  double      totalDomainDepth,
                                  double      dt,
                                  int         step );


void compute_addforce_nl ( mesh_t     *myMesh,
                           mysolver_t *mySolver,
                           double      theDeltaTSquared);

void compute_nonlinear_state ( mesh_t     *myMesh,
                               mysolver_t *mySolver,
                               int32_t     theNumberOfStations,
                               int32_t     myNumberOfStations,
                               station_t  *myStations,
                               double      theDeltaT,
                               int         step );

/* -------------------------------------------------------------------------- */
/*                        Nonlinear Output to Stations                        */
/* -------------------------------------------------------------------------- */

void nonlinear_stations_init ( mesh_t    *myMesh,
                               station_t *myStations,
                               int32_t    myNumberOfStations );

void print_nonlinear_stations ( mesh_t     *myMesh,
                                mysolver_t *mySolver,
                                station_t  *myStations,
                                int32_t     myNumberOfStations,
                                double      dt,
                                int         step,
                                int         rate);

/* -------------------------------------------------------------------------- */

void nonlinear_yield_stats(mesh_t *myMesh, int32_t myID, int32_t theTotalSteps, int32_t theGroupSize);

void check_balance( int32_t myID );

#endif /* Q_NONLINEAR_H */








