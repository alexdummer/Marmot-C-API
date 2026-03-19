#include "Marmot-C-API/MarmotElement_C.h"
#include "Marmot/Marmot.h"
#include "Marmot/MarmotElement.h"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

extern "C" {

using namespace MarmotLibrary;

int MarmotElementFactory_getElementCodeFromName( const char* elementName )
{
  try {
    return MarmotElementFactory::getElementCodeFromName( std::string( elementName ) );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElementFactory_getElementCodeFromName: " << e.what() << std::endl;
    return -1;
  }
}

int MarmotMaterialFactory_getMaterialCodeFromName( const char* materialName )
{
  try {
    return MarmotMaterialFactory::getMaterialCodeFromName( std::string( materialName ) );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotMaterialFactory_getMaterialCodeFromName: " << e.what() << std::endl;
    return -1;
  }
}

MarmotElement_t MarmotElementFactory_createElement( int elementCode, int elementNumber )
{
  try {
    MarmotElement* el = MarmotElementFactory::createElement( elementCode, elementNumber );
    return reinterpret_cast< MarmotElement_t >( el );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElementFactory_createElement: " << e.what() << std::endl;
    return nullptr;
  }
}

void MarmotElement_destroy( MarmotElement_t element )
{
  if ( element ) {
    delete reinterpret_cast< MarmotElement* >( element );
  }
}

int MarmotElement_getNumberOfRequiredStateVars( MarmotElement_t element )
{
  if ( !element )
    return 0;
  return reinterpret_cast< MarmotElement* >( element )->getNumberOfRequiredStateVars();
}

int MarmotElement_getNNodes( MarmotElement_t element )
{
  if ( !element )
    return 0;
  return reinterpret_cast< MarmotElement* >( element )->getNNodes();
}

int MarmotElement_getNodeFieldsCount( MarmotElement_t element, int nodeIndex )
{
  if ( !element )
    return -1;
  auto fields = reinterpret_cast< MarmotElement* >( element )->getNodeFields();
  if ( nodeIndex < 0 || nodeIndex >= static_cast< int >( fields.size() ) )
    return -1;
  return static_cast< int >( fields[nodeIndex].size() );
}

void MarmotElement_getNodeFieldName( MarmotElement_t element,
                                     int             nodeIndex,
                                     int             fieldIndex,
                                     char*           outBuffer,
                                     int             bufferSize )
{
  if ( !element || !outBuffer || bufferSize <= 0 )
    return;
  outBuffer[0] = '\0';
  auto fields  = reinterpret_cast< MarmotElement* >( element )->getNodeFields();
  if ( nodeIndex >= 0 && nodeIndex < static_cast< int >( fields.size() ) ) {
    if ( fieldIndex >= 0 && fieldIndex < static_cast< int >( fields[nodeIndex].size() ) ) {
      std::strncpy( outBuffer, fields[nodeIndex][fieldIndex].c_str(), bufferSize - 1 );
      outBuffer[bufferSize - 1] = '\0';
    }
  }
}

void MarmotElement_getDofIndicesPermutationPattern( MarmotElement_t element,
                                                    int*            outArray,
                                                    int             maxSize,
                                                    int*            actualSize )
{
  if ( !element || !outArray )
    return;
  auto pattern = reinterpret_cast< MarmotElement* >( element )->getDofIndicesPermutationPattern();
  int  size    = static_cast< int >( pattern.size() );
  if ( actualSize )
    *actualSize = size;
  for ( int i = 0; i < size && i < maxSize; ++i ) {
    outArray[i] = pattern[i];
  }
}

int MarmotElement_getNSpatialDimensions( MarmotElement_t element )
{
  if ( !element )
    return 0;
  return reinterpret_cast< MarmotElement* >( element )->getNSpatialDimensions();
}

int MarmotElement_getNDofPerElement( MarmotElement_t element )
{
  if ( !element )
    return 0;
  return reinterpret_cast< MarmotElement* >( element )->getNDofPerElement();
}

void MarmotElement_getElementShape( MarmotElement_t element, char* outBuffer, int bufferSize )
{
  if ( !element || !outBuffer || bufferSize <= 0 )
    return;
  std::string shape = reinterpret_cast< MarmotElement* >( element )->getElementShape();
  std::strncpy( outBuffer, shape.c_str(), bufferSize - 1 );
  outBuffer[bufferSize - 1] = '\0';
}

void MarmotElement_assignStateVars( MarmotElement_t element, double* stateVars, int nStateVars )
{
  if ( !element )
    return;
  reinterpret_cast< MarmotElement* >( element )->assignStateVars( stateVars, nStateVars );
}

void MarmotElement_assignElementProperty( MarmotElement_t element,
                                          const double*   elementProperties,
                                          int             nElementProperties )
{
  if ( !element )
    return;
  ElementProperties props( elementProperties, nElementProperties );
  reinterpret_cast< MarmotElement* >( element )->assignProperty( props );
}

void MarmotElement_assignMaterialSection( MarmotElement_t element,
                                          int             materialCode,
                                          const double*   materialProperties,
                                          int             nMaterialProperties )
{
  if ( !element )
    return;
  MarmotMaterialSection props( materialCode, materialProperties, nMaterialProperties );
  reinterpret_cast< MarmotElement* >( element )->assignProperty( props );
}

void MarmotElement_assignNodeCoordinates( MarmotElement_t element, const double* coordinates )
{
  if ( !element )
    return;
  reinterpret_cast< MarmotElement* >( element )->assignNodeCoordinates( coordinates );
}

void MarmotElement_initializeYourself( MarmotElement_t element )
{
  if ( !element )
    return;
  try {
    reinterpret_cast< MarmotElement* >( element )->initializeYourself();
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_initializeYourself: " << e.what() << std::endl;
  }
}

void MarmotElement_setInitialConditions( MarmotElement_t            element,
                                         MarmotElement_StateTypes_t state,
                                         const double*              values )
{
  if ( !element )
    return;
  try {
    reinterpret_cast< MarmotElement* >( element )->setInitialConditions( static_cast< MarmotElement::StateTypes >(
                                                                           state ),
                                                                         values );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_setInitialConditions: " << e.what() << std::endl;
  }
}

void MarmotElement_computeYourself( MarmotElement_t element,
                                    const double*   QTotal,
                                    const double*   dQ,
                                    double*         Pint,
                                    double*         K,
                                    const double*   time,
                                    double          dT,
                                    double*         pNewdT )
{
  if ( !element ) {
    std::cerr << "MarmotElement_computeYourself: Element is null" << std::endl;
    return;
  }
  try {
    reinterpret_cast< MarmotElement* >( element )->computeYourself( QTotal, dQ, Pint, K, time, dT, *pNewdT );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_computeYourself: " << e.what() << std::endl;
  }
}

void MarmotElement_computeDistributedLoad( MarmotElement_t                      element,
                                           MarmotElement_DistributedLoadTypes_t loadType,
                                           double*                              Pext,
                                           double*                              K,
                                           int                                  elementFace,
                                           const double*                        load,
                                           const double*                        QTotal,
                                           const double*                        time,
                                           double                               dT )
{
  if ( !element )
    return;
  try {
    reinterpret_cast< MarmotElement* >( element )
      ->computeDistributedLoad( static_cast< MarmotElement::DistributedLoadTypes >( loadType ),
                                Pext,
                                K,
                                elementFace,
                                load,
                                QTotal,
                                time,
                                dT );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_computeDistributedLoad: " << e.what() << std::endl;
  }
}

void MarmotElement_computeBodyForce( MarmotElement_t element,
                                     double*         Pext,
                                     double*         K,
                                     const double*   load,
                                     const double*   QTotal,
                                     const double*   time,
                                     double          dT )
{
  if ( !element )
    return;
  try {
    reinterpret_cast< MarmotElement* >( element )->computeBodyForce( Pext, K, load, QTotal, time, dT );
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_computeBodyForce: " << e.what() << std::endl;
  }
}

int MarmotElement_computeLumpedInertia( MarmotElement_t element, double* I )
{
  if ( !element )
    return -1;
  try {
    reinterpret_cast< MarmotElement* >( element )->computeLumpedInertia( I );
    return 0;
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_computeLumpedInertia: " << e.what() << std::endl;
    return -1;
  }
}

int MarmotElement_computeConsistentInertia( MarmotElement_t element, double* I )
{
  if ( !element )
    return -1;
  try {
    reinterpret_cast< MarmotElement* >( element )->computeConsistentInertia( I );
    return 0;
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_computeConsistentInertia: " << e.what() << std::endl;
    return -1;
  }
}

MarmotElement_StateView_t MarmotElement_getStateView( MarmotElement_t element,
                                                      const char*     stateName,
                                                      int             quadraturePoint )
{
  MarmotElement_StateView_t view = { nullptr, 0 };
  if ( !element || !stateName )
    return view;
  try {
    StateView cppView  = reinterpret_cast< MarmotElement* >( element )->getStateView( std::string( stateName ),
                                                                                     quadraturePoint );
    view.stateLocation = cppView.stateLocation;
    view.stateSize     = cppView.stateSize;
  }
  catch ( const std::exception& e ) {
    std::cerr << "MarmotElement_getStateView: " << e.what() << std::endl;
  }
  return view;
}

void MarmotElement_getCoordinatesAtCenter( MarmotElement_t element, double* outArray, int maxSize, int* actualSize )
{
  if ( !element || !outArray )
    return;
  auto coords = reinterpret_cast< MarmotElement* >( element )->getCoordinatesAtCenter();
  int  size   = static_cast< int >( coords.size() );
  if ( actualSize )
    *actualSize = size;
  for ( int i = 0; i < size && i < maxSize; ++i ) {
    outArray[i] = coords[i];
  }
}

int MarmotElement_getNumberOfQuadraturePoints( MarmotElement_t element )
{
  if ( !element )
    return 0;
  return reinterpret_cast< MarmotElement* >( element )->getNumberOfQuadraturePoints();
}

void MarmotElement_getCoordinatesAtQuadraturePoints( MarmotElement_t element,
                                                     double*         outArray,
                                                     int             maxPoints,
                                                     int             maxDim,
                                                     int*            actualPoints,
                                                     int*            actualDim )
{
  if ( !element || !outArray )
    return;
  auto coords    = reinterpret_cast< MarmotElement* >( element )->getCoordinatesAtQuadraturePoints();
  int  numPoints = static_cast< int >( coords.size() );
  int  numDims   = numPoints > 0 ? static_cast< int >( coords[0].size() ) : 0;

  if ( actualPoints )
    *actualPoints = numPoints;
  if ( actualDim )
    *actualDim = numDims;

  for ( int p = 0; p < numPoints && p < maxPoints; ++p ) {
    for ( int d = 0; d < numDims && d < maxDim; ++d ) {
      outArray[p * maxDim + d] = coords[p][d];
    }
  }
}

} // extern "C"
