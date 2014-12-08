/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FoamHistogram.hpp"
#include "FatalError.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

FoamHistogram::FoamHistogram(double xmin, double xmax, int nb)
{
    m_Entries=0.0;
    int i;
    m_xmin=xmin;
    m_xmax=xmax;
    m_Nb=nb;
    m_Bin1 = 0;
    m_Bin2 = 0;
    if( m_xmin >= m_xmax || m_Nb<1 ) throw FATALERROR("Invalid arguments: xmax<xmin or Nb<1");

    if(nb>0)
    {
        m_Bin1 = new double[m_Nb+2];
        m_Bin2 = new double[m_Nb+2];
        for (i=0; i<=m_Nb+1; i++) m_Bin1[i]=0.0;
        for (i=0; i<=m_Nb+1; i++) m_Bin2[i]=0.0;
    }
}

///////////////////////////////////////////////////////////////////////////////

FoamHistogram::~FoamHistogram()
{
    delete[] m_Bin1;
    delete[] m_Bin2;
}

///////////////////////////////////////////////////////////////////////////////

void
FoamHistogram::Fill(double xx, double wt)
{
    // fill histogram with single weighted event
    int ib;
    ib = (int)( (xx-m_xmin)/(m_xmax-m_xmin) *m_Nb) +1;
    if(ib<1)    ib=0;       // underflow
    if(ib>m_Nb) ib=m_Nb+1;  // overflow
    m_Bin1[ib] += wt;
    m_Bin2[ib] += wt*wt;
    m_Entries  +=1.0;
}

///////////////////////////////////////////////////////////////////////////////

double
FoamHistogram::GetBinContent(int nb)
{
    // Get content of bin nb.    nb=1 for 1-st bin, underflow nb=0, overflow nb=Nb+1
    if( (nb>=0) && (nb<m_Nb+2) ) return m_Bin1[nb];
    else throw FATALERROR("Invalid bin index");
}

///////////////////////////////////////////////////////////////////////////////

double
FoamHistogram::GetBinError(int nb)
{
    // Get content of bin nb.    nb=1 for 1-st bin, underflow nb=0, overflow nb=Nb+1
    if( (nb>=0) && (nb<m_Nb+2) ) return sqrt(m_Bin2[nb]);
    else throw FATALERROR("Invalid bin index");
}

///////////////////////////////////////////////////////////////////////////////

void
FoamHistogram::Reset()
{
    // Reset bin content
    int i;
    for (i=0; i<=m_Nb+1; i++) m_Bin1[i]=0.0;
    for (i=0; i<=m_Nb+1; i++) m_Bin2[i]=0.0;
}

///////////////////////////////////////////////////////////////////////////////
