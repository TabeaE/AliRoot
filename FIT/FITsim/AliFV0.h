#ifndef ALIFV0_H
#define ALIFV0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
// Full geomrtry  hits classes for detector: FIT    //
////////////////////////////////////////////////

#include <Rtypes.h>
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGraph.h"
#include <TGeoMatrix.h>
#include <TVirtualMC.h>
#include <TVector.h>
#include <TArrayI.h>
#include <sstream>
#include <string>
#include <vector>
//#include <array>

class TArrayI;

/// FV0 Geometry
class AliFV0 : public TNamed
{
public:
  /// AliFV0 type options possible to be initialized. The type of the Geometry will specify which components are
  /// created. Geometry types
  ///  -> eUnitialized   => no parts
  ///  -> eOnlySensitive => only sensitive detector parts
  ///  -> eRough         => sensitive parts and rough structural elements
  ///  -> eFull          => complete, detailed Geometry (including screws, etc.)
  enum EGeoType
  {
    eUninitialized,
    eOnlySensitive,
    eRough,
    eFull
  };

  /// Geometry components possible to be enabled/disabled. Only enabled components will be created.
  enum EGeoComponent
  {
    eScintillator,
    ePlastics,
    ePmts,
    eFibers,
    eScrews,
    eRods,
    eContainer
  };

  enum Constants
  {
    sNumberOfCellSectors = 4,
    sNumberOfCellRings = 5,
    sNumberOfCells = sNumberOfCellRings * sNumberOfCellSectors * 2,
    sNumberOfPMTsPerSector = 6,
    sNumberOfPMTs = 24,
    sNumberOfScrewTypes = 6,
    sNumberOfRodTypes = 4,
    sNumberOfReadoutChannels = sNumberOfCells + sNumberOfCellSectors * 2
  };

  /// Default constructor.
  /// It must be kept public for root persistency purposes,
  /// but should never be called by the outside world
  AliFV0() : mGeometryType(eUninitialized), mLeftTransformation(nullptr), mRightTransformation(nullptr){};
  AliFV0(const char *name, const char *title) : TNamed(name, title), mGeometryType(eUninitialized), mLeftTransformation(nullptr), mRightTransformation(nullptr){};

  AliFV0(EGeoType initType);

  ~AliFV0();

  /// Get the unique ID of the current scintillator cell during simulation.
  /// The ID is a number starting from 0 at the first cell right of the y-axis
  /// and continues clockwise one ring at a time.
  /// \param  fMC The virtual Monte Carlo interface.
  /// \return The ID of the current scintillator cell during simulation.
  int getCurrentCellId(const TVirtualMC *fMC) const;

  int getCurrentRingId(const TVirtualMC *fMC) const;

  // 1: 00 01 02 03 04 05 06 07
  // 2: 08 09 10 11 12 13 14 15
  // 3: 16 17 18 19 20 21 22 23
  // 4: 24 25 26 27 28 29 30 31
  // 5: 32 33 34 35 36 37 38 39

  /// Add alignable  volumes
  void addAlignableVolumes() const;

  /// Get the names of all the sensitive volumes of the Geometry.
  /// \return The names of all the sensitive volumes of the Geometry.
  const std::vector<std::string> &getSensitiveVolumeNames() const { return mSensitiveVolumeNames; };

  /// Enable or disable a Geometry component. To be called before the Geometry is built. A disabled component will not
  /// be added to the Geometry. The enabled components are by default specified by the Geometry type.
  /// \param  component   The Geometry component to be enabled/disabled.
  /// \param  enable      Setting the enabled state. Default is true.
  /// \return The enabled state of the Geometry component.
  bool enableComponent(EGeoComponent component, bool enable = true);

  /// Build the Geometry.
  void ConstructGeometry();

  /// Utility functions to be accessed externally

  /// Sets the input parameters to the position of the geometrical center of sensitive detector
  /// \param  x   x [cm].
  /// \param  y   y [cm].
  /// \param  z   z [cm].
  void getGlobalPosition(float &x, float &y, float &z);
  TVector3 &getCellCenter(UInt_t cellId);
  TVector3 &getReadoutCenter(UInt_t cellId);
  const int getNumberOfReadoutChannels() { return sNumberOfReadoutChannels; };

