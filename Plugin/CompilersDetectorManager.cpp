//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilersDetectorManager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "CompilersDetectorManager.h"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorGCC.h"
#include "CompilerLocatorCrossGCC.h"
#include "CompilerLocatorMSVC9.h"  // VS 2008
#include "CompilerLocatorMSVC10.h" // VS 2010
#include "CompilerLocatorMSVC11.h" // VS 2012
#include "CompilerLocatorMSVC12.h" // VS 2013
#include "CompilerLocatorCLANG.h"
#include "CompilerLocatorCygwin.h"
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include "cl_config.h"
#include "macros.h"
#include <wx/arrstr.h>
#include <wx/choicdlg.h>

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC9()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC10()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC11()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC12()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCygwin()));

#elif defined(__WXGTK__)
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorGCC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));

#elif defined(__WXMAC__)
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorGCC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));

#endif
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCrossGCC()));
}

CompilersDetectorManager::~CompilersDetectorManager() {}

bool CompilersDetectorManager::Locate()
{
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter) {
        if((*iter)->Locate()) {
            m_compilersFound.insert(
                m_compilersFound.end(), (*iter)->GetCompilers().begin(), (*iter)->GetCompilers().end());
        }
    }
    return !m_compilersFound.empty();
}

CompilerPtr CompilersDetectorManager::Locate(const wxString& folder)
{
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter) {
        CompilerPtr comp = (*iter)->Locate(folder);
        if(comp) {
            return comp;
        }
    }
    return NULL;
}

bool CompilersDetectorManager::FoundMinGWCompiler() const
{
    for(size_t i = 0; i < m_compilersFound.size(); ++i) {
        CompilerPtr compiler = m_compilersFound.at(i);
        if(compiler->GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
            // we found at least one MinGW compiler
            return true;
        }
    }
    return false;
}

void CompilersDetectorManager::MSWSuggestToDownloadMinGW()
{
#ifdef __WXMSW__
    if(::wxMessageBox(_("Could not locate any MinGW compiler installed on your machine, would you like to "
                        "install one now?"),
                      "CodeLite",
                      wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTER | wxICON_QUESTION) == wxYES) {
        // No MinGW compiler detected!, offer the user to download one
        wxStringMap_t mingwCompilers;
        wxArrayString options;
        mingwCompilers.insert(std::make_pair("MinGW 4.9.2 - 32 Bit",
                                             "http://sourceforge.net/projects/"
                                             "tdm-gcc/files/TDM-GCC%20Installer/"
                                             "tdm-gcc-4.9.2.exe/download"));
        mingwCompilers.insert(std::make_pair("MinGW 4.9.2 - 64 Bit",
                                             "http://sourceforge.net/projects/"
                                             "tdm-gcc/files/TDM-GCC%20Installer/"
                                             "tdm64-gcc-4.9.2-3.exe/download"));
        wxStringMap_t::iterator iter = mingwCompilers.begin();
        for(; iter != mingwCompilers.end(); ++iter) {
            options.Add(iter->first);
        }
#ifdef _WIN64
        int sel = 1;
#else
        int sel = 0;
#endif
        wxString selection =
            ::wxGetSingleChoice(_("Select a compiler to download"), _("Choose compiler"), options, sel);
        if(!selection.IsEmpty()) {
            // Reset the compiler detection flag so next time codelite is restarted, it will
            // rescan the machine
            clConfig::Get().Write(kConfigAutoDetectCompilerOnStartup, true);

            // Open the browser to start downloading the compiler
            ::wxLaunchDefaultBrowser(mingwCompilers.find(selection)->second);
            ::wxMessageBox(
                _("After install is completed, restart CodeLite"), "CodeLite", wxOK | wxCENTER | wxICON_INFORMATION);
        }
    }
#endif // __WXMSW__
}
