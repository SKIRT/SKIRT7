/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "RegisterSimulationItems.hpp"
#include "SimulationItemRegistry.hpp"

// add new items in alphabetical order

#include "AdaptiveMeshAmrvacFile.hpp"
#include "AdaptiveMeshAsciiFile.hpp"
#include "AdaptiveMeshDustDistribution.hpp"
#include "AdaptiveMeshDustGrid.hpp"
#include "AdaptiveMeshGeometry.hpp"
#include "AdaptiveMeshStellarComp.hpp"
#include "AllCellsDustLib.hpp"
#include "AmHydrocarbonGrainComposition.hpp"
#include "AxGeometry.hpp"
#include "Benchmark1DDustMix.hpp"
#include "Benchmark2DDustMix.hpp"
#include "BinTreeDustGrid.hpp"
#include "BlackBodySED.hpp"
#include "BolLuminosityStellarCompNormalization.hpp"
#include "BoxClipGeometryDecorator.hpp"
#include "BoxDustGrid.hpp"
#include "BrokenExpDiskGeometry.hpp"
#include "BruzualCharlotSED.hpp"
#include "BruzualCharlotSEDFamily.hpp"
#include "CartesianDustGrid.hpp"
#include "ClumpyGeometryDecorator.hpp"
#include "CombineGeometryDecorator.hpp"
#include "CompDustDistribution.hpp"
#include "ConfigurableDustMix.hpp"
#include "ConicalShellGeometry.hpp"
#include "CubBackgroundGeometry.hpp"
#include "CubicSplineSmoothingKernel.hpp"
#include "Cylinder2DDustGrid.hpp"
#include "CylinderDustGrid.hpp"
#include "CylindricalClipGeometryDecorator.hpp"
#include "Dim1DustLib.hpp"
#include "Dim2DustLib.hpp"
#include "DraineGraphiteGrainComposition.hpp"
#include "DraineIonizedPAHGrainComposition.hpp"
#include "DraineLiDustMix.hpp"
#include "DraineNeutralPAHGrainComposition.hpp"
#include "DraineSilicateGrainComposition.hpp"
#include "DustComp.hpp"
#include "DustCompNormalization.hpp"
#include "DustDistribution.hpp"
#include "DustEmGrainComposition.hpp"
#include "DustGrid.hpp"
#include "DustMassDustCompNormalization.hpp"
#include "DustMixPopulation.hpp"
#include "EdgeOnDustCompNormalization.hpp"
#include "EinastoGeometry.hpp"
#include "ElectronDustMix.hpp"
#include "EnstatiteGrainComposition.hpp"
#include "ExpDiskGeometry.hpp"
#include "ExtragalacticUnits.hpp"
#include "FaceOnDustCompNormalization.hpp"
#include "FileGrainComposition.hpp"
#include "FilePaths.hpp"
#include "FileSED.hpp"
#include "FileWavelengthGrid.hpp"
#include "FoamGeometryDecorator.hpp"
#include "ForsteriteGrainComposition.hpp"
#include "FrameInstrument.hpp"
#include "FullInstrument.hpp"
#include "GammaGeometry.hpp"
#include "GaussianGeometry.hpp"
#include "GenGeometry.hpp"
#include "GreyBodyDustEmissivity.hpp"
#include "IdenticalAssigner.hpp"
#include "InstrumentFrame.hpp"
#include "InstrumentSystem.hpp"
#include "InterstellarDustMix.hpp"
#include "KuruczSED.hpp"
#include "LaserGeometry.hpp"
#include "LinMesh.hpp"
#include "LogMesh.hpp"
#include "LogNormalGrainSizeDistribution.hpp"
#include "LogWavelengthGrid.hpp"
#include "LuminosityStellarCompNormalization.hpp"
#include "MappingsSEDFamily.hpp"
#include "MGEGeometry.hpp"
#include "MRNDustMix.hpp"
#include "MarastonSED.hpp"
#include "MappingsSED.hpp"
#include "MeanZubkoDustMix.hpp"
#include "MeshDustComponent.hpp"
#include "MieSilicateGrainComposition.hpp"
#include "MinSilicateGrainComposition.hpp"
#include "ModifiedLogNormalGrainSizeDistribution.hpp"
#include "ModifiedPowerLawGrainSizeDistribution.hpp"
#include "MultiFrameInstrument.hpp"
#include "NestedLogWavelengthGrid.hpp"
#include "NetzerAccretionDiskGeometry.hpp"
#include "OctTreeDustGrid.hpp"
#include "OffsetGeometryDecorator.hpp"
#include "OligoDustSystem.hpp"
#include "OligoMonteCarloSimulation.hpp"
#include "OligoStellarComp.hpp"
#include "OligoWavelengthGrid.hpp"
#include "PanDustSystem.hpp"
#include "PanMonteCarloSimulation.hpp"
#include "PanStellarComp.hpp"
#include "PanWavelengthGrid.hpp"
#include "ParticleTreeDustGrid.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PegaseSED.hpp"
#include "PerspectiveInstrument.hpp"
#include "PlummerGeometry.hpp"
#include "PointGeometry.hpp"
#include "PolarizedGraphiteGrainComposition.hpp"
#include "PolarizedSilicateGrainComposition.hpp"
#include "PowMesh.hpp"
#include "PowerLawGrainSizeDistribution.hpp"
#include "PseudoSersicGeometry.hpp"
#include "QuasarSED.hpp"
#include "RadialDustCompNormalization.hpp"
#include "Random.hpp"
#include "RandomAssigner.hpp"
#include "ReadFitsGeometry.hpp"
#include "RingGeometry.hpp"
#include "RotateGeometryDecorator.hpp"
#include "SEDInstrument.hpp"
#include "SequentialAssigner.hpp"
#include "SIUnits.hpp"
#include "SolarPatchGeometry.hpp"
#include "SPHDustDistribution.hpp"
#include "SPHGeometry.hpp"
#include "SPHStellarComp.hpp"
#include "SersicGeometry.hpp"
#include "ShellGeometry.hpp"
#include "SimpleInstrument.hpp"
#include "SimpleOligoDustMix.hpp"
#include "SingleGrainSizeDistribution.hpp"
#include "SmoothingKernel.hpp"
#include "SpectralLuminosityStellarCompNormalization.hpp"
#include "SpheBackgroundGeometry.hpp"
#include "SpheGeometry.hpp"
#include "Sphere1DDustGrid.hpp"
#include "Sphere2DDustGrid.hpp"
#include "SphereDustGrid.hpp"
#include "SphericalAdaptiveMeshDustDistribution.hpp"
#include "SphericalClipGeometryDecorator.hpp"
#include "SpheroidalGeometryDecorator.hpp"
#include "SpiralStructureGeometryDecorator.hpp"
#include "StaggeredAssigner.hpp"
#include "StarburstSED.hpp"
#include "StellarSurfaceGeometry.hpp"
#include "StellarSystem.hpp"
#include "StellarUnits.hpp"
#include "SunSED.hpp"
#include "SymPowMesh.hpp"
#include "TTauriDiskGeometry.hpp"
#include "ThemisDustMix.hpp"
#include "TorusGeometry.hpp"
#include "TransientDustEmissivity.hpp"
#include "TreeDustGrid.hpp"
#include "TriaxialGeometryDecorator.hpp"
#include "Trust1Geometry.hpp"
#include "Trust2Geometry.hpp"
#include "Trust6Geometry.hpp"
#include "Trust7aGeometry.hpp"
#include "Trust7bGeometry.hpp"
#include "TrustDustMix.hpp"
#include "TrustGraphiteGrainComposition.hpp"
#include "TrustMeanDustMix.hpp"
#include "TrustNeutralPAHGrainComposition.hpp"
#include "TrustPolarizedMeanDustMix.hpp"
#include "TrustSilicateGrainComposition.hpp"
#include "TwoPhaseDustGrid.hpp"
#include "UniformCuboidGeometry.hpp"
#include "UniformSmoothingKernel.hpp"
#include "VoronoiDustDistribution.hpp"
#include "VoronoiDustGrid.hpp"
#include "VoronoiGeometry.hpp"
#include "VoronoiMeshAsciiFile.hpp"
#include "VoronoiStellarComp.hpp"
#include "WeingartnerDraineDustMix.hpp"
#include "XDustCompNormalization.hpp"
#include "YDustCompNormalization.hpp"
#include "ZDustCompNormalization.hpp"
#include "ZubkoDustMix.hpp"
#include "ZubkoGraphiteGrainSizeDistribution.hpp"
#include "ZubkoPAHGrainSizeDistribution.hpp"
#include "ZubkoSilicateGrainSizeDistribution.hpp"

