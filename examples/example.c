#include "Marmot-C-API/MarmotElement_C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  printf( "=========================================\n" );
  printf( "Starting Marmot C API Comprehensive Example...\n" );
  printf( "=========================================\n" );

  const char* elementName = "CPE4";
  int         code        = MarmotElementFactory_getElementCodeFromName( elementName );

  if ( code < 0 ) {
    printf( "Element '%s' not found.\n", elementName );
    return 0;
  }

  int             elementNumber = 1001;
  MarmotElement_t el            = MarmotElementFactory_createElement( code, elementNumber );
  if ( !el ) {
    printf( "Error: Failed to create element.\n" );
    return 1;
  }

  /* 1. Basic Element Dimension Queries (Material Independent) */
  int nodes = MarmotElement_getNNodes( el );
  int dims  = MarmotElement_getNSpatialDimensions( el );
  int dofs  = MarmotElement_getNDofPerElement( el );
  printf( "Created %s (ID: %d): Nodes=%d, Dims=%d, Dofs=%d\n", elementName, elementNumber, nodes, dims, dofs );

  /* 2. Assign Node Coordinates (Often needed before querying shape or
   * quadrature points) */
  double* nodeCoords = (double*)calloc( nodes * dims > 0 ? nodes * dims : 1, sizeof( double ) );
  if ( nodes * dims > 0 ) {
    /* Pseudo-valid coordinates for a unit square to prevent singular Jacobian
     * exceptions */
    if ( nodes >= 4 && dims >= 2 ) {
      nodeCoords[0] = 0.0;
      nodeCoords[1] = 0.0; /* Node 1 */
      nodeCoords[2] = 1.0;
      nodeCoords[3] = 0.0; /* Node 2 */
      nodeCoords[4] = 1.0;
      nodeCoords[5] = 1.0; /* Node 3 */
      nodeCoords[6] = 0.0;
      nodeCoords[7] = 1.0; /* Node 4 */
    }
    MarmotElement_assignNodeCoordinates( el, nodeCoords );
  }

  /* 3. Assign Materials and Properties (MUST be done before requesting
   * StateVars count or initializing) */
  int matCode = MarmotMaterialFactory_getMaterialCodeFromName( "LINEARELASTIC" );
  if ( matCode >= 0 ) {
    double linearElasticProps[2] = { 1000.0, 0.0 }; /* E, nu */
    MarmotElement_assignMaterialSection( el, matCode, linearElasticProps, 2 );
  }
  else {
    printf( "Material 'LINEARELASTIC' not registered via factory, passing "
            "dummy.\n" );
    exit( 1 );
  }

  double dummyElementProps[1] = { 1.0 };
  MarmotElement_assignElementProperty( el, dummyElementProps, 1 );

  /* 4. Query State Variables and Allocate corresponding Arrays */
  int nStateVars = MarmotElement_getNumberOfRequiredStateVars( el );
  printf( "Element requires %d State Variables.\n", nStateVars );
  double* stateVars = (double*)calloc( nStateVars > 0 ? nStateVars : 1, sizeof( double ) );
  if ( nStateVars > 0 ) {
    MarmotElement_assignStateVars( el, stateVars, nStateVars );
  }

  /* 5. Additional Information Queries */
  int  qpoints = MarmotElement_getNumberOfQuadraturePoints( el );
  char shape[64];
  MarmotElement_getElementShape( el, shape, sizeof( shape ) );
  printf( "Quadrature Points: %d, Shape: %s\n", qpoints, shape );

  if ( nodes > 0 ) {
    for ( int i = 0; i < nodes; ++i ) {
      int fieldsCount = MarmotElement_getNodeFieldsCount( el, i );
      if ( fieldsCount > 0 ) {
        char fieldName[64];
        MarmotElement_getNodeFieldName( el, i, 0, fieldName, sizeof( fieldName ) );
        if ( i == 0 ) {
          printf( "Node 0 has %d fields. First field: %s\n", fieldsCount, fieldName );
        }
      }
    }
  }

  int dofPattern[100];
  int actualPatternSize = 0;
  MarmotElement_getDofIndicesPermutationPattern( el, dofPattern, 100, &actualPatternSize );
  printf( "DOF Permutation Pattern size queried.\n" );

  double centerCoords[3]  = { 0.0 };
  int    actualCenterDims = 0;
  MarmotElement_getCoordinatesAtCenter( el, centerCoords, 3, &actualCenterDims );

  if ( qpoints > 0 ) {
    double* qCoords       = (double*)calloc( qpoints * dims, sizeof( double ) );
    int     actualQPoints = 0, actualQDims = 0;
    MarmotElement_getCoordinatesAtQuadraturePoints( el, qCoords, qpoints, dims, &actualQPoints, &actualQDims );
    free( qCoords );
    printf( "Quadrature Coordinates queried.\n" );
  }

  /* 6. Initialization & Conditions */
  MarmotElement_initializeYourself( el );

  double dummyConditions[1] = { 0.0 };
  MarmotElement_setInitialConditions( el, 0, dummyConditions );
  printf( "Initialization successful.\n" );

  /* 7. Computation Methods */
  /* Allocating appropriately sized memory structures based on DOF size */
  double* QTotal = (double*)calloc( dofs > 0 ? dofs : 1, sizeof( double ) );
  double* dQ     = (double*)calloc( dofs > 0 ? dofs : 1, sizeof( double ) );
  double* Pint   = (double*)calloc( dofs > 0 ? dofs : 1, sizeof( double ) );
  double* Pext   = (double*)calloc( dofs > 0 ? dofs : 1, sizeof( double ) );
  double* K      = (double*)calloc( dofs * dofs > 0 ? dofs * dofs : 1, sizeof( double ) );
  double* M      = (double*)calloc( dofs * dofs > 0 ? dofs * dofs : 1, sizeof( double ) );

  double time = 0.0, dT = 1.0, pNewdT = 1e12;

  dQ[1] = 0.1;
  dQ[3] = 0.1;
  /* Execute a computation step */
  MarmotElement_computeYourself( el, QTotal, dQ, Pint, K, &time, dT, &pNewdT );

  for ( int i = 0; i < dofs; i++ ) {
    printf( "Internal Force: %f\n", Pint[i] );
  }
  for ( int i = 0; i < dofs * dofs; i++ ) {
    printf( "Tangent Stiffness: %f\n", K[i] );
  }

  /* Extrapolate Loads */
  double load[3] = { 1.0 };
  MarmotElement_computeDistributedLoad( el, MARMOT_ELEMENT_LOAD_PRESSURE, Pext, K, 1, load, QTotal, &time, dT );

  printf( "Distributed Load:\n" );
  for ( int i = 0; i < dofs; i++ ) {
    printf( "    External Force: %f\n", Pext[i] );
  }

  double bodyForce[3] = { 1.0 };
  MarmotElement_computeBodyForce( el, Pext, K, bodyForce, QTotal, &time, dT );

  printf( "Body Force:\n" );
  for ( int i = 0; i < dofs; i++ ) {
    printf( "    External Force: %f\n", Pext[i] );
  }

  /* Inertia Extrapolations */
  MarmotElement_computeLumpedInertia( el, M );
  MarmotElement_computeConsistentInertia( el, M );

  printf( "Computation tests completed.\n" );

  /* 8. State View */
  if ( qpoints > 0 ) {
    MarmotElement_StateView_t view = MarmotElement_getStateView( el, "Sigma11", 0 );
    if ( view.stateLocation ) {
      printf( "StateView 'Sigma11' retrieved successfully, pointer located.\n" );
    }
  }

  /* 9. Cleanup */
  free( stateVars );
  free( nodeCoords );
  free( QTotal );
  free( dQ );
  free( Pint );
  free( Pext );
  free( K );
  free( M );

  MarmotElement_destroy( el );
  printf( "Element cleanly destroyed.\n" );
  printf( "Comprehensive test completed successfully.\n" );

  return 0;
}
