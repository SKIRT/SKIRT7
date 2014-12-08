/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Foam.hpp"
#include "FoamCell.hpp"
#include "FoamDensity.hpp"
#include "FoamHistogram.hpp"
#include "FoamMatrix.hpp"
#include "FoamPartition.hpp"
#include "FoamVector.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

#define MIN -1.0e150
#define MAX  1.0e150

///////////////////////////////////////////////////////////////////////////////////////

Foam *Foam::createFoam(Log* log, Random* random, FoamDensity* foamdensity, int dimension, int numcells)
{
    log->info("Growing foam of up to " + QString::number(numcells) + " cells...");

    // Make two passes: in case the first pass fails, we try again with a different random sequence
    Foam* foam = 0;
    for (int pass = 0; ; pass++)
    {
        try
        {
            foam = new Foam();
            foam->SetnDim(0);          // SIMPLICAL subspace
            foam->SetkDim(dimension);  // HYP-CUBICAL subspace
            foam->SetnCells(numcells); // Number of Cells
            foam->SetnSampl(500);      // Number of MC events per cell in build-up
            foam->SetnBin(8);          // Number of bins in build-up
            foam->SetOptRej(1);        // =0, weighted events;  =1, wt=1 events
            foam->SetOptDrive(2);      // (D=2) Option, type of Drive =0,1,2 for TrueVol,Sigma,WtMax
            foam->SetOptEdge(0);       // (D=0) Option, vertices are included in the sampling (0) or not (1)
            foam->SetOptOrd(0);        // (D=0) Option, single simplex or nDim! simlices
            foam->SetOptPeek(0);       // (D=0) Option, choice of cell in build-up maximum(0), random(1)
            foam->SetOptMCell(1);      // (D=1) Option, Mega-Cell = slim memory
            foam->SetOptVert(1);       // (D=1) Vertices are not stored
            foam->SetEvPerBin(25);     // Maximum events (equiv.) per bin in build-up
            foam->SetMaxWtRej(1.1);    // Maximum wt for rejection, for OptRej=1
            foam->SetPseRan(random);   // our simulation's random generator
            foam->SetRho(foamdensity); // foam density provided by caller
            foam->Initialize();
            break;  // if we reach here, all is well and we can quit the loop
        }
        catch (FatalError& error)
        {
            // if this is not the first time around, simple rethrow the error
            if (pass > 0) throw error;

            // if this is the first time, output a warning and try again
            log->warning("The foam code reported the following error:");
            foreach (QString line, error.message()) log->warning(line);
            log->warning("Retrying to grow foam with a different random sequence...");

            // try to recover the allocated memory
            delete foam;
        }
    }
    log->info("Foam has been grown.");

    return foam;
}

///////////////////////////////////////////////////////////////////////////////////////

Foam::Foam()
{
    m_OptCu1st = 1;                // =1 Numbering of dimens starts with h-cubic, DIPSWITCH
    m_MaskDiv  = NULL;             // Dynamic Mask for  cell division, h-cubic + simplical
    m_InhiDiv  = NULL;             // Flag alowing to inhibit cell division in certain projection/edge
    m_XdivPRD  = NULL;             // Lists of division values encoded in one verctor per direction
    m_VerX     = NULL;
    m_Cells    = NULL;
    m_Lambda   = NULL;
    m_Alpha    = NULL;
    m_CellsAct = NULL;
    m_PrimAcu  = NULL;
    m_HistEdg  = NULL;
    m_HistWt   = NULL;
    m_nDim     = 0;                // dimension of simplical sub-space
    m_kDim     = 0;                // dimension of hyp-cubical sub-space
    m_nCells   = 1000;             // Maximum number of Cells,    is usually re-set
    m_vMax     = 0;                // Maximum number of vertices, is re-calculated from m_nCells
    m_nSampl   = 200;              // No of sampling when dividing cell
    m_OptPRD   = 0;                // General Option switch for PRedefined Division, for quick check
    m_OptDrive = 2;                // type of Drive =1,2 for TrueVol,Sigma,WtMax
    m_OptEdge  = 0;                // decides whether vertices are included in the sampling
    m_OptPeek  = 0;                // type of Peek =0,1 for maximum, random
    m_OptOrd   = 0;                // root cell h-cub. or simplex, =0,1
    m_OptMCell = 1;                // MegaCell option, slim memory for h-cubes
    m_OptVert  = 1;                // Vertices of simplex are not stored
    m_OptRej   = 0;                // OptRej=0, wted events; OptRej=0, wt-1 events
    //------------------------------------------------------
    m_nBin     = 8;                // binning of edge-histogram in cell exploration
    m_EvPerBin =25;                // maximum no. of EFFECTIVE event per bin, =0 option is inactive
    //------------------------------------------------------
    m_nCalls = 0;                  // No of function calls
    m_nEffev = 0;                  // Total no of eff. wt=1 events in build=up
    m_LastCe =-1;                  // Index of the last cell
    m_NoAct  = 0;                  // No of active cells (used in MC generation)
    m_LastVe =-1;                  // Index of the last vertex
    m_WtMin = MAX;                 // Minimal weight
    m_WtMax = MIN;                 // Maximal weight
    m_MaxWtRej =1.10;              // Maximum weight in rejection for getting wt=1 events
    m_PseRan = NULL;
    m_Rho = NULL;
}

///////////////////////////////////////////////////////////////////////////////