using namespace SimulationItemRegistry;

////////////////////////////////////////////////////////////////////

void RegisterSimulationItems::registerAll()
{
    // add new items in the order you want them to appear in choice lists for the user
    // !! provide an argument of "false" for abstract classes that shouldn't be instantiated

    // basic building blocks
    add<Simulation>(false);
    add<Random>();
    add<Units>(false);
    add<SIUnits>();
    add<StellarUnits>();
    add<ExtragalacticUnits>();

    // Monte Carlo simulations
    add<MonteCarloSimulation>(false);
    add<OligoMonteCarloSimulation>();
    add<PanMonteCarloSimulation>();

    // instruments
    add<InstrumentSystem>();
    add<Instrument>(false);
    add<SEDInstrument>();
    add<FrameInstrument>();
    add<SimpleInstrument>();
    add<FullInstrument>();
    add<PerspectiveInstrument>();
    add<MultiFrameInstrument>();
    add<InstrumentFrame>();

    // wavelength grids
    add<WavelengthGrid>(false);
    add<OligoWavelengthGrid>();
    add<PanWavelengthGrid>(false);
    add<LogWavelengthGrid>();
    add<NestedLogWavelengthGrid>();
    add<FileWavelengthGrid>();

    // stellar systems
    add<StellarSystem>();
    add<StellarComp>(false);
    add<GeometricStellarComp>(false);
    add<OligoStellarComp>();
    add<PanStellarComp>();
    add<StellarCompNormalization>(false);
    add<BolLuminosityStellarCompNormalization>();
    add<LuminosityStellarCompNormalization>();
    add<SpectralLuminosityStellarCompNormalization>();
    add<SPHStellarComp>();
    add<AdaptiveMeshStellarComp>();
    add<VoronoiStellarComp>();

    // isotropic geometries
    add<Geometry>(false);
    add<PointGeometry>();
    add<SpheGeometry>(false);
    add<PlummerGeometry>();
    add<GammaGeometry>();
    add<SersicGeometry>();
    add<PseudoSersicGeometry>();
    add<EinastoGeometry>();
    add<GaussianGeometry>();
    add<ShellGeometry>();
    add<AxGeometry>(false);
    add<ExpDiskGeometry>();
    add<BrokenExpDiskGeometry>();
    add<MGEGeometry>();
    add<TTauriDiskGeometry>();
    add<RingGeometry>();
    add<TorusGeometry>();
    add<ConicalShellGeometry>();
    add<GenGeometry>(false);
    add<UniformCuboidGeometry>();
    add<SPHGeometry>();
    add<AdaptiveMeshGeometry>();
    add<VoronoiGeometry>();
    add<ReadFitsGeometry>();

    // anistropic geometries
    add<LaserGeometry>();
    add<NetzerAccretionDiskGeometry>();
    add<StellarSurfaceGeometry>();
    add<SolarPatchGeometry>();
    add<SpheBackgroundGeometry>();
    add<CubBackgroundGeometry>();

    // TRUST benchmark geometries
    add<Trust1Geometry>();
    add<Trust2Geometry>();
    add<Trust6Geometry>();
    add<Trust7aGeometry>();
    add<Trust7bGeometry>();

    // geometry decorators
    add<OffsetGeometryDecorator>();
    add<RotateGeometryDecorator>();
    add<SpheroidalGeometryDecorator>();
    add<TriaxialGeometryDecorator>();
    add<SphericalClipGeometryDecorator>();
    add<CylindricalClipGeometryDecorator>();
    add<BoxClipGeometryDecorator>();
    add<SpiralStructureGeometryDecorator>();
    add<ClumpyGeometryDecorator>();
    add<CombineGeometryDecorator>();
    add<FoamGeometryDecorator>();

    // smoothing kernels
    add<SmoothingKernel>(false);
    add<UniformSmoothingKernel>();
    add<CubicSplineSmoothingKernel>();

    // dust systems
    add<DustSystem>(false);
    add<OligoDustSystem>();
    add<PanDustSystem>();

    // dust components and corresponding normalizations
    add<DustComp>();
    add<MeshDustComponent>();

    // dust component normalizations
    add<DustCompNormalization>(false);
    add<DustMassDustCompNormalization>();
    add<RadialDustCompNormalization>();
    add<FaceOnDustCompNormalization>();
    add<EdgeOnDustCompNormalization>();
    add<XDustCompNormalization>();
    add<YDustCompNormalization>();
    add<ZDustCompNormalization>();

    // dust distributions
    add<DustDistribution>(false);
    add<CompDustDistribution>();
    add<SPHDustDistribution>();
    add<AdaptiveMeshDustDistribution>();
    add<SphericalAdaptiveMeshDustDistribution>();
    add<VoronoiDustDistribution>();

    // mesh file representations
    add<AdaptiveMeshFile>(false);
    add<AdaptiveMeshAsciiFile>();
    add<AdaptiveMeshAmrvacFile>();
    add<VoronoiMeshFile>(false);
    add<VoronoiMeshAsciiFile>();

    // meshes for the dust grids
    add<Mesh>(false);
    add<MoveableMesh>(false);
    add<LinMesh>();
    add<PowMesh>();
    add<SymPowMesh>();
    add<AnchoredMesh>(false);
    add<LogMesh>();

    // dust grids
    add<DustGrid>(false);
    add<SphereDustGrid>(false);
    add<Sphere1DDustGrid>();
    add<Sphere2DDustGrid>();
    add<CylinderDustGrid>(false);
    add<Cylinder2DDustGrid>();
    add<BoxDustGrid>(false);
    add<CartesianDustGrid>();
    add<TwoPhaseDustGrid>();
    add<OctTreeDustGrid>();
    add<BinTreeDustGrid>();
    add<ParticleTreeDustGrid>();
    add<VoronoiDustGrid>();
    add<AdaptiveMeshDustGrid>();

    // dust mixtures
    add<DustMix>(false);
    add<InterstellarDustMix>();
    add<DraineLiDustMix>();
    add<MeanZubkoDustMix>();
    add<SimpleOligoDustMix>();
    add<Benchmark1DDustMix>();
    add<Benchmark2DDustMix>();
    add<TrustMeanDustMix>();
    add<TrustPolarizedMeanDustMix>();
    add<MRNDustMix>();
    add<WeingartnerDraineDustMix>();
    add<ZubkoDustMix>();
    add<TrustDustMix>();
    add<ThemisDustMix>();
    add<ConfigurableDustMix>();
    add<ElectronDustMix>();

    // grain compositions
    add<GrainComposition>(false);
    add<DraineGraphiteGrainComposition>();
    add<DraineSilicateGrainComposition>();
    add<DraineNeutralPAHGrainComposition>();
    add<DraineIonizedPAHGrainComposition>();
    add<MieSilicateGrainComposition>();
    add<MinSilicateGrainComposition>();
    add<AmHydrocarbonGrainComposition>();
    add<EnstatiteGrainComposition>();
    add<ForsteriteGrainComposition>();
    add<PolarizedGraphiteGrainComposition>();
    add<PolarizedSilicateGrainComposition>();
    add<TrustGraphiteGrainComposition>();
    add<TrustSilicateGrainComposition>();
    add<TrustNeutralPAHGrainComposition>();
    add<DustEmGrainComposition>();
    add<FileGrainComposition>();

    // grain size distributions
    add<GrainSizeDistribution>(false);
    add<PowerLawGrainSizeDistribution>();
    add<ModifiedPowerLawGrainSizeDistribution>();
    add<LogNormalGrainSizeDistribution>();
    add<ModifiedLogNormalGrainSizeDistribution>();
    add<SingleGrainSizeDistribution>();
    add<ZubkoGraphiteGrainSizeDistribution>();
    add<ZubkoSilicateGrainSizeDistribution>();
    add<ZubkoPAHGrainSizeDistribution>();

    // dust mix population
    add<DustMixPopulation>();

    // dust emissivity calculators
    add<DustEmissivity>(false);
    add<GreyBodyDustEmissivity>();
    add<TransientDustEmissivity>();

    // dust libraries
    add<DustLib>(false);
    add<AllCellsDustLib>();
    add<Dim1DustLib>();
    add<Dim2DustLib>();

    // stellar SEDs
    add<StellarSED>(false);
    add<BlackBodySED>();
    add<SunSED>();
    add<PegaseSED>();
    add<BruzualCharlotSED>();
    add<MarastonSED>();
    add<MappingsSED>();
    add<StarburstSED>();
    add<KuruczSED>();
    add<QuasarSED>();
    add<FileSED>();

    // SED families
    add<SEDFamily>(false);
    add<BruzualCharlotSEDFamily>();
    add<MappingsSEDFamily>();

    // process assigners
    add<ProcessAssigner>(false);
    add<IdenticalAssigner>();
    add<StaggeredAssigner>();
    add<SequentialAssigner>();
    add<RandomAssigner>();
}

////////////////////////////////////////////////////////////////////
