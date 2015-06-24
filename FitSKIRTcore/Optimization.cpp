/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Optimization.hpp"

#include "AdjustableSkirtSimulation.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MasterSlaveCommunicator.hpp"
#include "OligoFitScheme.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "ParameterRange.hpp"
#include "ParameterRanges.hpp"
#include "ReferenceImages.hpp"
#include "Units.hpp"
#include <QDir>

using namespace std;

//////////////////////////////////////////////////////////////////////

void MPIEvaluator(GAPopulation & p)
{
    Optimization *opt = (Optimization *)p.userData();
    opt->PopEvaluate(p);
}

//////////////////////////////////////////////////////////////////////

Optimization::Optimization()
    :_genome(0)
{
        _bestChi2=1e20;
        _consec=0;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
    FilePaths* path = find<FilePaths>();

    ParameterRanges* ranges = find<ParameterRanges>();
    foreach (ParameterRange* range, ranges->ranges())
    {
        GARealAlleleSet RealAllele(range->minimumValue(),range->maximumValue());
        _allelesetarray.add(RealAllele);
    }

    _genome= new GARealGenome(_allelesetarray);
    _genome->initializer(GARealGenome::UniformInitializer);
    _genome->mutator(GARealGaussianMutator);
    _genome->crossover(GARealUniformCrossover);
    _genome->userData(this);
    _ga= new GASteadyStateGA(*_genome);
    GASigmaTruncationScaling scaling;
    _ga->minimize();
    GAPopulation popu = _ga->population();
    popu.userData(this);
    popu.evaluator(MPIEvaluator);
    _ga->population(popu);
    _ga->populationSize(_popsize);
    _ga->nGenerations(_generations);
    _ga->pMutation(_pmut);
    _ga->pCrossover(_pcross);
    _ga->scaling(scaling);
    _ga->scoreFrequency(0);  // was 1e-22 implicitly converted to integer 0
    _ga->selectScores(GAStatistics::AllScores);
    _ga->flushFrequency(0);  // was 1e-22 implicitly converted to integer 0

    //must be done before initiliazing the GA
    MasterSlaveCommunicator * comm = find<MasterSlaveCommunicator>();
    FitScheme * fitsch = find<FitScheme>();
    comm->setLocalSlaveCount(fitsch->parallelSimulationCount());
    comm->registerTask(this, &Optimization::chi2);
    if(comm->isMaster())
    {
        QString filepath =path->outputPath()+path->outputPrefix()+"_allsimulations.dat";
        _stream.open(filepath.toLocal8Bit().constData());
        filepath =path->outputPath()+path->outputPrefix()+"_BESTsimulations.dat";
        _beststream.open(filepath.toLocal8Bit().constData());

    }
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPopsize(int value)
{
    _popsize = value;
}

//////////////////////////////////////////////////////////////////////

int Optimization::popsize() const
{
    return _popsize;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setGenerations(int value)
{
    _generations = value;
}

//////////////////////////////////////////////////////////////////////

int Optimization::generations() const
{
    return _generations;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPmut(double value)
{
    _pmut = value;
}

//////////////////////////////////////////////////////////////////////

double Optimization::pmut() const
{
    return _pmut;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPcross(double value)
{
    _pcross = value;
}

//////////////////////////////////////////////////////////////////////

double Optimization::pcross() const
{
    return _pcross;
}

//////////////////////////////////////////////////////////////////////

bool Optimization::done()
{
   return _ga->done();
}

//////////////////////////////////////////////////////////////////////

void Optimization::initialize()
{
   if(find<OligoFitScheme>()->fixedSeed())
        return _ga->initialize(4357);
   else
       return _ga->initialize();

}

//////////////////////////////////////////////////////////////////////

QVariant Optimization::chi2(QVariant input)
{
    QList<QVariant> totalVarList = input.toList();
    int index = totalVarList[0].toInt();
    QList<QVariant> valuesVarList = totalVarList[1].toList();

    ParameterRanges* ranges = find<ParameterRanges>();
    int counter=0;
    QVector<double> currentValues;
    AdjustableSkirtSimulation::ReplacementDict * replacementsGenome =
            new AdjustableSkirtSimulation::ReplacementDict();
    foreach (ParameterRange* range, ranges->ranges())
    {
        double value = valuesVarList[counter].toDouble();
        (*replacementsGenome)[range->label()] = qMakePair(value, range->quantityString());
        counter++;
    }
    OligoFitScheme* oligofit = find<OligoFitScheme>();
    QList<double> Chis;
    QList<QList<double>> luminosities;
    double chi_sum = oligofit->objective((*replacementsGenome),&luminosities,&Chis, index);
    QList<QVariant> output, lumis, chivalues;
    for(int i=0; i<luminosities.size(); i++)
    {
        for(int j=0; j<(luminosities[i]).size(); j++)
        {
            lumis.append((luminosities[i])[j]);
        }
        chivalues.append(Chis[i]);
    }
    output.append(chi_sum);
    output.insert(output.size(),lumis);
    output.insert(output.size(),chivalues);

    return output;

}

//////////////////////////////////////////////////////////////////////

void Optimization::splitChi()
{
    QVector<QVariant> data(_genValues.size());

    for(int i =0;i<_genValues.size();i++)
    {
        QList<QVariant> valuesVarList;
        for(int j = 0; j<_genValues[0].size(); j++)
        {
            valuesVarList.append((double)(_genValues[i])[j]);
        }
        QList<QVariant> totalVarList;
        totalVarList.append(i);
        totalVarList.insert(totalVarList.size(),valuesVarList);
        data[i]=totalVarList;
    }
    MasterSlaveCommunicator* comm = find<MasterSlaveCommunicator>();
    data = comm->performTask(data);

    for(int i =0;i<_genValues.size();i++)
    {
        QList<QVariant> output = data[i].toList();
        double chi_sum = output[0].toDouble();
        QList<QVariant> lumis = output[1].toList();
        QList<QVariant> chivalues = output[2].toList();
        QList<double> Chis;
        QList<double> All_luminosities;

        for(int j = 0; j<lumis.size(); j++)
        {
            All_luminosities.append(lumis[j].toDouble());
        }
        for(int j = 0; j<chivalues.size(); j++)
        {
            Chis.append(chivalues[j].toDouble());
        }

        _genScores[i]=chi_sum;
        _genLum[i]=All_luminosities;
        _genChis[i]=Chis;

    }

}

//////////////////////////////////////////////////////////////////////

void Optimization::step()
{
    _ga->step();
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeList(std::ofstream *stream, QList<double> list)
{
    for (int i=0; i<list.size(); i++)
        (*stream)<<list[i]<<" ";
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeBest(int index, int consec)
{
    _beststream<<consec<<" ";
    writeLine(&_beststream, index);
    ReferenceImages* refs = find<ReferenceImages>();
    refs->writeOutBest(index, consec);
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeLine(std::ofstream *stream, int i)
{
    for(int j=0;j<_genUnitsValues[0].size();j++)
        (*stream)<<(_genUnitsValues[i])[j]<<" ";
    (*stream)<<_genScores[i]<<" ";
    writeList(stream, _genLum[i]);
    writeList(stream, _genChis[i]);
    (*stream)<<endl;
}

//////////////////////////////////////////////////////////////////////

void Optimization::PopEvaluate(GAPopulation & p)
{
    find<Log>()->info("Evaluating generation "+QString::number(_ga->statistics().generation()));

    //creating a temporary folder to store the simulations
    QString folderpath = find<FilePaths>()->output("tmp");
    if(!QDir(folderpath).exists())
        QDir().mkdir(folderpath);

    //loop over all individuals and make replacement for all unevaluated individuals
    for (int i=0; i<p.size(); i++)
    {
        if (p.individual(i).isEvaluated()==gaFalse)
        {
            GARealGenome & genome = (GARealGenome &)p.individual(i);
            ParameterRanges* ranges = find<ParameterRanges>();

            //loop over all ranges to use the correct label but use the genome values to create the replacement
            int counter=0;
            QVector<double> currentUnitsValues, currentValues;
            foreach (ParameterRange* range, ranges->ranges())
            {
                double value = genome.gene(counter);
                currentValues.push_back(value);
                if (range->quantityString()!="")
                    value = find<Units>()->out(range->quantityString(),value);
                currentUnitsValues.push_back(value);
                counter++;
            }
            _genIndices.append(i);
            _genValues.append(currentValues);
            _genUnitsValues.append(currentUnitsValues);
        }
    }
    _genScores.resize(_genIndices.size());
    _genLum.resize(_genIndices.size());
    _genChis.resize(_genIndices.size());

    //Calculate the objective function values in parallel
    splitChi();

    //set the individuals scores and write out all and the best solutions
    find<Log>()->info("Setting Scores");
    for (int i=0; i<_genIndices.size(); i++)
    {
        p.individual(_genIndices[i]).score(_genScores[i]);
        _stream<<p.geneticAlgorithm()->generation()<<" ";
        writeLine(&_stream, i);
        if (_genScores[i]<_bestChi2)
        {
            _bestChi2=_genScores[i];
            writeBest(i,_consec);
            _consec++;
        }
    }
    clearGen(folderpath);
}

//////////////////////////////////////////////////////////////////////

void Optimization::clearGen(const QString & dirName)
{
    _genReplacement.clear();
    _genIndices.clear();
    _genScores.clear();
    _genValues.clear();
    _genLum.clear();
    _genChis.clear();
    _genUnitsValues.clear();

    QDir dir(dirName);
    find<Log>()->info("Removing " + dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  |
                                                    QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (!info.isDir())
                QFile::remove(info.absoluteFilePath());
        }
    }
    dir.rmdir(dirName);
}

//////////////////////////////////////////////////////////////////////
