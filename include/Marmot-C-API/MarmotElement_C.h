#ifndef MARMOT_ELEMENT_C_H
#define MARMOT_ELEMENT_C_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque pointer for MarmotElement */
typedef struct MarmotElement_s* MarmotElement_t;

/* Equivalent of MarmotElement::StateTypes */
typedef enum {
  MARMOT_ELEMENT_STATE_SIGMA11 = 0,
  MARMOT_ELEMENT_STATE_SIGMA22,
  MARMOT_ELEMENT_STATE_SIGMA33,
  MARMOT_ELEMENT_STATE_HYDROSTATIC_STRESS,
  MARMOT_ELEMENT_STATE_GEOSTATIC_STRESS,
  MARMOT_ELEMENT_STATE_MARMOT_MATERIAL_STATE_VARS,
  MARMOT_ELEMENT_STATE_MARMOT_MATERIAL_INITIALIZATION,
  MARMOT_ELEMENT_STATE_HAS_EIGEN_DEFORMATION
} MarmotElement_StateTypes_t;

/* Equivalent of MarmotElement::DistributedLoadTypes */
typedef enum {
  MARMOT_ELEMENT_LOAD_PRESSURE = 0,
  MARMOT_ELEMENT_LOAD_SURFACE_TORSION,
  MARMOT_ELEMENT_LOAD_SURFACE_TRACTION
} MarmotElement_DistributedLoadTypes_t;

/* Equivalent of StateView */
typedef struct {
  double* stateLocation;
  int     stateSize;
} MarmotElement_StateView_t;

/* ------------------------------------------------------------------------- */
/* MarmotFactory Wrappers                                                    */
/* ------------------------------------------------------------------------- */

/**
 * @brief Get the unique element code from its name.
 * @param elementName Name of the element (null-terminated string).
 * @return Unique code or error code (-1) if not found or exception occurred.
 */
int MarmotElementFactory_getElementCodeFromName( const char* elementName );

/**
 * @brief Get the unique material code from its name.
 * @param materialName Name of the material (null-terminated string).
 * @return Unique code or error code (-1) if not found or exception occurred.
 */
int MarmotMaterialFactory_getMaterialCodeFromName( const char* materialName );

/**
 * @brief Create an element instance based on its code and number.
 * @param elementCode Unique code for the element.
 * @param elementNumber Unique identifier for the element instance.
 * @return Pointer to the created MarmotElement instance, or NULL on failure.
 */
MarmotElement_t MarmotElementFactory_createElement( int elementCode, int elementNumber );

/* ------------------------------------------------------------------------- */
/* MarmotElement Lifecycle Wrapper                                           */
/* ------------------------------------------------------------------------- */

/**
 * @brief Destroy a MarmotElement instance.
 * @param element Pointer to the element.
 */
void MarmotElement_destroy( MarmotElement_t element );

/* ------------------------------------------------------------------------- */
/* MarmotElement Member Function Wrappers                                    */
/* ------------------------------------------------------------------------- */

int MarmotElement_getNumberOfRequiredStateVars( MarmotElement_t element );

/**
 * @brief Get the number of nodes in the element.
 */
int MarmotElement_getNNodes( MarmotElement_t element );

/**
 * @brief Get the number of fields for a specific node (0-indexed).
 * Returns -1 if nodeIndex is out of bounds.
 */
int MarmotElement_getNodeFieldsCount( MarmotElement_t element, int nodeIndex );

/**
 * @brief Get the field name for a specific node and field index.
 * @param outBuffer Buffer to copy the field name into (must be pre-allocated).
 * @param bufferSize Size of outBuffer.
 */
void MarmotElement_getNodeFieldName( MarmotElement_t element,
                                     int             nodeIndex,
                                     int             fieldIndex,
                                     char*           outBuffer,
                                     int             bufferSize );

/**
 * @brief Get permutation pattern for degrees of freedom.
 * @param outArray Array to fill with indices.
 * @param maxSize Maximum size of outArray.
 * @param actualSize Populated with the actual number of indices (can be NULL).
 */
void MarmotElement_getDofIndicesPermutationPattern( MarmotElement_t element,
                                                    int*            outArray,
                                                    int             maxSize,
                                                    int*            actualSize );

