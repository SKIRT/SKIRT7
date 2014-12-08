/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FOAM_HPP
#define FOAM_HPP

class FoamCell;
class FoamDensity;
class FoamHistogram;
class FoamVector;
class Log;
class Random;

///////////////////////////////////////////////////////////////////////////////

/** The Foam class allows generating random numbers with a non-analytical density distribution,
    which is supplied in the form of a call-back function defined in the FoamDensity interface. To
    accomplish its task, the Foam class creates a \em foam, i.e. a collection of cells with the
    appropriate properties for the specific density distribution under consideration.

    Use the static function createFoam() in this class to create and initialize a new Foam object.

    The Foam class uses the auxiliary classes FoamCell, FoamHistogram, FoamMatrix, FoamPartition and
    FoamVector. The implementation of the foam classes has been copied from an external library and
    adjusted to fit the SKIRT mold. There is no documentation other than the sparse comments in the
    source code. */
class Foam
{
public:
    /** This static function grows foam for a given density, and returns a newly created Foam
        object. Ownership for the object is handed over to the caller, who is responsible for
        deleting it. The arguments are: the simulation's log object; the simulation's random
        generator; the object that implements the FoamDensity interface for the density
        distribution to be foamed; the spatial dimension of the density distribution (2 or 3) and
        the maximum number of cells in the foam. */
    static Foam* createFoam(Log* log, Random* random, FoamDensity* foamdensity, int dimension, int numcells);

private:
    int m_nDim;                 // Dimension of the simplical subspace
    int m_kDim;                 // Dimension of the hyper-cubical subspace
    int m_TotDim;               // Total dimension = m_nDim+m_kDim
    int m_nCells;               // Maximum number of cells (from input)
    int m_vMax;                 // Maximum number of vertices (calculated)
    int m_LastVe;               // Index of the last vertex
    int m_RNmax;                // Maximum r.n. requested at once
    int m_OptDrive;             // Type of Drive =0,1,2 for TrueVol,Sigma,WtMax
    int m_OptEdge;              // Decides whether vertices are included in the sampling
    int m_OptPeek;              // Type of Peek =0,1,2 for maximum, random, random (new)
    int m_OptOrd;               // Single root simplex for OptOrd=1, otherwise root h-cubic
    int m_OptMCell;             // MegaCell option, slim memory for h-cubes
    int m_OptCu1st;             // m_OptCubFirst=1,0, Numbering of dims starts with h-cubic DIPSWITCH
    int m_OptVert;              // m_OptVert=0,1 (Vertices of simplex are stored=0 or not=1)
    int m_OptRej;               // OptRej=0,1, rejection in MC off, on (wt=1 events).
    int m_nBin;                 // Binning of edge-histogram in cell exploration
    int m_nProj;                // Number of projection edges
    int m_nSampl;               // No of sampling when dividing (exploring) cell
    int m_EvPerBin;             // Maximum number of EFFECTIVE event per bin
    int m_N0Si;                 // Start of numbering dimensions  for simplex hyperspace (see m_OptCu1st)
    int m_N0Cu;                 // Start of numbering dimensions  for hypercubic  hyperspace
    int m_P0Si;                 // Start of numbering projections for simplex hyperspace
    int m_P0Cu;                 // Start of numbering projections for hypercubic  hyperspace
    int* m_MaskDiv;             // ! [m_nProj] Dynamic Mask for  cell division, h-cubic + simplical
    int* m_InhiDiv;             // ! [m_kDim]  Flags alowing to inhibit cell division, h-cubic projection/edge
    int m_OptPRD;               // General Option switch for PRedefined Division, for quick check
    FoamVector** m_XdivPRD;     // ! Lists of division values encoded in one vector per direction
    int m_NoAct;                // Number of active cells
    int m_LastCe;               // Index of the last cell
    FoamCell** m_Cells;         // [m_nCells] array of ALL cells
    FoamVector** m_VerX;        // [m_vMax] array of pointers to vertex vectors
    double m_MaxWtRej;          // Maximum weight in rejection for getting wt=1 events
    FoamCell** m_CellsAct;      // ! Array of pointers to active cells, constructed at the end of build-up
    double* m_PrimAcu;          // ! Array of Cumulative Primary of active cells (for fast MC generation!)
    FoamHistogram** m_HistEdg;  // Array of pointers to histograms
    FoamHistogram* m_HistWt;    // Histograms of MC wt
    double* m_MCvect;           // [m_TotDim] Generated MC vector for the outside user
    double  m_MCwt;             // MC weight
    double* m_Rvec;             // [m_RNmax] random number vector from r.n. generator m_TotDim+1 maximum elements
    FoamDensity* m_Rho;         // Pointer to abstract class providing function to integrate
    Random* m_PseRan;           // Generator of pseudorandom numbers
    long m_nCalls;              // No of function calls
    long m_nEffev;              // Total no of effective events in build-up
    double m_SumWt, m_SumWt2;   // Sum of wt and wt^2
    double m_SumOve;            // Sum of overveighted events
    double m_NevGen;            // No of MC events
    double m_WtMax, m_WtMin;    // Max/Min wt
    double m_Prime;             // Primary integral Iprim (Itrue=Iprim<wt>)
    double m_MCresult;          // True Integral Itrue from MC series
    double m_MCerror;           // and its error
    double* m_Lambda;           // [m_nDim] Internal params of the simplex l_0+l_1+...+l_{nDim-1}<1
    double* m_Alpha;            // [m_kDim] Internal params of the hyp-cubic 0<a_i<1

public:
    Foam();                             // Constructor
    ~Foam();                            // Destructor
    void Initialize();                  // Initialization of the FOAM (grid, cells, etc).
    void InitVertices();                // Initializes first vertices of the basic cube
    void InitCells();                   // Initializes first n-factorial cells inside original cube
    int CellFill(int, FoamCell*, int*, FoamVector*, FoamVector*); // Fill next cell and return its index
    void LinkCells();                   // Lift up cells after re-read from disk
    void Explore(FoamCell*);            // Exploration of new cell, determine <wt>, wtMax etc.
    void Carver(int&, double&, double&); // Determine the best edge, wtmax   reduction
    void Varedu(double[], int&, double&,double&); // Determine the best edge, variace reduction
    void MakeLambda();                  // Provides random point inside simplex
    void MakeAlpha();                   // Provides random point inside hypercubic
    void Grow();                        // Adds new cells to FOAM until buffer is full
    long PeekMax();                     // Choose one active cell, used by Grow and also in MC generation
    FoamCell* PeekRan();                // Choose randomly one active cell, used only by Grow
    void Divide(FoamCell*);             // Divide iCell into two daughters; iCell retained, taged as inactive
    void MakeActiveList();              // Creates table of active cells used by GenerCel2
    void GenerCell(FoamCell*&);         // Choose an active cell with probability ~ Primary integral
    void GenerCel2(FoamCell*&);         // Choose an active cell with probability ~ Primary integral
    void MakeEvent();                   // Make one MC event
    void GetMCvect(double*);            // Provides random MC vector;
    void GetMCwt(double&);              // Provides obtained MC weight
    double GetMCwt();                   // Provides obtained MC weight
    double MCgenerate(double* mMCvect); // All three above functions in one
    void SetRho(FoamDensity* Rho) {m_Rho=Rho;}              // Sets new integrand distr.
    void SetPseRan(Random* PseRan) {m_PseRan=PseRan;}       // Sets new r.n. generator
    void SetnDim(int nDim) {m_nDim=nDim;}                   // Sets dimension of simplical subspace
    void SetkDim(int kDim) {m_kDim=kDim;}                   // Sets dimension of hyper-cubical subspace
    void SetnCells(long nCells) {m_nCells=nCells;}          // Sets maximum number of cells
    void SetnSampl(long nSampl) {m_nSampl=nSampl;}          // Sets no of MC events in cell exploration
    void SetnBin(int nBin) {m_nBin=nBin;}                   // Sets no of bins in histogs in cell exploration
    void SetOptRej(int OptRej) {m_OptRej=OptRej;}           // Sets option for MC rejection
    void SetOptDrive(int OptDrive) {m_OptDrive=OptDrive;}   // Sets option Drive, type of driver integrand
    void SetOptPeek(int OptPeek) {m_OptPeek=OptPeek;}       // Sets option Peek, type of choice of cell in build-up
    void SetOptEdge(int OptEdge) {m_OptEdge=OptEdge;}       // Sets option Edge, include or not vertices
    void SetOptOrd(int OptOrd) {m_OptOrd=OptOrd;}           // Sets option Ord, single root simplex or h-cub.
    void SetOptMCell(int OptMCell) {m_OptMCell=OptMCell;}   // Sets MegaCell option, slim memory for h-cubes
    void SetOptVert(int OptVert) {m_OptVert=OptVert;}       // Sets OptVert option
    void SetEvPerBin(int EvPerBin) {m_EvPerBin=EvPerBin;}   // Sets max. no. of eff. events per bin in exploration
    void SetMaxWtRej(double MaxWtRej) {m_MaxWtRej=MaxWtRej;} // Sets max. weight for rejection (wt=1 evts)
    int GetTotDim() {return m_TotDim;}                      // Get total dimension
    double GetPrimary() {return m_Prime;}                   // Get value of primary integral
    void GetPrimary(double &Prime) {Prime=m_Prime;}         // Get value of primary integral
    long GetnCalls() {return m_nCalls;}                     // Get total no of function calls
    long GetnEffev() {return m_nEffev;}                     // Get total no of effective wt=1 events
    void CheckAll();                                        // Checks corectness of FOAM structure
private:
    void RandomArray(int size, double* vect);         // Gets an array of uniform random numbers
    double sqr(double x) { return x*x; }
    double dmax(double x, double y) { if(x>y) return x; else return y; }
    double dmin(double x, double y) { if(x>y) return y; else return x; }
    long silnia(int n) { long s=n; for(int i=n-1; i>1; i--) s*=i; if(n==0) s=1; return s; }  // Factorial
private:
    Foam(const Foam&);
    Foam& operator=(const Foam&);
};

/////////////////////////////////////////////////////////////////////////////

#endif // FOAM_HPP
