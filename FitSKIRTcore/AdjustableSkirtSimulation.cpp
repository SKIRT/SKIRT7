/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AdjustableSkirtSimulation.hpp"

#include "DoublePropertyHandler.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FitScheme.hpp"
#include "InstrumentFrame.hpp"
#include "InstrumentSystem.hpp"
#include "Log.hpp"
#include "MultiFrameInstrument.hpp"
#include "ParallelFactory.hpp"
#include "Simulation.hpp"
#include "StellarSystem.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"
#include "XmlHierarchyCreator.hpp"

#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>

////////////////////////////////////////////////////////////////////

AdjustableSkirtSimulation::AdjustableSkirtSimulation()
    :_units(0)
{
}

////////////////////////////////////////////////////////////////////

AdjustableSkirtSimulation::~AdjustableSkirtSimulation()
{
    delete  _units;
}

////////////////////////////////////////////////////////////////////

void AdjustableSkirtSimulation::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // if the file does not exist as specified, try adding the .ski extension
    QString filepath = find<FilePaths>()->input(_skiName);
    if (!QFile::exists(filepath) && !filepath.toLower().endsWith(".ski")) filepath += ".ski";
    if (!QFile::exists(filepath))
        throw FATALERROR("This ski file does not exist: " + filepath);

    // read the file into our byte array
    QFile infile(filepath);
    if (infile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        _skiContent = infile.readAll();
        infile.close();
    }
    if (_skiContent.isEmpty())
        throw FATALERROR("Could not read the ski file " + filepath);

    // construct the simulation from the default ski content; use shared pointer for automatic clean-up
    find<Log>()->info("Constructing simulation hierarchy from ski file " + filepath + "...");
    XmlHierarchyCreator creator;
    QSharedPointer<Simulation> simulation( creator.createHierarchy<Simulation>(adjustedSkiContent()) );

    // setup any simulation attributes that are not loaded from the ski content
    // copy file paths
    FilePaths* myfilepaths = find<FilePaths>();
    FilePaths* itsfilepaths = simulation->filePaths();
    itsfilepaths->setOutputPrefix(myfilepaths->outputPrefix() + "_def");
    itsfilepaths->setInputPath(myfilepaths->inputPath());
    itsfilepaths->setOutputPath(myfilepaths->outputPath());
    // copy number of threads
    int threads = find<FitScheme>()->parallelThreadCount();
    if (threads > 0) simulation->parallelFactory()->setMaxThreadCount(threads);
    // suppress log messages
    simulation->log()->setLowestLevel(Log::Error);

    // output a ski file and a latex file reflecting this simulation for later reference
    //XmlHierarchyWriter writer1;
    //writer1.writeHierarchy(simulation.data(), itsfilepaths->output("params.xml"));
    //LatexHierarchyWriter writer2;
    //writer2.writeHierarchy(simulation.data(), itsfilepaths->output("params.tex"));

    // run the simulation
    find<Log>()->info("Performing the simulation with default attribute values...");
    simulation->setup();

    // steal the frames and instrument name from the simulation
    InstrumentSystem* instrSys = simulation->find<InstrumentSystem>();
    MultiFrameInstrument* multiframe = instrSys->find<MultiFrameInstrument>();
    StellarSystem* stelsys = simulation->find<StellarSystem>();
    _nframes = multiframe->frames().length();
    _instrname = multiframe->instrumentName();
    _ncomponents = stelsys->Ncomp();
    find<Log>()->info("Number of frames in this simulation: " + QString::number(_nframes));
    find<Log>()->info("Number of stellar components in this simulation: " + QString::number(_ncomponents));
    find<Log>()->info("Instrument name is : " + _instrname);
    foreach (InstrumentFrame* insFrame, multiframe->frames()){
        _xpress.append(2.00*insFrame->extentX()*(insFrame->pixelsX()-1));
        _ypress.append(2.00*insFrame->extentY()*(insFrame->pixelsY()-1));
    }    // steal the frames and instrument name from the simulation
}

////////////////////////////////////////////////////////////////////

