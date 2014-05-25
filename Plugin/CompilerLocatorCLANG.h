#ifndef COMPILERLOCATORCLANG_H
#define COMPILERLOCATORCLANG_H

#include "ICompilerLocator.h" // Base class: ICompilerLocator

class CompilerLocatorCLANG : public ICompilerLocator
{
protected:
    void MSWLocate();
    void AddTools(CompilerPtr compiler, const wxString &installFolder);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs = "");
    
public:
    CompilerLocatorCLANG();
    virtual ~CompilerLocatorCLANG();

public:
    virtual bool Locate();
};

#endif // COMPILERLOCATORCLANG_H