  /// Helper function to check if the cellId belongs to ring 5.
  /// \param  cellId  Id of the cell in range from 0 to 39.
  /// \return True if cellId belongs to ring 5.
  bool isRing5(UInt_t cellId);

  /// Get the density of the PMTs.
  const float getPmtDensity()
  {
    return sDensityPmt;
  }

  const std::string sDetectorName = "FV0";

  // General Geometry constants
  const float sEpsilon = 0.01;     ///< Used to make one spatial dimension infinitesimally larger than other
  const float sDzScintillator = 4; ///< Thickness of the scintillator
  const float sDzPlastic = 1;      ///< Thickness of the fiber plastics

  const float sXGlobal = 0;                         ///< Global x-position of the geometrical center of scintillators
  const float sYGlobal = 0;                         ///< Global y-position of the geometrical center of scintillators
                                                    // FT0 starts at z=328
  const float sZGlobal = 320 - sDzScintillator / 2; ///< Global z-pos of geometrical center of scintillators
  const float sDxHalvesSeparation = 0;              ///< Separation between the left and right side of the detector
  const float sDyHalvesSeparation = 0;              ///< y-position of the right detector part relative to the left part
  const float sDzHalvesSeparation = 0;              ///< z-position of the right detector part relative to the left part

  /// Cell and scintillator constants
  // static const int sNumberOfCellSectors;                                             ///< Number of cell sectors for one half of the detector  ( = 4)
  // static const int sNumberOfCellRings;                                               ///< Number of cell rings ( = 5)
  // static const int sNumberOfCells;       ///< Number of cells
  // static const int sNumberOfReadoutChannels; ///< Number of ch (2 ch per cell in r5)

  /// Look-up tables converting cellId to the ring and sector number
  const int sCellToRing[sNumberOfCells] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
  const int sCellToSector[sNumberOfCells] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

  /// Average cell ring radii.
  const float sCellRingRadii[sNumberOfCellRings + 1] = {4.01, 7.3, 12.9, 21.25, 38.7, 72.115};
  const char sCellTypes[sNumberOfCellSectors] = {'a', 'b', 'b', 'a'}; ///< Ordered cell types per half a ring
                                                                      /// Separation between the scintillator cells; paint thickness + half of separation gap.
  const float sDrSeparationScint = 0.03 + 0.04;

  /// Shift of the inner radius origin of the scintillators.
  const float sXShiftInnerRadiusScintillator = -0.15;
  /// Extension of the scintillator holes for the metal rods
  const float sDxHoleExtensionScintillator = 0.2;
  const float sDrHoleSmallScintillator = 0.265; ///< Radius of the small scintillator screw hole
  const float sDrHoleLargeScintillator = 0.415; ///< Radius of the large scintillator screw hole

  // Container constants
  const float sDzContainer = 30;                  ///< Depth of the metal container
  const float sDrContainerHole = 4.05;            ///< Radius of the beam hole in the metal container
  const float sXShiftContainerHole = -0.15;       ///< x-shift of the beam hole in the metal container
  const float sDrMaxContainerBack = 83.1;         ///< Outer radius of the container backplate
  const float sDzContainerBack = 1;               ///< Thickness of the container backplate
  const float sDrMinContainerFront = 45.7;        ///< Inner radius of the container frontplate
  const float sDrMaxContainerFront = 83.1;        ///< Outer radius of the container frontplate
  const float sDzContainerFront = 1;              ///< Thickness of the container frontplate
  const float sDxContainerStand = 40;             ///< Width of the container stand
  const float sDyContainerStand = 3;              ///< Height of the container stand at its center in x
  const float sDrMinContainerCone = 24.3;         ///< Inner radius at bottom of container frontplate cone
  const float sDzContainerCone = 16.2;            ///< Depth of the container frontplate cone
  const float sThicknessContainerCone = 0.6;      ///< Thickness of the container frontplate cone
  const float sXYThicknessContainerCone = 0.975;  ///< Radial thickness in the xy-plane of container cone
  const float sDrMinContainerOuterShield = 82.5;  ///< Inner radius of outer container shield
  const float sDrMaxContainerOuterShield = 82.65; ///< Outer radius of outer container shield
  const float sDrMinContainerInnerShield = 4;     ///< Inner radius of the inner container shield
  const float sDrMaxContainerInnerShield = 4.05;  ///< Outer radius of inner container shield
  const float sDxContainerCover = 0.15;           ///< Thickness of the container cover
  const float sDxContainerStandBottom = 38.5;     ///< Width of the bottom of the container stand
  const float sDyContainerStandBottom = 2;        ///< Thickness of the bottom of the container stand

