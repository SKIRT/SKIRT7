/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FOAMHISTOGRAM_HPP
#define FOAMHISTOGRAM_HPP

////////////////////////////////////////////////////////////////////

/** A FoamHistogram instance represents a histogram used by the Foam class. This class is part
    of the foam classes that have been copied from an external library and adjusted to fit the
    SKIRT mold. There is no documentation other than the sparse comments in the source code. */
class FoamHistogram
{
private:
    double  m_Entries;              // no of Entries
    int     m_Nb;                   // no of Bins
    double  m_xmin;                 // (xmin,xmax) is histo range
    double  m_xmax;                 // (xmin,xmax) is histo range
    double* m_Bin1;                 // Bin sum wt
    double* m_Bin2;                 // Bin sum wt*wt
public:
    FoamHistogram(double, double, int);         // Constructor
    ~FoamHistogram();                           // Destructor
    void Fill(double, double);                  // Fill histogram
    double GetBinContent(int);                  // Get content of bin nb
    double GetBinError(int);                    // Get error of bin nb
    double GetEntries() { return m_Entries; }   // Get no. of entries
    int GetNbin() { return m_Nb; }              // Get no. of bin nb
    void Reset();                               // Reset bin content
private:
    FoamHistogram();
    FoamHistogram(const FoamHistogram&);
    FoamHistogram& operator= (const FoamHistogram&);
};

/////////////////////////////////////////////////////////////////////////////

#endif // FOAMHISTOGRAM_HPP