void AdjustableSkirtSimulation::setSkiName(QString value)
{
    _skiName = value;
}

////////////////////////////////////////////////////////////////////

QString AdjustableSkirtSimulation::skiName() const
{
    return _skiName;
}

////////////////////////////////////////////////////////////////////

void AdjustableSkirtSimulation::performWith(AdjustableSkirtSimulation::ReplacementDict replacements,
                                            QString prefix)
{
    // construct the simulation from the ski content; use shared pointer for automatic clean-up
    XmlHierarchyCreator creator;
    QSharedPointer<Simulation> simulation( creator.createHierarchy<Simulation>(
                                                adjustedSkiContent(replacements)) );

    // setup any simulation attributes that are not loaded from the ski content
    // copy file paths
    FilePaths* myfilepaths = find<FilePaths>();
    FilePaths* itsfilepaths = simulation->filePaths();
    itsfilepaths->setOutputPrefix(myfilepaths->outputPrefix() + "_" + prefix);
    itsfilepaths->setInputPath(myfilepaths->inputPath());
    itsfilepaths->setOutputPath(myfilepaths->outputPath());
    // copy number of threads
    int threads = find<FitScheme>()->parallelThreadCount();
    if (threads > 0) simulation->parallelFactory()->setMaxThreadCount(threads);
    // suppress log messages
    simulation->log()->setLowestLevel(Log::Error);
    // run the simulation
    simulation->setupAndRun();
}

////////////////////////////////////////////////////////////////////

QByteArray AdjustableSkirtSimulation::adjustedSkiContent(AdjustableSkirtSimulation::ReplacementDict replacements)
{
    QByteArray in, out;

    // process square brackets
    {
        in = _skiContent;

        // loop over input; index points at the next byte to be processed
        int index = 0;
        while (true)
        {
            // look for left bracket
            int leftindex = in.indexOf('[', index);
            if (leftindex<0) break;

            // look for right bracket
            int rightindex = in.indexOf(']', leftindex+1);
            if (rightindex<0) throw FATALERROR("Square brackets not balanced in ski file");

            // copy everything up to the left bracket and put the input index beyond the right bracket
            out += in.mid(index, leftindex-index);
            index = rightindex+1;

            // get the segment containing the attribute value, without the brackets
            QByteArray segment = in.mid(leftindex+1, rightindex-leftindex-1);
            if (segment.contains('[')) throw FATALERROR("Square brackets not balanced in ski file");

            // look for colon
            int colonindex = segment.indexOf(':');
            if (colonindex<0) throw FATALERROR("Square brackets don't enclose colon in ski file");

            // get the label
            QByteArray label = segment.left(colonindex);

            // if the label is in the replacements dict, insert the replacement value, otherwise copy the default value
            if (replacements.contains(label))
                out += DoublePropertyHandler::convertDoubletoString(
                           replacements[label].first, replacements[label].second, _units);
            else
                out += segment.mid(colonindex+1);
        }

        // no more brackets -> copy the rest of the input
        out += in.mid(index);
        if (out.contains(']')) throw FATALERROR("Square brackets not balanced in ski file");
    }
    return out;
}

////////////////////////////////////////////////////////////////////

int AdjustableSkirtSimulation::ncomponents() const
{
    return _ncomponents;
}

////////////////////////////////////////////////////////////////////

int AdjustableSkirtSimulation::nframes() const
{
    return _nframes;
}

////////////////////////////////////////////////////////////////////

QString AdjustableSkirtSimulation::instrname() const
{
    return _instrname;
}

////////////////////////////////////////////////////////////////////

double AdjustableSkirtSimulation::wavelength(int ind) const
{
    return _wavelengthgrid[ind];
}

////////////////////////////////////////////////////////////////////

double AdjustableSkirtSimulation::xpress(int ind) const
{
    return _xpress[ind] ;
}

////////////////////////////////////////////////////////////////////

double AdjustableSkirtSimulation::ypress(int ind) const
{
    return _ypress[ind] ;
}

////////////////////////////////////////////////////////////////////
