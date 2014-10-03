/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "GALumfit.hpp"
#include "FatalError.hpp"
#include <math.h>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////

float Objective(GAGenome & g, QList<Array> *sim)
{

    Array ref = sim->last();
    sim->pop_back();
    GARealGenome& genome = (GARealGenome&)g;

    if (sim->size() != genome.size()) throw FATALERROR("Number of luminosities and components do not match");

    QList<double> lumis;
    for(int i=0; i<genome.length(); i++){
      lumis.append(genome.gene(i));
    }

    int arraysize = (*sim)[0].size();
    double chi=0;

    for (int m=0; m<arraysize; m++)
    {
        for(int n=0; n<sim->size(); n++)
        {
            double total_sim = lumis[n] * (((*sim)[n])[m]);
            double sigma = sqrt( abs(ref[m]) + total_sim);
            if (ref[m]==0)
            {
                ((*sim)[n])[m] = 0;
                total_sim = 0;
                sigma = 0;
            }
            else
            {
                chi += pow( (ref[m] - total_sim) / sigma,2);
            }
        }
    }
    sim->append(ref);
    return chi;
}
//////////////////////////////////////////////////////////////////////

void Evaluator(GAPopulation & p)
{
    QList<Array> *sim = (QList<Array> *)p.userData();

    //loop over all individuals and make replacement for all unevaluated individuals
    for (int i=0; i<p.size(); i++)
    {
        if (p.individual(i).isEvaluated()==gaFalse)
        {
            double value =  Objective(p.individual(i),sim);
            p.individual(i).score(value);
        }
    }

}

////////////////////////////////////////////////////////////////////

GALumfit::GALumfit()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GALumfit::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GALumfit::setMinLuminosities(QList<double> value)
{
    _minLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> GALumfit::minLuminosities() const
{
    return _minLum;
}

//////////////////////////////////////////////////////////////////////

void GALumfit::setMaxLuminosities(QList<double> value)
{
    _maxLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> GALumfit::maxLuminosities() const
{
    return _maxLum;
}

//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////

void GALumfit::optimize(const Array *Rframe, QList<Array> *frames, QList<double> *lumis, double &chi2)
{

    _ref = Rframe;
    QList<Array> *sim = frames;
    frames = sim;

    GARealAlleleSetArray allelesetarray;
    for(int i=0;i<_minLum.size();i++)
    {
        GARealAlleleSet RealAllele(_minLum[i],_maxLum[i]);
        allelesetarray.add(RealAllele);
    }

    GARealGenome* genome = new GARealGenome(allelesetarray);
    genome->initializer(GARealGenome::UniformInitializer);
    genome->mutator(GARealGaussianMutator);
    genome->crossover(GARealUniformCrossover);

    GASteadyStateGA* _ga= new GASteadyStateGA(*genome);
    GASigmaTruncationScaling scaling;
    _ga->minimize();
    GAPopulation popu = _ga->population();
    sim->append(*Rframe);
    popu.userData(sim);
    popu.evaluator(Evaluator);

    _ga->population(popu);
    _ga->populationSize(frames->size()*20);
    _ga->nGenerations(frames->size()*20);
    _ga->pMutation(0.03);
    _ga->pCrossover(0.65);
    _ga->scaling(scaling);
    _ga->scoreFrequency(0);  // was 1e-22 implicitly converted to integer 0
    _ga->selectScores(GAStatistics::AllScores);
    _ga->flushFrequency(0);  // was 1e-22 implicitly converted to integer 0
    _ga->initialize();
    while(!_ga->done()) _ga->step();

    GARealGenome& best_genome = (GARealGenome&) _ga->statistics().bestIndividual();

    for(int i=0;i<_minLum.size();i++)
    {
        lumis->append(best_genome.gene(i));
    }
    chi2=best_genome.score();





}

////////////////////////////////////////////////////////////////////