Foam::~Foam()
{
    if(m_Cells!= NULL)
    {
        for(int i=0; i<m_nCells; i++) delete m_Cells[i];
        delete[] m_Cells;
    }
    delete[] m_Rvec;
    delete[] m_Alpha;
    delete[] m_Lambda;
    delete[] m_MCvect;
    delete[] m_PrimAcu;
    delete[] m_MaskDiv;
    delete[] m_InhiDiv;
    if( m_VerX!= NULL)
    {
        for(int i=0; i<m_vMax; i++) delete m_VerX[i];
        delete[] m_VerX;
    }
    if( m_XdivPRD!= NULL)
    {
        for(int i=0; i<m_kDim; i++) delete m_XdivPRD[i];
        delete[] m_XdivPRD;
    }
    for(int i=0;i<m_nProj;i++) delete m_HistEdg[i];
    delete[] m_HistEdg;
    delete m_HistWt;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Initialize()
{
    m_TotDim=m_nDim+m_kDim;
    if(m_TotDim==0) throw FATALERROR("Zero dimension is not alowed");

    /////////////////////////////////////////////////////////////////////////
    //                   ALLOCATE SMALL LISTS                              //
    //   it is done globaly, not for each cell, to save on allocation time //
    /////////////////////////////////////////////////////////////////////////
    m_RNmax= m_TotDim+1;
    m_Rvec = new double[m_RNmax];   // Vector of random numbers

    if(m_nDim>0)
    {
        m_Lambda = new double[m_nDim];   // sum<1 for internal parametrization of the simplex
    }
    if(m_kDim>0)
    {
        m_Alpha = new double[m_kDim];    // sum<1 for internal parametrization of the simplex
    }
    m_MCvect = new double[m_TotDim]; // vector generated in the MC run

    //====== Variables related to MC cell exploration (projections edges) =====
    if(m_OptCu1st)
    {
        m_N0Cu=0;   m_N0Si=m_kDim;              // hcubic dimensions are first
        m_P0Cu=0;   m_P0Si=m_kDim;
    }
    else
    {
        m_N0Cu=m_nDim;                m_N0Si=0; // simplical dimensions are first
        m_P0Cu=m_nDim*(m_nDim+1)/2;   m_P0Si=0;
    }
    m_nProj = m_nDim*(m_nDim+1)/2 +m_kDim;        // number of the PROJECTION edges

    //====== List of directions inhibited for division
    if(m_InhiDiv == NULL)
    {
        m_InhiDiv = new int[m_kDim];
        for(int i=0; i<m_kDim; i++) m_InhiDiv[i]=0;    // Hcubic space only!!!
    }

    //====== Dynamic mask used in Explore for edge determibation
    if(m_MaskDiv == NULL)
    {
        m_MaskDiv = new int[m_nProj];
        for(int i=0; i<m_nProj; i++) m_MaskDiv[i]=1;    // Hcub+Simplical
    }

    //====== List of predefined division values in all directions (initialized as empty)
    if(m_XdivPRD == NULL)
    {
        m_XdivPRD = new FoamVector*[m_kDim];
        for(int i=0; i<m_kDim; i++)  m_XdivPRD[i]=NULL; // Artificialy extended beyond m_kDim
    }

    //====== Initialize list of histograms
    m_HistWt  = new FoamHistogram(0.0, 1.5*m_MaxWtRej, 100);
    m_HistEdg = new FoamHistogram*[m_nProj];   // Initialize list of histograms
    for(int i=0;i<m_nProj;i++) m_HistEdg[i]= new FoamHistogram(0.0, 1.0, m_nBin); // Initialize histogram for each edge

    // ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| //
    //                     BUILD-UP of the FOAM                            //
    // ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| //
    //        Allocate and Initialize BIG list of vertices
    if(m_nDim>0) InitVertices();
    //        Allocate BIG list of cells, within cMax limit
    //        Define and explore root cell(s)
    InitCells();
    Grow();
    MakeActiveList(); // Final Preperations for the M.C. generation

    // Preperations for the M.C. generation
    m_SumWt  = 0.0;               // M.C. generation sum of Wt
    m_SumWt2 = 0.0;               // M.C. generation sum of Wt**2
    m_SumOve = 0.0;               // M.C. generation sum of overweighed
    m_NevGen = 0.0;               // M.C. generation sum of 1d0
    m_WtMax  = MIN;               // M.C. generation maximum wt
    m_WtMin  = MAX;               // M.C. generation minimum wt
    m_MCresult= m_Cells[0]->GetIntg(); // M.C. Value of INTEGRAL,temporary asignment
    m_MCresult= m_Cells[0]->GetIntg(); // M.C. Value of INTEGRAL,temporary asignment
    m_MCerror = m_Cells[0]->GetIntg(); // M.C. Value of ERROR   ,temporary asignment
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::InitVertices()
{
    // Alocate and Define components of vertices in the simplical subspace
    // OptOrd=0: 2^nDim  initial vertices of the root unit hypercube
    //           like (0000),(0001),(0010),(0011),(0100)...(1111)
    // OptOrd=1:  nDim+1 initial  vertices of the root unit simplex
    //           like (0000),(1001),(1100),(1110),(1111)
    //
    long i,j, NoVert=0, nDivi, iVe;
    int k;
    FoamPartition partition(m_nDim);

    if( m_VerX != NULL ) delete[] m_VerX;
    if(m_nDim==0) throw FATALERROR("m_nDim must not be zero");

    switch(m_OptOrd)
    {
    case 0:
        // cMax = 1+ nDim! +2*nDivi, vMax = 2^nDim + nDivi, where  nDivi
        // is the no. of binary divisions after split of the unit h-cube
        nDivi= (m_nCells -1 - silnia(m_nDim))/2;
        if(nDivi < 1 ) throw FATALERROR("Too big m_nDim or too small m_nCells");
        m_vMax= (long)(pow(2.0, m_nDim))+nDivi;
        if(m_OptVert==1)  m_vMax= (long)(pow(2.0, m_nDim));

        // Allocate BIG list of vertices
        m_VerX = new FoamVector*[m_vMax];
        for(i=0;i<m_vMax;i++)
            m_VerX[i]= new  FoamVector(m_nDim);

        // Define components of the first 2^nDim vertices
        NoVert = (long)pow(2.0,m_nDim);
        iVe=0;
        partition.Reset();
        do {
            for(k=0; k<m_nDim; k++) (*m_VerX[iVe])[k] = (partition.Digit(k));
            iVe++;
        } while ( partition.Next() != 0);
        if(iVe != NoVert ) throw FATALERROR("Something wrong with sum over partitions");
        break;

    case 1:
        // cMax = 1+ 2*nDivi,  vMax = (nDim+1) + nDivi
        nDivi= (m_nCells -1)/2;
        m_vMax= (m_nDim+1)+nDivi;
        if(m_OptVert==1)  m_vMax=  m_nDim+1;

        // Allocate BIG list of vertices
        m_VerX = new FoamVector*[m_vMax];
        for(i=0;i<m_vMax;i++)
            m_VerX[i]= new  FoamVector(m_nDim);

        // Define components of the first nDim+1 vertices
        NoVert = m_nDim+1;
        for(i=0; i<NoVert; i++)
        {
            for(j=0; j<m_nDim; j++)
            {
                if(j>=i)
                    (*m_VerX[i])[j] = 0;
                else
                    (*m_VerX[i])[j] = 1;
            }
        }
        break;
    default:
        throw FATALERROR("Incorrect value for m_OptOrd");
    }
    m_LastVe = NoVert-1;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::InitCells()
{
    // Initialize "root part" of the FOAM of cells
    int i,j,k,Mask;
    long iVe,jser,iter,npow;
    int  *Perm,*digit;
    int    *Vertices =NULL;
    FoamVector *Posi =NULL;
    FoamVector *Size =NULL;
    m_LastCe =-1;                             // Index of the last cell
    if(m_Cells!= NULL)
    {
        for(i=0; i<m_nCells; i++) delete m_Cells[i];
        delete [] m_Cells;
    }
    m_Cells = new FoamCell*[m_nCells];
    for(i=0;i<m_nCells;i++)
    {
        m_Cells[i]= new FoamCell(m_nDim,m_kDim,m_OptMCell,m_OptCu1st); // Allocate BIG list of cells
        m_Cells[i]->SetCell0(m_Cells);
        m_Cells[i]->SetVert0(m_VerX);
        m_Cells[i]->SetSerial(i);
    }

    // define initial values of Posi Size vectors, if h-cubic is present

    if(m_kDim>0)
    {
        Posi = new FoamVector(m_kDim);
        Size = new FoamVector(m_kDim);
        (*Posi)=0.0;
        (*Size)=1.0;
    }

    if(m_nDim==0 && m_kDim>0)
    {
        CellFill(1, NULL, Vertices, Posi, Size);  //  0-th cell ACTIVE
    }
    else if(m_nDim>0)
    {
        Vertices = new int[m_nDim+1];       // ALLOCATE array of pointers to vertices
        switch(m_OptOrd)
        {
        case 0:
            // Status, Parent, Verts, Posi, Size
            CellFill(0, NULL, NULL, NULL, NULL); // 0-th cell INACTIVE

            // Split unit cube into nDim! SIMPLICES
            // Method of constructing permutation is primitive/simple
            // It is good enough for our n<10 problem

            // LOOP OVER Permutations of (0,1,2,3,...,m_Ndim-1) starts here
            npow = (long)pow((double)(m_nDim),m_nDim);
            Perm  = new int[m_nDim];
            digit = new int[m_nDim];
            for(i=0; i<m_nDim; i++) Perm[i]=0; // start
            for(iter=0; iter<npow; iter++)
            {
                Mask=1;
                for(i=0; i<m_nDim; i++)
                    for(j=i+1; j<m_nDim; j++)
                        if(Perm[i]==Perm[j]) Mask=0;
                if(Mask==1) //         NEW PERMUTATION is found, define NEW simplical CELL
                {
                    for(iVe=0;iVe<m_nDim+1;iVe++)
                    {
                        // This digit represents single BASIC SIMPLEX, other obtained by Permuting dimensions
                        for(k=0;k<m_nDim;k++)
                        {
                            digit[k]=0;
                            if(k<iVe) digit[k]=1;
                        }
                        // Translation from "binary" digit to serial pointer of a given vertex
                        jser=0;
                        for( k=0;k<m_nDim;k++)
                            jser= 2*jser+  digit[Perm[k]];
                        Vertices[iVe] = jser;      // FILL in pointers to vertices
                    }
                    // Status, Parent, Verts, Posi, Size
                    CellFill(1, m_Cells[0], Vertices, Posi, Size);
                }
                Perm[m_nDim-1]=Perm[m_nDim-1]+1; // increment
                for(k=m_nDim-1; k>0; k--)
                    if(Perm[k]==m_nDim){ Perm[k]=0; Perm[k-1]=Perm[k-1]+1;}
            }
            delete[] Perm;
            delete[] digit;
            break;

        case 1:
            // ROOT 0-th Cell is just SINGLE SIMPLEX, it is ACTIVE here of course!   //
            for( k=0;k<m_nDim+1;k++)
                Vertices[k] = k;          // FILL in pointers to vertices
            // Status, Parent, Verts, Posi, Size
            CellFill(1, NULL, Vertices, Posi, Size);  //  0-th cell ACTIVE
            break;

        default:
            throw FATALERROR("Incorrect value for m_OptOrd");
        }
    }

    // Exploration of the root cell(s)

    int iStart=0;                           // normal case
    if( m_OptOrd==0 && m_nDim>0 ) iStart=1; // the case of nDim! root cells
    for(long iCell=iStart; iCell<=m_LastCe; iCell++)
    {
        Explore( m_Cells[iCell] );               // Exploration of root cell(s)
    }

    // Cleanup

    if(Vertices!=NULL) delete[] Vertices;
    if(Posi!=NULL) delete Posi;
    if(Size!=NULL) delete Size;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::LinkCells()
{
    // lift up certain paremeters of cells after re-read from disk
    for(int i=0;i<m_nCells;i++)
    {
        m_Cells[i]->SetCell0(m_Cells);
        m_Cells[i]->SetVert0(m_VerX);
    }
    MakeActiveList(); // Final Preperations for the M.C. generation
}

///////////////////////////////////////////////////////////////////////////////

int
Foam::CellFill(int Status, FoamCell* Parent,
               int* Vertices, FoamVector* Posi,
               FoamVector* Size)
{
    // Fills in cell all content of the new active cell
    FoamCell* Cell;
    if (m_LastCe==m_nCells) throw FATALERROR("Too many cells");
    m_LastCe++;   // 0-th cell is the first
    if (Status==1) m_NoAct++;

    Cell = m_Cells[m_LastCe];

    int kParent = -1;
    if(Parent!=NULL) kParent =Parent->GetSerial();

    Cell->Fill(Status, kParent, -1, -1, Vertices, Posi, Size);

    Cell->SetBest( -1);         // pointer for planning division of the cell
    Cell->SetXdiv(0.5);         // factor for division
    double xInt2,xDri2;
    if(Parent!=NULL)
    {
        xInt2  = 0.5*Parent->GetIntg();
        xDri2  = 0.5*Parent->GetDriv();
        Cell->SetIntg(xInt2);
        Cell->SetDriv(xDri2);
    }
    else
    {
        Cell->SetIntg(0.0);
        Cell->SetDriv(0.0);
    }
    return m_LastCe;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Explore(FoamCell *Cell)
{
    //   Explore newly defined cell with help of special short MC sampling
    //   As a result, estimates of true and drive volume will be defined
    //   Average and dispersion of the weight distribution will be found along
    //   each edge and the best edge (minimum dispersion) is memorized for future use.
    //   Axerage x for eventual future cell division is also defined.
    //   Recorded are aso minimum and maximu weight etc.
    //   The volume estimate in all (inactive) parent cells is updated.
    //   Note that links to parents and initial volume = 1/2 parent has to be
    //   already defined prior to calling this function.
    double Factorial;
    double Wt, Dx, Dxx, Vsum, xBest, yBest;
    double IntOld, DriOld;
    long iev;
    double NevMC;
    int iv, jv, i, j, k;
    int nProj, kBest;
    double CeSum[5], Xproj;
    FoamVector Size(m_kDim);
    FoamVector Posi(m_kDim);
    FoamVector VRand(m_nDim), Lambda(m_nDim+1), X(m_nDim);
    Cell->GetHcub(Posi,Size);
    FoamCell* Parent;
    FoamMatrix Yrel(m_nDim), Xrel(m_nDim), XVert(m_nDim+1); // m_nDim=0 is handled internaly
    double* xRand = new double[m_TotDim];
    double* VolPart = NULL;
    if(m_nDim>0)
    {
        VolPart= new double[m_nDim+1];   // partial volumes for handling simplex geometry
        // Table of vertex position
        for(iv=0; iv<m_nDim+1; iv++)
        {
            if(m_OptVert==0)
            {
                for(j=0; j< m_nDim; j++)
                    XVert(iv,j)=(*(*Cell)[iv])[j];
            }
            else
            {
                X=0.;
                Lambda=0.;
                Cell->GetXSimp(X,Lambda,iv);
                for(j=0; j< m_nDim; j++)
                    XVert(iv,j)=X[j];
            }
        }
    }
    Cell->CalcVolume();
    Dx = Cell->GetVolume();    // Dx includes simplical and h-cubical parts
    Factorial=silnia(m_nDim);  // m_nDim=0 is handled internaly
    IntOld = Cell->GetIntg();  // memorize old values,
    DriOld = Cell->GetDriv();  // will be needed for correcting parent cells
    if(m_OptVert==0)
    {
        if(m_nDim>0)
        {     // decode vertex vectors, relative last vertex
            for(iv=0; iv< m_nDim; iv++)
                for(j=0; j< m_nDim; j++)
                    Xrel(iv,j) =  (*(*Cell)[iv])[j] - (*(*Cell)[m_nDim])[j];
        }
    }

    // Special Short MC sampling to probe cell
    CeSum[0]=0;
    CeSum[1]=0;
    CeSum[2]=0;
    CeSum[3]=MAX;  //wtmin
    CeSum[4]=MIN;  //wtmax
    if(m_nProj>0) for(i=0;i<m_nProj;i++) m_HistEdg[i]->Reset();              // Reset histograms
    m_HistWt->Reset();

    // Additional scan over vertices in order to improve max/min weights
    int icont =0;
    FoamPartition BiPart(m_kDim);
    if(m_OptEdge==1)
    {
        for(iv=0; iv<m_nDim+1; iv++)
        {      // loop over vertices in the simplex
            BiPart.Reset();
            do {                             // loop over vertices in hyp-cube
                for(j=0; j<m_nDim; j++)
                    xRand[m_N0Si+j]  = XVert(iv,j);
                for(k=0; k<m_kDim; k++)
                    xRand[m_N0Cu+k] =  Posi[k] +(BiPart.Digit(k))*(Size[k]);
                Wt = m_Rho->foamdensity(m_TotDim, xRand )*Dx; // <wt> normalised to integral over the cell
                m_nCalls++;
                icont++; if(icont>100) break; // protection against excesive scann
                if (CeSum[3]>Wt) CeSum[3]=Wt;
                if (CeSum[4]<Wt) CeSum[4]=Wt;
            } while ( BiPart.Next() != 0);
        }
    }

    // ||||||||||||||||||||||||||BEGIN MC LOOP|||||||||||||||||||||||||||||

    double NevEff = 0.0;
    for(iev=0;iev<m_nSampl;iev++)
    {
        MakeLambda();              // generate uniformly vector inside simplex
        MakeAlpha();               // generate uniformly vector inside hypercube
        if( m_OptVert && m_nDim>0 )
        {
            for(j=0; j<m_nDim; j++) Lambda[j]=m_Lambda[j];
            Lambda[m_nDim]=0.;
            VRand=0.;
            Cell->GetXSimp(VRand,Lambda,m_nDim);
            for(j=0; j<m_nDim; j++) xRand[m_N0Si+j]=VRand[j];
        }
        else
        {
            for(j=0; j<m_nDim; j++)
            {
                xRand[m_N0Si+j]=(*(*Cell)[m_nDim])[j];
                for(iv=0; iv<m_nDim; iv++)
                    xRand[m_N0Si+j] += m_Lambda[iv]*Xrel(iv,j);
            }
        }
        if(m_kDim>0)
        {
            for(j=0; j<m_kDim; j++)
                xRand[m_N0Cu+j]= Posi[j] +m_Alpha[j]*(Size[j]);
        }
        Wt = m_Rho->foamdensity(m_TotDim, xRand )*Dx;   // <wt> normalised to integral over the cell
        // calculate partial volumes, necessary for projecting in simplex edges
        if(m_nDim>0)
        {
            Vsum = 0.0;
            for(jv=0; jv<m_nDim+1; jv++)  // all vertices relative to xRand, jv is omitted
            {
                k=0;
                for(iv=0; iv<m_nDim+1; iv++)
                {
                    if(iv!=jv)  // vertex jv will be replaced with the random vertex
                    {
                        for(j=0; j<m_nDim; j++)
                            Yrel(k,j) = XVert(iv,j) - xRand[m_N0Si+j];
                        k++;
                    }
                }
                Dxx = Yrel.Determinant();
                VolPart[jv] = fabs(Dxx/Factorial);
                Vsum += VolPart[jv];
            }
            // this x-check only makes sense for pure simplical case!
            if( (m_kDim*m_nDim ==0) && (fabs(Vsum-Dx) > 1.0e-7*(fabs(Vsum)+fabs(Dx))) )
                throw FATALERROR("Something wrong with volume calculation");
        }

        // IMPORTANT! Two loops below determine indexing of edges (simplex and hypercube)
        nProj = m_P0Si;
        if(m_nDim>0)
        {
            for(jv=0; jv<m_nDim+1; jv++)
            {
                for(iv=jv+1; iv<m_nDim+1; iv++)
                {
                    Xproj = VolPart[jv]/(VolPart[jv]+VolPart[iv]);
                    m_HistEdg[nProj]->Fill(Xproj,Wt); // fill all histograms, search the best edge-candidate
                    nProj++;
                }
            }
        }
        nProj = m_P0Cu;
        if(m_kDim>0)
        {
            for(k=0; k<m_kDim; k++)
            {
                Xproj =m_Alpha[k];
                m_HistEdg[nProj]->Fill(Xproj,Wt); // fill all histograms, search the best edge-candidate
                nProj++;
            }
        }

        m_nCalls++;
        CeSum[0] += Wt;    // sum of weights
        CeSum[1] += Wt*Wt; // sum of weights squared
        CeSum[2]++;        // sum of 1
        if (CeSum[3]>Wt) CeSum[3]=Wt;  // minimum weight;
        if (CeSum[4]<Wt) CeSum[4]=Wt;  // maximum weight

        // test MC loop exit condition
        NevEff = CeSum[0]*CeSum[0]/CeSum[1];
        if( NevEff >= m_nBin*m_EvPerBin) break;
    }

    // Predefine logics of searching for the best division edge
    for(k=0; k<m_kDim;k++)
    {
        m_MaskDiv[m_P0Cu+k] =1;                        // default is all
        if( m_InhiDiv[k]==1) m_MaskDiv[m_P0Cu +k] = 0; // inhibit some...
    }

    // Note that predefined division below overrule inhibition above
    kBest=-1;
    double rmin,rmax,rdiv;
    if(m_OptPRD)      // quick check
    {
        for(k=0; k<m_kDim; k++)
        {
            rmin= Posi[k];
            rmax= Posi[k] +Size[k];
            if( m_XdivPRD[k] != NULL){
                int N= (m_XdivPRD[k])->GetDim();
                for(j=0; j<N; j++){
                    rdiv=(*m_XdivPRD[k])[j];
                    // check predefined divisions is available in this cell
                    if( (rmin +1e-99 <rdiv) && (rdiv< rmax -1e-99)){
                        kBest=k;
                        xBest= (rdiv-Posi[k])/Size[k] ;
                        goto ee05;
                    }
                }
            }
        }
    }
ee05:

    m_nEffev += (long)NevEff;
    NevMC = CeSum[2];
    double IntTrue = CeSum[0]/(NevMC+0.000001);
    double IntDriv = 0.0, IntPrim = 0.0;
    switch(m_OptDrive)
    {
    case 1:                       // VARIANCE REDUCTION
        if(kBest == -1) Varedu(CeSum,kBest,xBest,yBest); // determine the best edge,
        //IntDriv =sqrt( CeSum[1]/NevMC -IntTrue*IntTrue ); // Older ansatz, numericaly not bad
        IntDriv =sqrt(CeSum[1]/NevMC) -IntTrue; // Foam build-up, sqrt(<w**2>) -<w>
        IntPrim =sqrt(CeSum[1]/NevMC);          // MC gen. sqrt(<w**2>) =sqrt(<w>**2 +sigma**2)
        break;
    case 2:                       // WTMAX  REDUCTION
        if(kBest == -1) Carver(kBest,xBest,yBest);  // determine the best edge
        IntDriv =CeSum[4] -IntTrue; // Foam build-up, wtmax-<w>
        IntPrim =CeSum[4];          // MC generation, wtmax!
        break;
    default:
        throw FATALERROR("Incorrect value for m_OptDrive");
    }

    Cell->SetBest(kBest);
    Cell->SetXdiv(xBest);
    Cell->SetIntg(IntTrue);
    Cell->SetDriv(IntDriv);
    Cell->SetPrim(IntPrim);

    // correct/update integrals in all parent cells to the top of the tree
    double  ParIntg, ParDriv;
    for(Parent = Cell->GetPare(); Parent!=NULL; Parent = Parent->GetPare())
    {
        ParIntg = Parent->GetIntg();
        ParDriv = Parent->GetDriv();
        Parent->SetIntg( ParIntg +IntTrue -IntOld );
        Parent->SetDriv( ParDriv +IntDriv -DriOld );
    }
    delete[] VolPart;
    delete[] xRand;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Varedu(double CeSum[5],
             int& kBest,
             double& xBest,
             double& yBest)
{
    // The case of the optimization of the maximum weight.
    // Determine the best edge-candidate for future cell division,
    // using results of the MC exploration run within the cell stored in m_HistEdg
    double Nent   = CeSum[2];
    double sswAll = CeSum[1];
    double SSw    = sqrt(sswAll)/sqrt(Nent);
    kBest =-1;
    xBest =0.5;
    yBest =1.0;
    double MaxGain=0.0;

    // Now go over all projections kProj
    for(int kProj=0; kProj<m_nProj; kProj++)
        if( m_MaskDiv[kProj])
        {
            // initialize search over bins
            double SSwtBest = MAX;
            double Gain =0.0;
            double xMin=0.0; double xMax=0.0;
            // Double loop over all pairs jLo<jUp
            for(int jLo=1; jLo<=m_nBin; jLo++)
            {
                double swIn=0;  double sswIn=0;
                for(int jUp=jLo; jUp<=m_nBin;jUp++)
                {
                    swIn  +=     (m_HistEdg[kProj])->GetBinContent(jUp);
                    sswIn += sqr((m_HistEdg[kProj])->GetBinError(  jUp));
                    double xLo=(jLo-1.0)/m_nBin;
                    double xUp=(jUp*1.0)/m_nBin;
                    double SSwIn = sqrt(sswIn)       /sqrt(Nent*(xUp-xLo))     *(xUp-xLo);
                    double SSwOut= sqrt(sswAll-sswIn)/sqrt(Nent*(1.0-xUp+xLo)) *(1.0-xUp+xLo);
                    if( (SSwIn+SSwOut) < SSwtBest)
                    {
                        SSwtBest = SSwIn+SSwOut;
                        Gain     = SSw-SSwtBest;
                        xMin    = xLo;
                        xMax    = xUp;
                    }
                }
            }
            int iLo = (int) (m_nBin*xMin);
            int iUp = (int) (m_nBin*xMax);

            if(Gain>MaxGain)
            {
                MaxGain=Gain;
                kBest=kProj; // <--- !!!!! The best edge
                xBest=xMin;
                yBest=xMax;
                if(iLo == 0) xBest=yBest; // The best division point
                if(iUp == m_nBin) yBest=xBest; // this is not really used
            }
        }
    if( (kBest >= m_nProj) || (kBest<0) ) throw FATALERROR("Something wrong with kBest");
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Carver(int &kBest, double &xBest, double &yBest)
{
    // The case of the optimization of the maximum weight.
    // Determine the best edge-candidate for future cell division,
    // using results of the MC exploration run within the cell stored in m_HistEdg
    int    kProj,iBin;
    double Carve,CarvTot,CarvMax,CarvOne,BinMax,BinTot;
    int    jLow,jUp,iLow,iUp;
    double TheBin;
    double *Bins  = new double[m_nBin];      // bins of histogram for single  PROJECTION
    kBest =-1;
    xBest =0.5;
    yBest =1.0;
    CarvMax = MIN;
    for(kProj=0; kProj<m_nProj; kProj++)
        if( m_MaskDiv[kProj] )
        {
            BinMax = MIN;
            for(iBin=0; iBin<m_nBin;iBin++)
            {
                Bins[iBin]= (m_HistEdg[kProj])->GetBinContent(iBin+1);      // Unload histogram
                BinMax = dmax( BinMax, Bins[iBin]);       // Maximum content/bin
            }
            if(BinMax < 0)    //case of empty cell
            {
                delete [] Bins;
                return;
            }
            CarvTot = 0.0;
            BinTot  = 0.0;
            for(iBin=0;iBin<m_nBin;iBin++)
            {
                CarvTot = CarvTot + (BinMax-Bins[iBin]);     // Total Carve (more stable)
                BinTot  +=Bins[iBin];
            }
            jLow =0;
            jUp  =m_nBin-1;
            CarvOne = MIN;
            for(iBin=0; iBin<m_nBin;iBin++)
            {
                TheBin = Bins[iBin];
                //-----  walk to the left and find first bin > TheBin
                iLow = iBin;
                for(int j=iBin; j>-1; j-- )
                {
                    if(TheBin< Bins[j]) break;
                    iLow = j;
                }
                //------ walk to the right and find first bin > TheBin
                iUp  = iBin;
                for(int j=iBin; j<m_nBin; j++)
                {
                    if(TheBin< Bins[j]) break;
                    iUp = j;
                }
                //
                Carve = (iUp-iLow+1)*(BinMax-TheBin);
                if( Carve > CarvOne)
                {
                    CarvOne = Carve;
                    jLow = iLow;
                    jUp  = iUp;
                }
            }
            if( CarvTot > CarvMax)
            {
                CarvMax = CarvTot;
                kBest = kProj;    // Best edge
                xBest = ((double)(jLow))/m_nBin;
                yBest = ((double)(jUp+1))/m_nBin;
                if(jLow == 0 )       xBest = yBest;
                if(jUp  == m_nBin-1) yBest = xBest;
            }
        }
    if( (kBest >= m_nProj) || (kBest<0) ) throw FATALERROR("Something wrong with kBest");
    delete[] Bins;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::MakeAlpha()
{
    // HYP-CUBICAL SUBSPACE
    // Provides random vector Alpha  0< Alpha(i) < 1
    int k;
    if(m_kDim<1) return;
    // simply generate and load kDim uniform random numbers
    RandomArray(m_kDim,m_Rvec);   // kDim random numbers needed
    for(k=0; k<m_kDim; k++) m_Alpha[k] = m_Rvec[k];
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::MakeLambda()
{
    // SIMPLICAL SUBSPACE
    // Provides random vector Lambda such that Sum Lamba(i) < 1, with uniform
    // probability. This  vector is used to populate uniformly the interior
    // of a simplex. The method is: generate point inside cube, order components
    // (maping into simplex) and take differences of Lambda(i+1) - Lambda(i)
    // For higher dimension random-walk method is used instead of ordering.
    int i,k;
    double x, sum;
    if(m_nDim<1) return;
    int nDimMax = 4; // maximum dimension for bubble-sort ordering method
    if (m_nDim>nDimMax)                    // faster random-walk algorithm for high dimensions
    {
        RandomArray(m_nDim+1,m_Rvec);      // nDim+1 random numbers needed
        sum = 0.0;
        for(i=0; i<=m_nDim; i++)
        {
            sum += -log(m_Rvec[i]);
            m_Rvec[i]=sum;                 // ordering is here automatic
        }
        for(i=0; i<m_nDim; i++) m_Rvec[i] = m_Rvec[i] /m_Rvec[m_nDim]; // normalize to last one
    }
    else                                   // bubble-sort ordering (mapping cube->simplex)
    {
        RandomArray(m_nDim,m_Rvec);        // nDim random numbers needed
        for(i=m_nDim-1; i>=0; i--)
        {
            for(k=1; k<=i; k++){
                if ( m_Rvec[k] < m_Rvec[k-1] )
                {
                    x = m_Rvec[k]; m_Rvec[k] = m_Rvec[k-1]; m_Rvec[k-1] = x; // get ordering
                }
            }
        }
    }
    m_Lambda[0] = m_Rvec[0];
    for(k = 1 ; k < m_nDim ; k++)
        m_Lambda[k] = m_Rvec[k] - m_Rvec[k-1]; // Sum of lambda's should < 1 !!!
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Grow()
{
    long iCell;
    FoamCell* newCell=0;
    while ((m_LastCe+2) < m_nCells)     // this condition also checked inside Divide
    {
        switch (m_OptPeek)
        {
        case 0:
            iCell = PeekMax();          // peek up cell with maximum driver integral
            if ((iCell<0) || (iCell>m_LastCe)) throw FATALERROR("Something wrong with iCell");
            newCell = m_Cells[iCell];
            break;
        case 1:
            newCell = PeekRan();        // peek up randomly one cell, tree-wise algorithm
            break;
        default:
            throw FATALERROR("Incorrect value for m_OptPeek");
        }
        Divide(newCell);                // and divide it into two
    }
    CheckAll();
}

///////////////////////////////////////////////////////////////////////////////

long
Foam::PeekMax()
{
    // Initialization, Finds cell with maximal Driver integral
    long  i;
    long iCell = -1;
    double  DrivMax, Driv;
    DrivMax = -1E+150;
    for(i=0; i<=m_LastCe; i++) //without root
    {
        if( m_Cells[i]->GetStat() == 1 )
        {
            Driv =  fabs( m_Cells[i]->GetDriv());
            if(Driv > DrivMax)
            {
                DrivMax = Driv;
                iCell = i;
            }
        }
    }
    if (iCell == -1) throw FATALERROR("PeekMax not found for cell no " + QString::number(iCell));
    return(iCell);
}

///////////////////////////////////////////////////////////////////////////////

FoamCell*
Foam::PeekRan()
{
    // Initialization phase.
    //       Tree-wise algorithm
    //       Peek up randomly pointer iCell of an active cell
    // We walk randomly from top of tree downwards until we find active
    // cell m_CeStat=1
    // At each step one of daugters cells is choosen randomly according
    // to their volume estimates.
    long    iCell,iDau,nDaut;
    double  random,p1,volu1,volu2,sum;
    FoamCell *newCell = m_Cells[0];
    if( (m_nDim>1)&&(m_OptOrd==0))
    {
        // 0-th cell is special because has nDim! daughters
        double TotDriv = 0.0;
        double TotIntg = 0.0;
        nDaut = silnia(m_nDim);
        for(iCell= 1 ; iCell<=nDaut ; iCell++)  // without root
        {
            TotDriv += m_Cells[iCell]->GetDriv();
            TotIntg += m_Cells[iCell]->GetIntg();
        }
        if( (fabs(TotDriv -m_Cells[0]->GetDriv() ) > 1.0e-5*TotDriv) )
            throw FATALERROR("Something wrong with total driver integral");
        if( (fabs(TotIntg -m_Cells[0]->GetIntg() ) > 1.0e-5*TotDriv) )
            throw FATALERROR("Something wrong with total true integral");
        random=m_PseRan->uniform();
        iDau  = 0;
        sum   = 0.0;
        for(iCell = 1 ; iCell<=nDaut ; iCell++)  //without root
        {
            iDau = iCell;
            sum += m_Cells[iCell]->GetDriv();
            if( random < sum/TotDriv ) break;
        }
        if (iDau<1 ) throw FATALERROR("Something went wrong: iDau<1");
        newCell = m_Cells[iDau];
        if( newCell->GetStat() == 1 ) return newCell;
    }
    // now the other standard cells with 2 daughters
    while ( newCell->GetStat() != 1 )
    {
        volu1 = newCell->GetDau0()->GetDriv();
        volu2 = newCell->GetDau1()->GetDriv();
        p1 = volu1/(volu1+volu2);
        random=m_PseRan->uniform();
        if( random < p1 )
            newCell = newCell->GetDau0();
        else
            newCell = newCell->GetDau1();
    }
    return newCell;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::Divide(FoamCell *Cell)
{
    //  Divide cell iCell into two daughter cells.
    //  The iCell is retained and taged as inactive, daughter cells are appended
    //  at the end of the buffer.
    //  New vertex is added to list of vertices.
    //  List of active cells is updated, iCell remooved, two daughters added
    //  and their properties set with help of MC sampling (Foam_Explore)
    //  Return Code RC=-1 of buffer limit is reached,  m_LastCe=m_nBuf
    double Xdiv;
    int  *kVer1 =NULL;
    int  *kVer2 =NULL;
    FoamVector *Posi1  =NULL;
    FoamVector *Size1  =NULL;
    FoamVector *Posi2  =NULL;
    FoamVector *Size2  =NULL;
    FoamVector *NewVert, *OldVe1, *OldVe2;
    int   kBest, j, jv, Old1,Old2;
    if(m_LastCe+1 >= m_nCells) throw FATALERROR("Buffer limit is reached, m_LastCe==m_nBuf");
    if(m_nDim>0 && m_OptVert==0)
    {
        kVer1 = new int[m_nDim+1];  // array of pointers to vertices for daughter1
        kVer2 = new int[m_nDim+1];  // array of pointers to vertices for daughter2
        for(jv=0; jv<m_nDim+1; jv++)
        {
            kVer1[jv] = (*Cell)(jv);   //inherit vertex pointers from parent
            kVer2[jv] = (*Cell)(jv);   //inherit vertex pointers from parent
        }
    }
    if( (m_kDim>0)  && (m_OptMCell==0) )
    {
        Posi1 = new FoamVector(m_kDim);
        Posi2 = new FoamVector(m_kDim);
        Size1 = new FoamVector(m_kDim);
        Size2 = new FoamVector(m_kDim);
        Cell->GetHcub( *Posi1 , *Size1 );   //inherit from parent
        Cell->GetHcub( *Posi2 , *Size2 );   //inherit from parent
    }
    Cell->SetStat(0); // reset Cell as inactive
    m_NoAct--;
    Xdiv  = Cell->GetXdiv();
    kBest = Cell->GetBest();
    if( kBest<0 || kBest>=m_nProj ) throw FATALERROR("Something wrong with kBest");
    int nProj0 = (m_nDim+1)*m_nDim/2;
    if( ( m_P0Si <= kBest ) && ( kBest < m_P0Si+nProj0 ) )
    {
        if(m_OptVert==0)
        {
            // decoding pair (Old1,Old2) from kBest (a bit tricky method, see $**$)
            Old1 =0; Old2 =0;
            for(j=m_P0Si; j<=kBest; j++)
            {
                Old2++;
                if( Old2 > m_nDim ) { Old1++; Old2=Old1+1; }
            }
            OldVe1 = (*Cell)[Old1];
            OldVe2 = (*Cell)[Old2];
            if (m_LastVe+1>m_vMax) throw FATALERROR("Too short list of vertices");
            m_LastVe++;                  // add new vertex to the list
            NewVert = m_VerX[m_LastVe];  // pointer of new vertex
            for(j=0; j<m_nDim; j++)      // define components of new vertex using Xdiv and (Old1,Old2)
                (*NewVert)[j] = Xdiv* (*OldVe1)[j] + (1.0-Xdiv)* (*OldVe2)[j];
            kVer1[Old1] = m_LastVe;     // old pointer is overwriten
            kVer2[Old2] = m_LastVe;     // old pointer is overwriten
        }
    }
    else
    {
        if( m_OptMCell==0 )
        {
            // The best division is in hyp-cubical subspace
            int kDiv = kBest-m_P0Cu;
            if(kDiv<0||kDiv>=m_kDim) throw FATALERROR("Something is wrong with kDiv");
            (*Posi1)[kDiv]=                   (*Posi1)[kDiv];      // (1) Position unchanged
            (*Size1)[kDiv]=              Xdiv*(*Size1)[kDiv];      // (1) Size reduced by Xdiv
            (*Posi2)[kDiv]=   (*Posi1)[kDiv] +(*Size1)[kDiv];      // (2) Position shifted
            (*Size2)[kDiv]=        (1.0-Xdiv)*(*Size2)[kDiv];      // (2) Size reduced by (1-Xdiv)
        }
    }

    // Define two daughter cells (active)
    // Status, Parent, Verts,  Posi, Size
    int d1 = CellFill(1, Cell, kVer1, Posi1, Size1);
    int d2 = CellFill(1, Cell, kVer2, Posi2, Size2);
    Cell->SetDau0(d1);
    Cell->SetDau1(d2);
    Explore( (m_Cells[d1]) );
    Explore( (m_Cells[d2]) );

    // Cleanup
    if(kVer1 != NULL)  delete[] kVer1;
    if(kVer2 != NULL)  delete[] kVer2;
    if(Posi1 != NULL)  delete Posi1;
    if(Posi2 != NULL)  delete Posi2;
    if(Size1 != NULL)  delete Size1;
    if(Size2 != NULL)  delete Size2;
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::MakeActiveList()
{
    //  Finds out number of active cells m_NoAct,
    //  Creates list of active cell m_CellsAct and primary cumulative m_PrimAcu
    //  They are used during MC to choose randomly an active cell
    long n, iCell;
    double sum;
    // fush previous result
    if(m_PrimAcu  != NULL) delete [] m_PrimAcu;
    if(m_CellsAct != NULL) delete [] m_CellsAct;
    // Count Active cells and find total Primary
    m_Prime = 0.0; n = 0;
    for(iCell=0; iCell<=m_LastCe; iCell++)
    {
        if (m_Cells[iCell]->GetStat()==1)
        {
            m_Prime += m_Cells[iCell]->GetPrim();
            n++;
        }
    }
    // Allocate tables of active cells
    m_NoAct = n;
    m_CellsAct = new FoamCell*[m_NoAct]; // list of pointers
    m_PrimAcu  = new  double[m_NoAct]; // acumulative primary for MC generation

    // Fill-in tables of active cells
    n = 0;
    for(iCell=0; iCell<=m_LastCe; iCell++)
    {
        if( m_Cells[iCell]->GetStat() == 1 )
        {
            m_CellsAct[n] = m_Cells[iCell]; // ?????
            n++;
        }
    }
    sum = 0.0;
    for(iCell=0; iCell<m_NoAct; iCell++)
    {
        sum = sum +(m_CellsAct[iCell])->GetPrim()/m_Prime;
        m_PrimAcu[iCell]=sum;
    }
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::GenerCell(FoamCell*& pCell)
{
    //  Generation phase.
    //  Return randomly chosen active cell with probability equal to its
    //  contribution into total driver integral using binary search .
    long  lo=0, hi=m_NoAct-1, hit;
    double Random;
    Random=m_PseRan->uniform();
    while(lo+1<hi)
    {
        hit = (lo+hi)/2;
        if (m_PrimAcu[hit]>Random)
            hi = hit;
        else
            lo = hit;
    }
    if (m_PrimAcu[lo]>Random)
        pCell = m_CellsAct[lo];
    else
        pCell = m_CellsAct[hi];
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::GenerCel2(FoamCell*& pCell)
{
    // Generation phase
    // Return randomly chosen active cell with probability equal to its
    // contribution into total driver integral using interpolation search
    long  lo, hi, hit;
    double fhit, flo, fhi;
    double Random;
    Random=m_PseRan->uniform();
    lo  = 0;              hi =m_NoAct-1;
    flo = m_PrimAcu[lo];  fhi=m_PrimAcu[hi];
    while(lo+1<hi)
    {
        hit = lo + (int)( (hi-lo)*(Random-flo)/(fhi-flo)+0.5);
        if (hit<=lo)
            hit = lo+1;
        else if(hit>=hi)
            hit = hi-1;
        fhit=m_PrimAcu[hit];
        if (fhit>Random)
        {
            hi = hit;
            fhi = fhit;
        }
        else
        {
            lo = hit;
            flo = fhit;
        }
    }
    if (m_PrimAcu[lo]>Random)
        pCell = m_CellsAct[lo];
    else
        pCell = m_CellsAct[hi];
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::MakeEvent()
{
    // Generation phase.
    // Generates point/vector Xrand with the weight MCwt.
    int      j,iv;
    double   Wt,Dx,MCwt;
    FoamCell *rCell;
    FoamVector  VRand(m_nDim),Lambda(m_nDim+1), X(m_nDim);
    int OptGener =2;      // local dip-switch for tests!!!
    //
    //********************** MC LOOP STARS HERE **********************
ee0:
    if(OptGener==1)
        GenerCell(rCell);   // primitive - more effective for small number of cells (?)
    else
        GenerCel2(rCell);   // choose randomly one cell

    MakeLambda();
    MakeAlpha();
    if( m_OptVert && m_nDim>0 )
    {
        for(j=0; j<m_nDim; j++) Lambda[j]=m_Lambda[j];
        Lambda[m_nDim]=0.;
        VRand=0.;
        rCell->GetXSimp(VRand,Lambda,m_nDim);
        for(j=0; j<m_nDim; j++)
            m_MCvect[m_N0Si+j]=VRand[j];
    }
    else
    {
        for(j=0 ;  j<m_nDim ; j++)
        {
            m_MCvect[m_N0Si+j] = (*(*rCell)[m_nDim])[j];
            for(iv=0 ; iv <m_nDim ; iv++)
                m_MCvect[m_N0Si+j] += m_Lambda[iv] *( (*(*rCell)[iv])[j] - (*(*rCell)[m_nDim])[j] );
        }
    }
    FoamVector  Posi(m_kDim); FoamVector  Size(m_kDim);
    rCell->GetHcub(Posi,Size);
    for(j=0; j<m_kDim; j++)
        m_MCvect[m_N0Cu+j]= Posi[j] +m_Alpha[j]*Size[j];
    Dx = rCell->GetVolume();      // Cartesian volume of the Cell
    //  weight average normalised to PRIMARY integral over the cell
    Wt = m_Rho->foamdensity(m_TotDim, m_MCvect )*Dx;
    MCwt = Wt / rCell->GetPrim();  // PRIMARY controls normalization
    m_nCalls++;
    m_MCwt   =  MCwt;
    // accumulation of statistics for the main MC weight
    m_SumWt  += MCwt;           // sum of Wt
    m_SumWt2 += MCwt*MCwt;      // sum of Wt**2
    m_NevGen++;                 // sum of 1d0
    m_WtMax  =  dmax(m_WtMax, MCwt);   // maximum wt
    m_WtMin  =  dmin(m_WtMin, MCwt);   // minimum wt
    m_HistWt->Fill(MCwt,1.0);          // histogram
    //*******  Optional rejection ******
    if(m_OptRej == 1)
    {
        double Random;
        Random=m_PseRan->uniform();
        if( m_MaxWtRej*Random > m_MCwt) goto ee0;  // Wt=1 events, internal rejection
        if( m_MCwt<m_MaxWtRej )
        {
            m_MCwt = 1.0;                  // normal Wt=1 event
        }
        else
        {
            m_MCwt = m_MCwt/m_MaxWtRej;    // weight for overveighted events! kept for debug
            m_SumOve += m_MCwt-m_MaxWtRej; // contribution of overveighted
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::GetMCvect(double* MCvect)
{
    // Returns generated point/vector
    for ( int k=0 ; k<m_TotDim ; k++) *(MCvect +k) = m_MCvect[k];
}

///////////////////////////////////////////////////////////////////////////////

double
Foam::GetMCwt()
{
    // Returns point/vector MCvect weight MCwt
    return(m_MCwt);
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::GetMCwt(double& MCwt)
{
    // Returns point/vector MCvect weight MCwt
    MCwt=m_MCwt;
}

///////////////////////////////////////////////////////////////////////////////

double
Foam::MCgenerate(double* MCvect)
{
    // Generates event and returns weight MCvect
    MakeEvent();
    GetMCvect(MCvect);
    return(m_MCwt);
}

///////////////////////////////////////////////////////////////////////////////

void
Foam::CheckAll()
{
    // checks on cells
    m_NoAct=0;
    for(int iCell=1; iCell<=m_LastCe; iCell++)  // without root
    {
        FoamCell* Cell = m_Cells[iCell];
        if (Cell->GetStat()==1)
            m_NoAct++;

        //  checking general rules
        if( ((Cell->GetDau0()==NULL) && (Cell->GetDau1()!=NULL) ) ||
            ((Cell->GetDau1()==NULL) && (Cell->GetDau0()!=NULL) ) )
            throw FATALERROR("Cell no " + QString::number(iCell) + " has only one daughter");
        if( (Cell->GetDau0()==NULL) && (Cell->GetDau1()==NULL) && (Cell->GetStat()==0) )
            throw FATALERROR("Cell no " + QString::number(iCell) + " has no daughter and is inactive");
        if( (Cell->GetDau0()!=NULL) && (Cell->GetDau1()!=NULL) && (Cell->GetStat()==1) )
            throw FATALERROR("Cell no " + QString::number(iCell) + " has two daughters and is active");

        // checking parents
        if( (Cell->GetPare())!=m_Cells[0] )   // not child of the root
        {
            if ( (Cell != Cell->GetPare()->GetDau0()) && (Cell != Cell->GetPare()->GetDau1()) )
                throw FATALERROR("Cell no " + QString::number(iCell) + " parent not pointing to this cell");
        }

        // checking daughters
        if(Cell->GetDau0()!=NULL)
        {
            if(Cell != (Cell->GetDau0())->GetPare())
                throw FATALERROR("Cell no " + QString::number(iCell) + " daughter 0 not pointing to this cell");
        }
        if(Cell->GetDau1()!=NULL)
        {
            if(Cell != (Cell->GetDau1())->GetPare())
                throw FATALERROR("Cell no " + QString::number(iCell) + " daughter 1 not pointing to this cell");
        }
    }

    // checks on vertices
    if(m_nDim>0)
    {
        for(int iVert=0; iVert<=m_LastVe; iVert++)
        {
            int Ref = 0; //False
            for(int jCe=0; jCe<=m_LastCe; jCe++)
            {
                FoamCell* Cell = m_Cells[jCe];
                int nVert = Cell->GetnVert();
                for(int k=0; k<nVert; k++)
                {
                    if( (*Cell)[k]==m_VerX[iVert] )
                    {
                        Ref = 1; break; // True;
                    }
                }
                if (Ref==1) break;
            }
            if (Ref==0) throw FATALERROR("Vertex no " + QString::number(iVert) + " NOT referenced");
        }
    }

    // check for empty cells
    for(int iCell=0; iCell<=m_LastCe; iCell++)
    {
        FoamCell* Cell = m_Cells[iCell];
        if( (Cell->GetStat()==1) && (Cell->GetDriv()==0) )
            throw FATALERROR("Cell no " + QString::number(iCell) + " is active but empty");
    }
}

///////////////////////////////////////////////////////////////////////////////////////

void
Foam::RandomArray(int size, double* vect)
{
    for( int i=0; i < size; ++i) vect[i] = m_PseRan->uniform();
}

///////////////////////////////////////////////////////////////////////////////////////