  // PMT constants
  // static const int sNumberOfPMTs;                                        ///< Number of PMTs for one half of the detector.
  // static const int sNumberOfPMTsPerSector;                                ///< Number of PMTs for one sector,
  const float sDrPmt = 3.75;                                           ///< PMT radius
  const float sDzPmt = 12.199;                                         ///< PMT length
  const float sMPmt = 376.77;                                          ///< PMT mass
  const float sDensityPmt = sMPmt / (M_PI * sDrPmt * sDrPmt * sDzPmt); ///< PMT density

  // Fiber constants
  const int sNumberOfPMTFiberVolumes = 5; ///< Number of different fiber equivalent volumes in front of the PMTs
                                          /// The order of cells from which the fibers arrive to the PMTs in one sector.
  const int sPMTFiberCellOrder[sNumberOfPMTsPerSector] = {2, 5, 4, 3, 5, 1};

  // Local position constants

  /// x-position of the right half of the scintillator.
  const float sXScintillator = sDxContainerCover;
  /// z-position of the scintillator cells.
  const float sZScintillator = 0;
  /// z-position of the plastic cells.
  const float sZPlastic = sZScintillator + sDzScintillator / 2 + sDzPlastic / 2;
  /// z-position of the container backplate.
  const float sZContainerBack = sZScintillator - sDzScintillator / 2 - sDzContainerBack / 2;
  /// z-position of the container frontplate.
  const float sZContainerFront = sZContainerBack - sDzContainerBack / 2 + sDzContainer - sDzContainerFront / 2;
  /// z-position of the center of the container.
  const float sZContainerMid = (sZContainerBack + sZContainerFront) / 2;
  /// z-position of the fiber volumes.
  const float sZFiber = (sZPlastic + sZContainerFront) / 2;
  /// z-position of the container frontplate cone.
  const float sZCone = sZContainerFront + sDzContainerFront / 2 - sDzContainerCone / 2;
  /// x shift of all screw holes.
  const float sXShiftScrews = sXScintillator;
  /// x-positions of the PMTs in the right half of the detector.
  const float sXPmt[sNumberOfPMTs] = {8.023, 16.612, 24.987, 33.042, 40.671, 47.778, 59.646, 64.73, 68.982, 72.348, 74.783, 76.257, 76.330, 74.931, 72.569, 69.273, 65.088, 60.065, 48.3, 41.238, 33.645, 25.62, 17.265, 8.688};
  /// y-positions of the PMTs in one half of the detector.
  const float sYPmt[sNumberOfPMTs] = {76.33, 74.931, 72.569, 69.273, 65.088, 60.065, 48.3, 41.238, 33.645, 25.62, 17.265, 8.688, -8.023, -16.612, -24.987, -33.042, -40.671, -47.778, -59.646, -64.73, -68.982, -72.348, -74.783, -76.257};
  /// z-position of the PMTs.
  const float sZPmt = sZContainerBack + sDzContainerBack / 2 + sDzPmt / 2;

  // Screw and rod dimensions

  /// Number of the different screw types.
  // static const int sNumberOfScrewTypes;
  /// Radii of the thinner part of the screw types.
  const float sDrMinScrewTypes[sNumberOfScrewTypes] = {0.25, 0.25, 0.4, 0.4, 0.4, 0.4};
  /// Radii of the thicker part of the screw types.
  const float sDrMaxScrewTypes[sNumberOfScrewTypes] = {0, 0.5, 0.6, 0.6, 0.6, 0};
  /// Length of the thinner part of the screw types.
  const float sDzMaxScrewTypes[sNumberOfScrewTypes] = {6.02, 13.09, 13.1, 23.1, 28.3, 5};
  /// Length of the thicker part of the screw types.
  const float sDzMinScrewTypes[sNumberOfScrewTypes] = {0, 6.78, 6.58, 15.98, 21.48, 0};
  /// z shift of the screws. 0 means they are aligned with the scintillator.
  const float sZShiftScrew = 0;

