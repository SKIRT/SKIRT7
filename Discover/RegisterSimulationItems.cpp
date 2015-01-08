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
#include "AdaptiveMeshDustGridStructure.hpp"
#include "AdaptiveMeshGeometry.hpp"
#include "AdaptiveMeshStellarComp.hpp"
#include "AllCellsDustLib.hpp"
#include "AxGeometry.hpp"
#include "AxSpheDustGridStructure.hpp"
#include "Benchmark1DDustMix.hpp"
#include "Benchmark2DDustMix.hpp"
#include "BinTreeDustGridStructure.hpp"
#include "BlackBodySED.hpp"
#include "BolLuminosityStellarCompNormalization.hpp"
#include "BruzualCharlotSED.hpp"
#include "ClumpyGeometry.hpp"
#include "CMamForsteriteGrainComposition.hpp"
#include "CompDustDistribution.hpp"
#include "ConfigurableDustMix.hpp"
#include "CubicSplineSmoothingKernel.hpp"
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
#include "DustGridStructure.hpp"
#include "DustMassDustCompNormalization.hpp"
#include "DustMixPopulation.hpp"
#include "EdgeOnDustCompNormalization.hpp"
#include "EinastoGeometry.hpp"
#include "EnstatiteGrainComposition.hpp"
#include "ExpDiskGeometry.hpp"
#include "ExtragalacticUnits.hpp"
#include "FaceOnDustCompNormalization.hpp"
#include "FileGrainComposition.hpp"
#include "FilePaths.hpp"
#include "FileSED.hpp"
#include "FileWavelengthGrid.hpp"
#include "ForsteriteGrainComposition.hpp"
#include "FrameInstrument.hpp"
#include "FullInstrument.hpp"
#include "GammaGeometry.hpp"
#include "GaussianGeometry.hpp"
#include "GenGeometry.hpp"
#include "GenLinCubDustGridStructure.hpp"
#include "GreyBodyDustEmissivity.hpp"
#include "IdenticalAssigner.hpp"
#include "InstrumentFrame.hpp"
#include "InstrumentSystem.hpp"
#include "InterstellarDustMix.hpp"
#include "KuruczSED.hpp"
#include "LinAxDustGridStructure.hpp"
#include "LinCubDustGridStructure.hpp"
#include "LinSpheDustGridStructure.hpp"
#include "LogLinAxDustGridStructure.hpp"
#include "LogLinAxSpheDustGridStructure.hpp"
#include "LogNormalGrainSizeDistribution.hpp"
#include "LogPowAxDustGridStructure.hpp"
#include "LogSpheDustGridStructure.hpp"
#include "LogWavelengthGrid.hpp"
#include "LuminosityStellarCompNormalization.hpp"
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
#include "OctTreeDustGridStructure.hpp"
#include "OffsetGeometry.hpp"
#include "OligoDustSystem.hpp"
#include "OligoMonteCarloSimulation.hpp"
#include "OligoStellarComp.hpp"
#include "OligoWavelengthGrid.hpp"
#include "PanDustSystem.hpp"
#include "PanMonteCarloSimulation.hpp"
#include "PanStellarComp.hpp"
#include "ParticleTreeDustGridStructure.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PegaseSED.hpp"
#include "PerspectiveInstrument.hpp"
#include "PlummerGeometry.hpp"
#include "PointGeometry.hpp"
#include "PolarizedGraphiteGrainComposition.hpp"
#include "PolarizedSilicateGrainComposition.hpp"
#include "PowAxDustGridStructure.hpp"
#include "PowCubDustGridStructure.hpp"
#include "PowSpheDustGridStructure.hpp"
#include "PowerLawGrainSizeDistribution.hpp"
#include "PseudoSersicGeometry.hpp"
#include "QuasarSED.hpp"
#include "RadialDustCompNormalization.hpp"
#include "Random.hpp"
#include "ReadFitsGeometry.hpp"
#include "RingGeometry.hpp"
#include "RotateGeometry.hpp"
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
#include "SingleGrainSizeDistribution.hpp"
#include "SmoothingKernel.hpp"
#include "SpheGeometry.hpp"
#include "SphericalHoleGeometry.hpp"
#include "SpheroidalGeometry.hpp"
#include "SpiralStructureGeometry.hpp"
#include "StaggeredAssigner.hpp"
#include "StarburstSED.hpp"
#include "StellarSurfaceGeometry.hpp"
#include "StellarSystem.hpp"
#include "StellarUnits.hpp"
#include "SunSED.hpp"
#include "TTauriDiskGeometry.hpp"
#include "TorusGeometry.hpp"
#include "TransientDustEmissivity.hpp"
#include "TriaxialGeometry.hpp"
#include "Trust1Geometry.hpp"
#include "Trust2Geometry.hpp"
#include "Trust6Geometry.hpp"
#include "Trust7aGeometry.hpp"
#include "Trust7bGeometry.hpp"
#include "TrustDustMix.hpp"
#include "TrustGraphiteGrainComposition.hpp"
#include "TrustMeanDustMix.hpp"
#include "TrustNeutralPAHGrainComposition.hpp"
#include "TrustSilicateGrainComposition.hpp"
#include "TwoPhaseDustGridStructure.hpp"
#include "UniformCuboidGeometry.hpp"
#include "UniformSmoothingKernel.hpp"
#include "VoronoiDustDistribution.hpp"
#include "VoronoiDustGridStructure.hpp"
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

    add<Simulation>(false);
    add<Random>();
    add<Units>(false);
    add<SIUnits>();
    add<StellarUnits>();
    add<ExtragalacticUnits>();

    add<MonteCarloSimulation>(false);
    add<OligoMonteCarloSimulation>();
    add<PanMonteCarloSimulation>();

    add<InstrumentSystem>();
    add<Instrument>(false);
    add<SEDInstrument>();
    add<FrameInstrument>();
    add<SimpleInstrument>();
    add<FullInstrument>();
    add<PerspectiveInstrument>();
    add<MultiFrameInstrument>();
    add<InstrumentFrame>();

    add<WavelengthGrid>(false);
    add<OligoWavelengthGrid>();
    add<PanWavelengthGrid>(false);
    add<LogWavelengthGrid>();
    add<NestedLogWavelengthGrid>();
    add<FileWavelengthGrid>();

    add<StellarSystem>();
    add<StellarComp>(false);
    add<GeometricStellarComp>(false);
    add<OligoStellarComp>();
    add<PanStellarComp>();
    add<StellarCompNormalization>(false);
    add<BolLuminosityStellarCompNormalization>();
    add<LuminosityStellarCompNormalization>();
    add<SPHStellarComp>();
    add<AdaptiveMeshStellarComp>();
    add<VoronoiStellarComp>();

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
    add<MGEGeometry>();
    add<TTauriDiskGeometry>();
    add<RingGeometry>();
    add<TorusGeometry>();
    add<SpheroidalGeometry>();
    add<GenGeometry>(false);
    add<NetzerAccretionDiskGeometry>();
    add<StellarSurfaceGeometry>();
    add<SolarPatchGeometry>();
    add<UniformCuboidGeometry>();
    add<Trust1Geometry>();
    add<Trust2Geometry>();
    add<Trust6Geometry>();
    add<Trust7aGeometry>();
    add<Trust7bGeometry>();
    add<SPHGeometry>();
    add<AdaptiveMeshGeometry>();
    add<VoronoiGeometry>();
    add<OffsetGeometry>();
    add<RotateGeometry>();
    add<ClumpyGeometry>();
    add<TriaxialGeometry>();
    add<SphericalHoleGeometry>();
    add<SpiralStructureGeometry>();
    add<ReadFitsGeometry>();

    add<SmoothingKernel>(false);
    add<UniformSmoothingKernel>();
    add<CubicSplineSmoothingKernel>();

    add<DustSystem>(false);
    add<OligoDustSystem>();
    add<PanDustSystem>();

    add<DustComp>();

    add<DustCompNormalization>(false);
    add<DustMassDustCompNormalization>();
    add<RadialDustCompNormalization>();
    add<FaceOnDustCompNormalization>();
    add<EdgeOnDustCompNormalization>();
    add<XDustCompNormalization>();
    add<YDustCompNormalization>();
    add<ZDustCompNormalization>();

    add<MeshDustComponent>();

    add<DustDistribution>(false);
    add<CompDustDistribution>();
    add<SPHDustDistribution>();
    add<AdaptiveMeshDustDistribution>();
    add<VoronoiDustDistribution>();

    add<AdaptiveMeshFile>(false);
    add<AdaptiveMeshAsciiFile>();
    add<AdaptiveMeshAmrvacFile>();

    add<VoronoiMeshFile>(false);
    add<VoronoiMeshAsciiFile>();

    add<DustGridStructure>(false);
    add<LinSpheDustGridStructure>();
    add<PowSpheDustGridStructure>();
    add<LogSpheDustGridStructure>();
    add<LinAxDustGridStructure>();
    add<PowAxDustGridStructure>();
    add<LogLinAxDustGridStructure>();
    add<LogPowAxDustGridStructure>();
    add<LogLinAxSpheDustGridStructure>();
    add<LinCubDustGridStructure>();
    add<GenLinCubDustGridStructure>();
    add<PowCubDustGridStructure>();
    add<TwoPhaseDustGridStructure>();
    add<OctTreeDustGridStructure>();
    add<BinTreeDustGridStructure>();
    add<ParticleTreeDustGridStructure>();
    add<VoronoiDustGridStructure>();
    add<AdaptiveMeshDustGridStructure>();

    add<DustMix>(false);
    add<DraineLiDustMix>();
    add<InterstellarDustMix>();
    add<MeanZubkoDustMix>();
    add<Benchmark1DDustMix>();
    add<Benchmark2DDustMix>();
    add<TrustMeanDustMix>();
    add<MRNDustMix>();
    add<WeingartnerDraineDustMix>();
    add<ZubkoDustMix>();
    add<TrustDustMix>();
    add<ConfigurableDustMix>();

    add<GrainComposition>(false);
    add<DraineGraphiteGrainComposition>();
    add<DraineSilicateGrainComposition>();
    add<DraineNeutralPAHGrainComposition>();
    add<DraineIonizedPAHGrainComposition>();
    add<MieSilicateGrainComposition>();
    add<MinSilicateGrainComposition>();
    add<EnstatiteGrainComposition>();
    add<ForsteriteGrainComposition>();
    add<PolarizedGraphiteGrainComposition>();
    add<PolarizedSilicateGrainComposition>();
    add<TrustGraphiteGrainComposition>();
    add<TrustSilicateGrainComposition>();
    add<TrustNeutralPAHGrainComposition>();
    add<DustEmGrainComposition>();
    add<FileGrainComposition>();
    add<CMamForsteriteGrainComposition>();

    add<GrainSizeDistribution>(false);
    add<PowerLawGrainSizeDistribution>();
    add<ModifiedPowerLawGrainSizeDistribution>();
    add<LogNormalGrainSizeDistribution>();
    add<ModifiedLogNormalGrainSizeDistribution>();
    add<SingleGrainSizeDistribution>();
    add<ZubkoGraphiteGrainSizeDistribution>();
    add<ZubkoSilicateGrainSizeDistribution>();
    add<ZubkoPAHGrainSizeDistribution>();

    add<DustMixPopulation>();

    add<DustEmissivity>(false);
    add<GreyBodyDustEmissivity>();
    add<TransientDustEmissivity>();

    add<DustLib>(false);
    add<AllCellsDustLib>();
    add<Dim1DustLib>();
    add<Dim2DustLib>();

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

    add<ProcessCommunicator>(false);
    add<PeerToPeerCommunicator>();

    add<ProcessAssigner>(false);
    add<IdenticalAssigner>();
    add<StaggeredAssigner>();
    add<SequentialAssigner>();
}

////////////////////////////////////////////////////////////////////