int MarmotElement_getNSpatialDimensions( MarmotElement_t element );

int MarmotElement_getNDofPerElement( MarmotElement_t element );

/**
 * @brief Get the element shape as a string.
 * @param outBuffer Buffer to copy the shape string into.
 * @param bufferSize Size of outBuffer.
 */
void MarmotElement_getElementShape( MarmotElement_t element, char* outBuffer, int bufferSize );

void MarmotElement_assignStateVars( MarmotElement_t element, double* stateVars, int nStateVars );

/**
 * @brief Assign element property (wrapper for assignProperty(const ElementProperties&)).
 */
void MarmotElement_assignElementProperty( MarmotElement_t element,
                                          const double*   elementProperties,
                                          int             nElementProperties );

/**
 * @brief Assign material section property (wrapper for assignProperty(const MarmotMaterialSection&)).
 */
void MarmotElement_assignMaterialSection( MarmotElement_t element,
                                          int             materialCode,
                                          const double*   materialProperties,
                                          int             nMaterialProperties );

void MarmotElement_assignNodeCoordinates( MarmotElement_t element, const double* coordinates );

void MarmotElement_initializeYourself( MarmotElement_t element );

void MarmotElement_setInitialConditions( MarmotElement_t            element,
                                         MarmotElement_StateTypes_t state,
                                         const double*              values );

void MarmotElement_computeYourself( MarmotElement_t element,
                                    const double*   QTotal,
                                    const double*   dQ,
                                    double*         Pint,
                                    double*         K,
                                    const double*   time,
                                    double          dT,
                                    double*         pNewdT );

void MarmotElement_computeDistributedLoad( MarmotElement_t                      element,
                                           MarmotElement_DistributedLoadTypes_t loadType,
                                           double*                              Pext,
                                           double*                              K,
                                           int                                  elementFace,
                                           const double*                        load,
                                           const double*                        QTotal,
                                           const double*                        time,
                                           double                               dT );

void MarmotElement_computeBodyForce( MarmotElement_t element,
                                     double*         Pext,
                                     double*         K,
                                     const double*   load,
                                     const double*   QTotal,
                                     const double*   time,
                                     double          dT );

/**
 * @brief Compute lumped inertia. Return 0 on success, non-zero on error/unimplemented.
 */
int MarmotElement_computeLumpedInertia( MarmotElement_t element, double* I );

/**
 * @brief Compute consistent inertia. Return 0 on success, non-zero on error/unimplemented.
 */
int MarmotElement_computeConsistentInertia( MarmotElement_t element, double* I );

/**
 * @brief Get state view.
 * @param stateName Null-terminated string.
 */
MarmotElement_StateView_t MarmotElement_getStateView( MarmotElement_t element,
                                                      const char*     stateName,
                                                      int             quadraturePoint );

/**
 * @brief Get coordinates at center.
 * @param outArray Pre-allocated array to hold coordinates.
 * @param maxSize Maximum number of elements in outArray.
 * @param actualSize Number of elements actually written (can be NULL).
 */
void MarmotElement_getCoordinatesAtCenter( MarmotElement_t element, double* outArray, int maxSize, int* actualSize );

/**
 * @brief Get the number of quadrature points.
 */
int MarmotElement_getNumberOfQuadraturePoints( MarmotElement_t element );

/**
 * @brief Get coordinates at all quadrature points as a flat 1D array.
 * @param outArray Pre-allocated flat array [num_qpoints * num_dims].
 * @param maxPoints Maximum number of points outArray can hold.
 * @param maxDim Maximum number of dimensions outArray can hold per point.
 * @param actualPoints Populated with actual number of points.
 * @param actualDim Populated with actual number of dimensions.
 */
void MarmotElement_getCoordinatesAtQuadraturePoints( MarmotElement_t element,
                                                     double*         outArray,
                                                     int             maxPoints,
                                                     int             maxDim,
                                                     int*            actualPoints,
                                                     int*            actualDim );

#ifdef __cplusplus
}
#endif

#endif /* MARMOT_ELEMENT_C_H */