  /// Number of the different rod types.
  // static const int sNumberOfRodTypes;
  /// Width of the thinner part of the rod types.
  const float sDxMinRodTypes[sNumberOfRodTypes] = {0.366, 0.344, 0.344, 0.344};
  /// Width of the thicker part of the rod types.
  const float sDxMaxRodTypes[sNumberOfRodTypes] = {0.536, 0.566, 0.566, 0.716};
  /// Height of the thinner part of the rod types.
  const float sDyMinRodTypes[sNumberOfRodTypes] = {0.5, 0.8, 0.8, 0.8};
  /// Height of the thicker part of the rod types.
  const float sDyMaxRodTypes[sNumberOfRodTypes] = {0.9, 1.2, 1.2, 1.2};
  /// Length of the thinner part of the rod types.
  const float sDzMaxRodTypes[sNumberOfRodTypes] = {12.5, 12.5, 22.5, 27.7};
  /// Length of the thicker part of the rod types.
  const float sDzMinRodTypes[sNumberOfRodTypes] = {7.45, 7.45, 17.45, 22.65};
  /// z shift of the rods. 0 means they are aligned with the scintillators.
  const float sZShiftRod = -0.05;

  // Strings for volume names, etc.
  const std::string sScintillatorName = "SCINT";
  const std::string sPlasticName = "PLAST";
  const std::string sSectorName = "SECTOR";
  const std::string sCellName = "CELL";
  const std::string sScintillatorSectorName = sScintillatorName + sSectorName;
  const std::string sScintillatorCellName = sScintillatorName + sCellName;
  const std::string sPlasticSectorName = sPlasticName + sSectorName;
  const std::string sPlasticCellName = sPlasticName + sCellName;
  const std::string sPmtName = "PMT";
  const std::string sFiberName = "FIBER";
  const std::string sScrewName = "SCREW";
  const std::string sScrewHolesCSName = "FV0SCREWHOLES";
  const std::string sRodName = "ROD";
  const std::string sRodHolesCSName = "FV0RODHOLES";
  const std::string sContainerName = "CONTAINER";

private:
  /// Copy constructor.
  AliFV0(const AliFV0 &AliFV0);
  /// assignment operator
  AliFV0 &operator=(const AliFV0 &source); // assignment operator

  /// Initialize the Geometry.
  void initializeGeometry();

  /// Initialize maps with Geometry information.
  void initializeMaps();

  /// Initialize vectors with Geometry information.
  void initializeVectors();

  /// Initialize common transformations.
  void initializeTransformations();

  /// Initialize the cell ring radii.
  void initializeCellRingRadii();

  /// Initialize sector transformations.
  void initializeSectorTransformations();

  /// Initialize fiber volume radii.
  void initializeFiberVolumeRadii();

  /// Initialize fiber mediums.
  void initializeFiberMedium();

  /// Initialize the radii of the screw and rod positions.
  void initializeScrewAndRodRadii();

  /// Initialize the screw type medium.
  void initializeScrewTypeMedium();

  /// Initialize the rod type medium.
  void initializeRodTypeMedium();

  /// Add a screw property set to the collection of total screws.
  /// \param  screwTypeID The screw type ID.
  /// \param  iRing       The ring number.
  /// \param  phi         Azimuthal angle of the screw location.
  void addScrewProperties(int screwTypeID, int iRing, float phi);

  /// Add a rod property set to the collection of total rods.
  /// \param  rodTypeID The rod type ID.
  /// \param  iRing     The ring number.
  void addRodProperties(int rodTypeID, int iRing);

  /// Initialize the position and dimension for every screw and rod.
  void initializeScrewAndRodPositionsAndDimensions();

  /// Initialize the sensitive volumes.
  void initializeSensVols();

  /// Initialize the non-sensitive volumes.
  void initializeNonSensVols();

