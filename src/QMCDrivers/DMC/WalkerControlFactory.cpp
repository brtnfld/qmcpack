//////////////////////////////////////////////////////////////////
// (c) Copyright 2005- by Jeongnim Kim
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//   Jeongnim Kim
//   National Center for Supercomputing Applications &
//   Materials Computation Center
//   University of Illinois, Urbana-Champaign
//   Urbana, IL 61801
//   e-mail: jnkim@ncsa.uiuc.edu
//   Tel:    217-244-6319 (NCSA) 217-333-3324 (MCC)
//
// Supported by 
//   National Center for Supercomputing Applications, UIUC
//   Materials Computation Center, UIUC
//////////////////////////////////////////////////////////////////
// -*- C++ -*-
#include "QMCDrivers/DMC/WalkerControlFactory.h"
#include "QMCDrivers/DMC/WalkerReconfiguration.h"
#if defined(HAVE_MPI)
#include "QMCDrivers/DMC/WalkerControlMPI.h"
#include "QMCDrivers/DMC/WalkerReconfigurationMPI.h"
#endif
namespace qmcplusplus {

#if defined(HAVE_MPI)
  WalkerControlBase* 
    CreateWalkerController(int& swapmode, 
        int nideal, int nmax, int nmin, WalkerControlBase* wc) {

      int ncontexts = OHMMS::Controller->ncontexts();

      //overwrite the SwapMode
      if(ncontexts == 1) { swapmode=0;}

      if(swapmode) {
        int npernode=nideal/ncontexts;
        nmax=2*npernode+1;
        nmin=npernode/5+1;
      } else {
        nmax=2*nideal;
        nmin=nideal/2;
      }

      if(wc) {
        if(swapmode != wc->SwapMode) {
          delete wc;
          wc=0;
        }
      } 

      if(wc == 0) {
        if(swapmode) {
          wc = new WalkerControlMPI;
          //wc = new AsyncWalkerControl;
        } else {
          wc = new WalkerControlBase;
        }
      }
      wc->Nmin=nmin;
      wc->Nmax=nmax;
      return wc;
    }
#else
  WalkerControlBase* CreateWalkerController(
      bool reconfig, int& swapmode, int nideal,
      int nmax, int nmin, WalkerControlBase* wc) {
    //reset to 0 so that never ask the same question
    swapmode = 0;
    //if(nmax<0) nmax=2*nideal;
    //if(nmin<0) nmin=nideal/2;

    //if(wc== 0) wc= new WalkerControlBase;
    if(wc== 0) {
      if(reconfig) {
        app_log() << "  Using a fixed number of walkers by reconfiguration." << endl;
        wc = new WalkerReconfiguration;
        wc->Nmax=nideal;
        wc->Nmin=nideal;
      } else {
        app_log() << "  Using a WalkerControlBase with population fluctations." << endl;
        wc = new WalkerControlBase;
        wc->Nmax=2*nideal;
        wc->Nmin=nideal/2;
      }
    }
    return wc;
  }
#endif
}
/***************************************************************************
 * $RCSfile$   $Author$
 * $Revision$   $Date$
 * $Id$ 
 ***************************************************************************/