  /// Initialize a composite shape of all screw holes. This shape is removed
  /// from all volumes that the screws are passing through to avoid overlaps.
  void initializeScrewHoles();

  /// Initialize a composite shape of all rod holes. This shape is removed
  /// from all volumes that the rods are passing through to avoid overlaps.
  void initializeRodHoles();

  /// Initialize cell volumes with a specified thickness and medium.
  /// \param  cellType    The type of the cells.
  /// \param  zThicknes   The thickness of the cells.
  /// \param  medium      The medium of the cells.
  /// \param  isSensitive Specifies if the cells are sensitive volumes.
  void initializeCells(const std::string &cellType, const float zThickness, const TGeoMedium *medium, bool isSensitive);

  /// Initialize scintillator cell volumes.
  void initializeScintCells();

  /// Initialize plastic cell volumes for optical fiber support.
  void initializePlasticCells();

  /// Initialize PMTs.
  void initializePmts();

  /// Initialize volumes equivalent to the optical fibers.
  void initializeFibers();

  /// Initialize the screw volumes.
  void initializeScrews();

  /// Initialize the rod volumes.
  void initializeRods();

  /// Initialize the metal container volume.
  void initializeMetalContainer();

  /// Assemble the sensitive volumes.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleSensVols(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the nonsensitive volumes.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleNonSensVols(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the scintillator sectors.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleScintSectors(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the plastice sectors.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assemblePlasticSectors(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the PMTs.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assemblePmts(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the optical fibers.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleFibers(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the screwss.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleScrews(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the rods.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleRods(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Assemble the metal container.
  /// \param vFV0Right The right FV0 volume.
  /// \param vFV0Left  The left FV0 volume.
  void assembleMetalContainer(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const;

  /// Build sector assembly of specified type.
  /// \param  cellName  The type of the cells in the sector assembly.
  /// \return The sector assembly.
  TGeoVolumeAssembly *buildSectorAssembly(const std::string &cellName) const;

  /// Build a sector of specified type and number.
  /// \param  cellType  The type of the cells in the sector.
  /// \param  iSector   The numbering of the sector.
  /// \return The sector.
  TGeoVolumeAssembly *buildSector(const std::string &cellType, int iSector) const;

  /// Create the shape for a specified screw.
  /// \param  shapeName   The name of the shape.
  /// \param  screwTypeID The number of the screw type.
  /// \param  xEpsilon    Shrinks or expands the x dimensions of the screw shape.
  /// \param  yEpsilon    Shrinks or expands the y dimensions of the screw shape.
  /// \param  zEpsilon    Shrinks or expands the z dimensions of the screw shape.
  /// \return The screw shape.
  TGeoShape *createScrewShape(const std::string &shapeName, int screwTypeID, float xEpsilon = 0, float yEpsilon = 0,
                              float zEpsilon = 0) const;

  /// Create the shape for a specified rod.
  /// \param  shapeName The name of the shape.
  /// \param  rodTypeID The number of the rod type.
  /// \param  xEpsilon  Shrinks or expands the x dimensions of the rod shape.
  /// \param  yEpsilon  Shrinks or expands the y dimensions of the rod shape.
  /// \param  zEpsilon  Shrinks or expands the z dimensions of the rod shape.
  /// \return The rod shape.
  TGeoShape *createRodShape(const std::string &shapeName, int rodTypeID, float xEpsilon = 0, float yEpsilon = 0,
                            float zEpsilon = 0) const;

  /// Helper function for creating and registering a TGeoTranslation.
  /// \param  name  The name of the translation.
  /// \param  dx    Translation dx.
  /// \param  dy    Translation dy.
  /// \param  dz    Translation dz.
  /// \return The newly created and registered TGeoTranslation.
  TGeoTranslation *createAndRegisterTrans(const std::string &name, double dx = 0, double dy = 0, double dz = 0) const;

  /// Helper function for creating and registering a TGeoRotation.
  /// \param  name  The name of the rotation.
  /// \param  dx    Translation phi.
  /// \param  dy    Translation theta.
  /// \param  dz    Translation psi.
  /// \return The newly created and registered TGeoRotation.
  TGeoRotation *createAndRegisterRot(const std::string &name, double phi = 0, double theta = 0, double psi = 0) const;

  /// Helper function for creating volume names.
  /// \param  volumeType  A string that will be included in the volume name.
  /// \param  number      A number, e.g. an ID, that is included in the name. A negative number is omitted.
  /// \return The volume name.
  const std::string createVolumeName(const std::string &volumeType, int number = -1) const;

  /// Utility methods
  void initializeCellCenters();    ///< To be called in constructor to initialize mCellCenter
  void initializeReadoutCenters(); ///< To be called in constructor to initialize mReadoutCenter

  std::vector<std::string> mSensitiveVolumeNames; ///< The names of all the sensitive volumes

  /// Average ring radii
  ///
  /// index 0 -> ring 1 min, index 1 -> ring 1 max and ring 2 min, ... index 5 -> ring 5 max
  std::vector<float> mRAvgRing;
  std::vector<float> mRMinScintillator; ///< Inner radii of scintillator rings (.at(0) -> ring 1, .at(4) -> ring 5)
  std::vector<float> mRMaxScintillator; ///< Outer radii of scintillator rings (.at(0) -> ring 1, .at(4) -> ring 5)
  std::vector<float> mRMinFiber;        ///< Inner radii of fiber volumes (.at(0) -> fiber 1)
  std::vector<float> mRMaxFiber;        ///< Outer radii of fiber volumes (.at(0) -> fiber 1)

  /// Medium of the fiber volumes
  /// .at(n) -> medium of the n:th fiber starting from the middle.
  std::vector<TGeoMedium *> mMediumFiberRings;
  std::vector<TGeoMedium *> mMediumFiberPMTs;

  std::vector<float> mRScrewAndRod; ///< Radii of the screw and rod positions

  std::vector<float> mDrMinScrews; ///< Radii of the thinner part of the screws
  std::vector<float> mDrMaxScrews; ///< Radii of the thicker part of the screws
  std::vector<float> mDzMaxScrews; ///< Length of the thinner part of the screws
  std::vector<float> mDzMinScrews; ///< Length of the thicker part of the screws

  std::vector<float> mRScrews;    ///< Radial distance to the screw locations
  std::vector<int> mScrewTypeIDs; ///< The type ID of each screw (.at(n) -> type ID of screw no. n)

  std::vector<float> mDxMinRods; ///< Width of the thinner part of the rods
  std::vector<float> mDxMaxRods; ///< Width of the thicker part of the rods
  std::vector<float> mDyMinRods; ///< Height of the thinner part of the rods
  std::vector<float> mDyMaxRods; ///< Height of the thicker part of the rods
  std::vector<float> mDzMaxRods; ///< Length of the thinner part of the rods
  std::vector<float> mDzMinRods; ///< Length of the thicker part of the rods
  std::vector<int> mRodTypeIDs;  ///< The type ID of each rod (.at(n) -> type ID of rod no. n)

  std::vector<TGeoMatrix *> mSectorTrans;      ///< Transformations of sectors (.at(0) -> sector 1)
  std::vector<std::vector<float>> mScrewPos;   ///< xyz-coordinates of all the screws
  std::vector<std::vector<float>> mRodPos;     ///< xyz-coordinates of all the rods
  std::vector<TGeoMedium *> mMediumScrewTypes; ///< Medium of the screw types
  std::vector<TGeoMedium *> mMediumRodTypes;   ///< Medium of the rod types

  const int mGeometryType;                          ///< The type of the geometry.
  std::map<EGeoComponent, bool> mEnabledComponents; ///< Map of the enabled state of all Geometry components
  TGeoMatrix *mLeftTransformation;                  ///< Transformation for the left part of the detector
  TGeoMatrix *mRightTransformation;                 ///< Transformation for the right part of the detector

  /// Utility arrays derived from constants
  std::array<TVector3, 40> mCellCenter;    ///< Center of each scintillator cell  !!! Size of array is sNumberOfCells  (=40)
  std::array<TVector3, 48> mReadoutCenter; ///< Similar to mCellCenter, cells in r5 are additionally divided  !!! Size of array is sNumberOfReadoutChannels (=48)

  ClassDef(AliFV0, 1);
};
#endif
