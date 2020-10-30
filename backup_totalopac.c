/* This file is part of Exo_Transmit.

    Exo_Transmit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Exo_Transmit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Exo_Transmit.  If not, see <http://www.gnu.org/licenses/>.
*/

/*----------------------- totalopac.c ----------------------------

Author: Eliza Kempton (kemptone@grinnell.edu)

------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "include.h"
#include "constant.h"
#include "atmos.h"
#include "opac.h"
#include "nrutil.h"
#include "vars.h"
#include "prototypes.h"

/* --- Global variables ------------------------------------------ */

extern struct Opac opac;
extern struct Atmos atmos;

struct Chem chem;

struct Opac opacCH4;
struct Opac opacCO2;
struct Opac opacCO;
struct Opac opacH2O;
struct Opac opacNH3;
struct Opac opacO2;
struct Opac opacO3;
struct Opac opacC2H2;
struct Opac opacC2H4;
struct Opac opacC2H6;
struct Opac opacCrH; 
struct Opac opacH2CO;
struct Opac opacH2S; 
struct Opac opacHCl; 
struct Opac opacHCN; 
struct Opac opacHF;
struct Opac opacMgH; 
struct Opac opacN2; 
struct Opac opacNO; 
struct Opac opacNO2;
struct Opac opacOH; 
struct Opac opacOCS;
struct Opac opacPH3;
struct Opac opacSH; 
struct Opac opacSiH; 
struct Opac opacSiO;
struct Opac opacSO2; 
struct Opac opacTiO; 
struct Opac opacVO; 
struct Opac opacK; 
struct Opac opacNa; 

struct Opac opacscat;
struct Opac opacCIA;

/* ---------------------------------------------------------------
 * Computes the total opacity due to all of the atmospheric 
 * constituents.
 * --------------------------------------------------------------- */

/* ------- begin ------------ TotalOpac.c ------------------------ */

void TotalOpac() {

  double **opac_CIA_H2H2, **opac_CIA_H2He, **opac_CIA_H2H, 
    **opac_CIA_H2CH4, **opac_CIA_CH4Ar, **opac_CIA_CH4CH4, 
    **opac_CIA_CO2CO2, **opac_CIA_HeH, **opac_CIA_N2CH4, 
    **opac_CIA_N2H2, **opac_CIA_N2N2, **opac_CIA_O2CO2, 
    **opac_CIA_O2N2, **opac_CIA_O2O2;
  double **kappa_nu;
  int i, j, k, a, b;
  
  char **fileArray = getFileArray(); 	//get file names
  vars variables = getVars(); 		//get planet variables
  int chemSelection[32]; 		//get chemistry selections
  
  getChemSelection(chemSelection); 
  
  int NLAMBDA = variables.NLAMBDA; 	//rename relevant variables
  int NPRESSURE = variables.NPRESSURE;
  int NTEMP = variables.NTEMP;
  int NTAU = variables.NTAU;
  double RAYLEIGH = variables.RAYLEIGH;
  
  /* Molecular masses */
  /* Masses sourced from http://www.webqc.org/mmcalc.php */
  
  double m_H2 = 2.0158;
  double m_H = 1.0079;
  double m_He = 4.002602;
  double m_H2O = 18.0152;
  double m_CH4 = 16.0423;
  double m_CO = 28.010;
  double m_CO2 = 44.010;
  double m_O = 15.9994;
  double m_C = 12.0107;
  double m_N = 14.0067;
  double m_NH3 = 17.031;
  double m_N2 = 28.0134;
  double m_O2 = 31.9988;
  double m_O3 = 47.9982;
  double m_C2H2 = 26.0373;
  double m_C2H4 = 28.0532;
  double m_C2H6 = 30.0690;
  double m_H2CO = 30.0260;
  double m_H2S = 34.0809;
  double m_HCl= 36.4609;
  double m_HCN = 27.0253;
  double m_HF = 20.0063;
  double m_MgH = 25.3129;
  double m_NO = 30.0061;
  double m_NO2 = 46.0055;
  double m_OCS = 60.0751;
  double m_OH = 17.0073;
  double m_PH3 = 33.9976;
  double m_SH = 33.0729;
  double m_SiH = 29.0934;
  double m_SiO = 44.0849;
  double m_SO2 = 64.0638;
  double m_TiO = 63.8664;
  double m_VO = 66.9409;
  double m_Na = 22.988977; 
  double m_K = 39.0983; 
	
  FILE *f1;
  
  /* Allocate Memory */
  
  opac_CIA_H2H2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_H2He = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_H2H = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_H2CH4 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_CH4Ar = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_CH4CH4 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_CO2CO2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_HeH = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_N2CH4 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_N2H2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_N2N2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_O2CO2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_O2N2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  opac_CIA_O2O2 = dmatrix(0, NTEMP-1, 0, NLAMBDA-1);
  
  /* Read Chemistry Table */
  
  ReadChemTable();
  printf("ReadChemTable done\n");
  
  /* Allocate for total opacity */

  opac.name = "Total";          //Name it Total
  opac.T = dvector(0, NTEMP-1); //Declare T, P, Plog10, and kappa arrays
  opac.P = dvector(0, NPRESSURE-1);
  opac.Plog10 = dvector(0, NPRESSURE-1);
  opac.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
  //opac.abundance = dvector(0, NTAU-1);
  
  //populate with zeros	
  for (i=0; i<NLAMBDA; i++)
    for (j=0; j<NPRESSURE; j++)
        for (k=0; k<NTEMP; k++)
	    opac.kappa[i][j][k] = 0.;
  
  /* Fill in mean molecular weight (mu) values */
  
  chem.mu = dvector(0, NTAU-1);
  
  for (j=0; j<NTAU; j++) {
      chem.mu[j] = 
	chem.H2[j]* m_H2 
	+ chem.H[j]* m_H 
	+ chem.He[j] * m_He 
	+ chem.H2O[j] * m_H2O 
	+ chem.C[j] * m_C 
	+ chem.CH4[j] * m_CH4 
	+ chem.CO[j] * m_CO 
	+ chem.CO2[j] * m_CO2 
	+ chem.O[j] * m_O 
	+ chem.N[j] * m_N 
	+ chem.NH3[j] * m_NH3 
	+ chem.N2[j] * m_N2 
	+ chem.O2[j] * m_O2 
	+ chem.O3[j] * m_O3
	+ chem.C2H2[j] * m_C2H2 
	+ chem.C2H4[j] * m_C2H4
	+ chem.C2H6[j] * m_C2H6
	+ chem.HCN[j] * m_HCN 
	+ chem.HCl[j] * m_HCl
	+ chem.HF[j] * m_HF 
	+ chem.H2CO[j] * m_H2CO
	+ chem.H2S[j] * m_H2S 
	+ chem.MgH[j] * m_MgH
	+ chem.NO2[j] * m_NO2 
	+ chem.NO[j] * m_NO
	+ chem.OH[j] * m_OH 
	+ chem.PH3[j] * m_PH3
	+ chem.SH[j] * m_SH 
	+ chem.SO2[j] * m_SO2
	+ chem.SiH[j] * m_SiH 
	+ chem.SiO[j] * m_SiO
	+ chem.TiO[j] * m_TiO
	+ chem.VO[j] * m_VO 
	+ chem.OCS[j] * m_OCS 
	+ chem.Na[j] * m_Na 
	+ chem.K[j] * m_K;
  }

  /* This adds in the calculation to atmos.mu */
  atmos.mu = dvector(0, NTAU-1);
  for (j=0; j<NTAU; j++) {
      atmos.mu[j] = chem.mu[j];
  };

  printf("atmos.mu has been populated from chem.mu\n");

  /* Allocate atmos.kappa_nu and kappa_nu, and populate with zeros */
  atmos.kappa_nu = dmatrix(0, NLAMBDA-1, 0, NTAU-1);
  kappa_nu = dmatrix(0, NLAMBDA-1, 0, NTAU-1);
  for (i = 0; i < NLAMBDA; i++) {
      for (j = 0; j < NTAU; j++) {
          atmos.kappa_nu[i][j] = 0.;
          kappa_nu[i][j] = 0.;
      };
  };

  printf("Made it here");
  
  /* Fill in CH4 opacities */
  if(chemSelection[0] == 1){          //If CH4 is selected
    opacCH4.name = "CH4";             //Name it CH4
    opacCH4.T = dvector(0, NTEMP-1);  //Declare T, P, Plog10, and kappa arrays
    opacCH4.P = dvector(0, NPRESSURE-1);
    opacCH4.Plog10 = dvector(0, NPRESSURE-1);
    opacCH4.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    
    ReadOpacTable(opacCH4, fileArray[3]);     //Read opacity table for CH4
    
    /* Populate atmos.kappa_nu */
    for (i=0; i<NLAMBDA; i++){
      for (j=0; j<NTAU; j++) {
          /* Interpolate from TP grid onto the altitude grid */
          Locate(NTEMP, opacCH4.T, atmos.T[j], &a);
          Locate(NPRESSURE, opacCH4.P, atmos.P[j], &b);

          //if (j == 0) {
          //  printf("i = %d, j = %d, a = %d, b = %d, T = %e, P = %e, MR = %e\n",
          //        i, j, a, b, opacCH4.T[a], opacCH4.P[b], chem.CH4[j]);
          //};


          kappa_nu[i][j] = lint2D(opac.T[a], opac.T[a+1], opac.P[b], opac.P[b+1],
      			      opac.kappa[i][b][a], opac.kappa[i][b][a+1],
      			      opac.kappa[i][b+1][a], opac.kappa[i][b+1][a+1],
      			      atmos.T[j], atmos.P[j]);

	  /* Add to overall opac.kappa */
	  atmos.kappa_nu[i][j] += opacCH4.kappa[i][a][b] * chem.CH4[j];
      }
    };
    printf("Read CH4 Opacity done\n");	     //Confirmation message
    
    FreeOpacTable(opacCH4);                  //Free CH4 opacity table
  }
  
  //This procedure repeats for all gases!!
  
  /* Fill in CO2 opacities */
  if(chemSelection[1] == 1){
    opacCO2.name = "CO2";             //Name it CO2
    opacCO2.T = dvector(0, NTEMP-1);  //Declare T, P, Plog10, and kappa arrays
    opacCO2.P = dvector(0, NPRESSURE-1);
    opacCO2.Plog10 = dvector(0, NPRESSURE-1);
    opacCO2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    
    ReadOpacTable(opacCO2, fileArray[3]);     //Read opacity table for CO2
    
    /* Populate atmos.kappa_nu */
    for (i=0; i<NLAMBDA; i++){
      for (j=0; j<NTAU; j++) {
          /* Interpolate from TP grid onto the altitude grid */
          Locate(NTEMP, opacCO2.T, atmos.T[j], &a);
          Locate(NPRESSURE, opacCO2.P, atmos.P[j], &b);

          if (j == 0) {
            printf("i = %d, j = %d, a = %d, b = %d, T = %e, P = %e, MR = %e\n",
                  i, j, a, b, opacCO2.T[a], opacCO2.P[b], chem.CO2[j]);
          };

	  /* Add to overall opac.kappa */
	  atmos.kappa_nu[i][j] += opacCO2.kappa[i][a][b] * chem.CO2[j];
      }
    };
    printf("Read CO2 Opacity done\n");	     //Confirmation message
    
    FreeOpacTable(opacCO2);                  //Free CO2 opacity table
  }
  
  /* Fill in CO opacities */
  if(chemSelection[2] == 1){
    opacCO.name = "CO";
    opacCO.T = dvector(0, NTEMP-1);
    opacCO.P = dvector(0, NPRESSURE-1);
    opacCO.Plog10 = dvector(0, NPRESSURE-1);
    opacCO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacCO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacCO.abundance[j] = chem.CO[j];
    }
    ReadOpacTable(opacCO, fileArray[7]);
    
    //Populate opac.kappa
    for(i=0; i<NLAMBDA; i++)
      for(j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacCO.kappa[i][j][k];
    
    printf("Read CO Opacity done\n");

    FreeOpacTable(opacCO);
  }
  
  /* Fill in H2O opacities */
  if(chemSelection[3] == 1){
    opacH2O.name = "H2O";
    opacH2O.T = dvector(0, NTEMP-1);
    opacH2O.P = dvector(0, NPRESSURE-1);
    opacH2O.Plog10 = dvector(0, NPRESSURE-1);
    opacH2O.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacH2O.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacH2O.abundance[j] = chem.H2O[j];
    }
    ReadOpacTable(opacH2O, fileArray[10]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacH2O.kappa[i][j][k];
    
    printf("Read H2O Opacity done\n");

    FreeOpacTable(opacH2O);
  }
  
  /* Fill in NH3 opacities */
  if(chemSelection[4] == 1){
    opacNH3.name = "NH3";
    opacNH3.T = dvector(0, NTEMP-1);
    opacNH3.P = dvector(0, NPRESSURE-1);
    opacNH3.Plog10 = dvector(0, NPRESSURE-1);
    opacNH3.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacNH3.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacNH3.abundance[j] = chem.NH3[j];
    }
    ReadOpacTable(opacNH3, fileArray[17]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacNH3.kappa[i][j][k];
    
    printf("Read NH3 Opacity done\n");

    FreeOpacTable(opacNH3);
  }
  
  /* Fill in O2 opacities */
  if(chemSelection[5] == 1){
    opacO2.name = "O2";
    opacO2.T = dvector(0, NTEMP-1);
    opacO2.P = dvector(0, NPRESSURE-1);
    opacO2.Plog10 = dvector(0, NPRESSURE-1);
    opacO2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacO2.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacO2.abundance[j] = chem.O2[j];
    }
    ReadOpacTable(opacO2, fileArray[20]);
    
    //Populate opac.kappa
    for(i=0; i<NLAMBDA; i++)
      for(j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacO2.kappa[i][j][k];
    
    printf("Read O2 Opacity done\n");

    FreeOpacTable(opacO2);
  }
  
  /* Fill in O3 opacities */
  if(chemSelection[6] == 1){
    opacO3.name = "O3";
    opacO3.T = dvector(0, NTEMP-1);
    opacO3.P = dvector(0, NPRESSURE-1);
    opacO3.Plog10 = dvector(0, NPRESSURE-1);
    opacO3.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacO3.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacO3.abundance[j] = chem.O3[j];
    }
    ReadOpacTable(opacO3, fileArray[21]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacO3.kappa[i][j][k];
    
    printf("Read O3 Opacity done\n");
    
    FreeOpacTable(opacO3);
  }
  
  /* Fill in C2H2 opacities */
  if(chemSelection[7] == 1){
    opacC2H2.name = "C2H2";
    opacC2H2.T = dvector(0, NTEMP-1);
    opacC2H2.P = dvector(0, NPRESSURE-1);
    opacC2H2.Plog10 = dvector(0, NPRESSURE-1);
    opacC2H2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacC2H2.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacC2H2.abundance[j] = chem.C2H2[j];
    }
    ReadOpacTable(opacC2H2, fileArray[4]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacC2H2.kappa[i][j][k];
    
    printf("Read C2H2 Opacity done\n");

    FreeOpacTable(opacC2H2);
  }
  
  /* Fill in C2H4 opacities */
  if(chemSelection[8] == 1){
    opacC2H4.name = "C2H4";
    opacC2H4.T = dvector(0, NTEMP-1);
    opacC2H4.P = dvector(0, NPRESSURE-1);
    opacC2H4.Plog10 = dvector(0, NPRESSURE-1);
    opacC2H4.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacC2H4.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacC2H4.abundance[j] = chem.C2H4[j];
    }
    ReadOpacTable(opacC2H4, fileArray[5]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacC2H4.kappa[i][j][k];
    
    printf("Read C2H4 Opacity done\n");

    FreeOpacTable(opacC2H4);
  }
  
  /* Fill in C2H6 opacities */
  if(chemSelection[9] == 1){
    opacC2H6.name = "C2H6";
    opacC2H6.T = dvector(0, NTEMP-1);
    opacC2H6.P = dvector(0, NPRESSURE-1);
    opacC2H6.Plog10 = dvector(0, NPRESSURE-1);
    opacC2H6.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacC2H6.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacC2H6.abundance[j] = chem.C2H6[j];
    }
    ReadOpacTable(opacC2H6, fileArray[6]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacC2H6.kappa[i][j][k];
    
    printf("Read C2H6 Opacity done\n");

    FreeOpacTable(opacC2H6);
  }
  
  /* Fill in H2CO opacities */
  if(chemSelection[10] == 1){
    opacH2CO.name = "H2CO";
    opacH2CO.T = dvector(0, NTEMP-1);
    opacH2CO.P = dvector(0, NPRESSURE-1);
    opacH2CO.Plog10 = dvector(0, NPRESSURE-1);
    opacH2CO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacH2CO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacH2CO.abundance[j] = chem.H2CO[j];
    }
    ReadOpacTable(opacH2CO, fileArray[9]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacH2CO.kappa[i][j][k];
    
    printf("Read H2CO Opacity done\n");
    
    FreeOpacTable(opacH2CO);
  }
  
  /* Fill in H2S opacities */
  if(chemSelection[11] == 1){
    opacH2S.name = "H2S";
    opacH2S.T = dvector(0, NTEMP-1);
    opacH2S.P = dvector(0, NPRESSURE-1);
    opacH2S.Plog10 = dvector(0, NPRESSURE-1);
    opacH2S.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacH2S.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacH2S.abundance[j] = chem.H2S[j];
    }
    ReadOpacTable(opacH2S, fileArray[11]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacH2S.kappa[i][j][k];
    
    printf("Read H2S Opacity done\n");

    FreeOpacTable(opacH2S);
  }
  
  /* Fill in HCl opacities */
  if(chemSelection[12] == 1){
    opacHCl.name = "HCl";
    opacHCl.T = dvector(0, NTEMP-1);
    opacHCl.P = dvector(0, NPRESSURE-1);
    opacHCl.Plog10 = dvector(0, NPRESSURE-1);
    opacHCl.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacHCl.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacHCl.abundance[j] = chem.HCl[j];
    }
    ReadOpacTable(opacHCl, fileArray[13]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacHCl.kappa[i][j][k];
    
    printf("Read HCl Opacity done\n");

    FreeOpacTable(opacHCl);
  }
  
  /* Fill in HCN opacities */
  if(chemSelection[13] == 1){
    opacHCN.name = "HCN";
    opacHCN.T = dvector(0, NTEMP-1);
    opacHCN.P = dvector(0, NPRESSURE-1);
    opacHCN.Plog10 = dvector(0, NPRESSURE-1);
    opacHCN.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacHCN.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacHCN.abundance[j] = chem.HCN[j];
    }
    ReadOpacTable(opacHCN, fileArray[12]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacHCN.kappa[i][j][k];
    
    printf("Read HCN Opacity done\n");
    
    FreeOpacTable(opacHCN);
  }
  
  /* Fill in HF opacities */
  if(chemSelection[14] == 1){
    opacHF.name = "HF";
    opacHF.T = dvector(0, NTEMP-1);
    opacHF.P = dvector(0, NPRESSURE-1);
    opacHF.Plog10 = dvector(0, NPRESSURE-1);
    opacHF.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacHF.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacHF.abundance[j] = chem.HF[j];
    }
    ReadOpacTable(opacHF, fileArray[14]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacHF.kappa[i][j][k];
    
    printf("Read HF Opacity done\n");

    FreeOpacTable(opacHF);
  }
  
  /* Fill in MgH opacities */
  if(chemSelection[15] == 1){
    opacMgH.name = "MgH";
    opacMgH.T = dvector(0, NTEMP-1);
    opacMgH.P = dvector(0, NPRESSURE-1);
    opacMgH.Plog10 = dvector(0, NPRESSURE-1);
    opacMgH.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacMgH.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacMgH.abundance[j] = chem.MgH[j];
    }
    ReadOpacTable(opacMgH, fileArray[15]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacMgH.kappa[i][j][k];
    
    printf("Read MgH Opacity done\n");

    FreeOpacTable(opacMgH);
  }
  
  /* Fill in N2 opacities */
  if(chemSelection[16] == 1){
    opacN2.name = "N2";
    opacN2.T = dvector(0, NTEMP-1);
    opacN2.P = dvector(0, NPRESSURE-1);
    opacN2.Plog10 = dvector(0, NPRESSURE-1);
    opacN2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacN2.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacN2.abundance[j] = chem.N2[j];
    }
    ReadOpacTable(opacN2, fileArray[16]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacN2.kappa[i][j][k];
    
    printf("Read N2 Opacity done\n");

    FreeOpacTable(opacN2);
  }
  
  /* Fill in NO opacities */
  if(chemSelection[17] == 1){
    opacNO.name = "NO";
    opacNO.T = dvector(0, NTEMP-1);
    opacNO.P = dvector(0, NPRESSURE-1);
    opacNO.Plog10 = dvector(0, NPRESSURE-1);
    opacNO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacNO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacNO.abundance[j] = chem.NO[j];
    }
    ReadOpacTable(opacNO, fileArray[18]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacNO.kappa[i][j][k];
    
    printf("Read NO Opacity done\n");

    FreeOpacTable(opacNO);
  }
  
  /* Fill in NO2 opacities */
  if(chemSelection[18] == 1){
    opacNO2.name = "NO2";
    opacNO2.T = dvector(0, NTEMP-1);
    opacNO2.P = dvector(0, NPRESSURE-1);
    opacNO2.Plog10 = dvector(0, NPRESSURE-1);
    opacNO2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacNO2.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacNO2.abundance[j] = chem.NO2[j];
    }
    ReadOpacTable(opacNO2, fileArray[19]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacNO2.kappa[i][j][k];
    
    printf("Read NO2 Opacity done\n");

    FreeOpacTable(opacNO2);
  }
  
  /* Fill in OCS opacities */
  if(chemSelection[19] == 1){
    opacOCS.name = "OCS";
    opacOCS.T = dvector(0, NTEMP-1);
    opacOCS.P = dvector(0, NPRESSURE-1);
    opacOCS.Plog10 = dvector(0, NPRESSURE-1);
    opacOCS.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacOCS.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacOCS.abundance[j] = chem.OCS[j];
    }
    ReadOpacTable(opacOCS, fileArray[22]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacOCS.kappa[i][j][k];
    
    printf("Read OCS Opacity done\n");

    FreeOpacTable(opacOCS);
  }
  
  /* Fill in OH opacities */
  if(chemSelection[20] == 1){
    opacOH.name = "OH";
    opacOH.T = dvector(0, NTEMP-1);
    opacOH.P = dvector(0, NPRESSURE-1);
    opacOH.Plog10 = dvector(0, NPRESSURE-1);
    opacOH.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacOH.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacOH.abundance[j] = chem.OH[j];
    }
    ReadOpacTable(opacOH, fileArray[23]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacOH.kappa[i][j][k];
    
    printf("Read OH Opacity done\n");

    FreeOpacTable(opacOH);
  }
  
  /* Fill in PH3 opacities */
  if(chemSelection[21] == 1){
    opacPH3.name = "PH3";
    opacPH3.T = dvector(0, NTEMP-1);
    opacPH3.P = dvector(0, NPRESSURE-1);
    opacPH3.Plog10 = dvector(0, NPRESSURE-1);
    opacPH3.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacPH3.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacPH3.abundance[j] = chem.PH3[j];
    }
    ReadOpacTable(opacPH3, fileArray[24]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
	  opac.kappa[i][j][k] += opacPH3.kappa[i][j][k];
    
    printf("Read PH3 Opacity done\n");

    FreeOpacTable(opacPH3);
  }
  
  /* Fill in SH opacities */
  if(chemSelection[22] == 1){
    opacSH.name = "SH";
    opacSH.T = dvector(0, NTEMP-1);
    opacSH.P = dvector(0, NPRESSURE-1);
    opacSH.Plog10 = dvector(0, NPRESSURE-1);
    opacSH.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacSH.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacSH.abundance[j] = chem.SH[j];
    }
    ReadOpacTable(opacSH, fileArray[25]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacSH.kappa[i][j][k];
    
    printf("Read SH Opacity done\n");

    FreeOpacTable(opacSH);
  }
  
  /* Fill in SiH opacities */
  if(chemSelection[23] == 1){
    opacSiH.name = "SiH";
    opacSiH.T = dvector(0, NTEMP-1);
    opacSiH.P = dvector(0, NPRESSURE-1);
    opacSiH.Plog10 = dvector(0, NPRESSURE-1);
    opacSiH.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacSiH.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacSiH.abundance[j] = chem.SiH[j];
    }
    ReadOpacTable(opacSiH, fileArray[27]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacSiH.kappa[i][j][k];
    
    printf("Read SiH Opacity done\n");

    FreeOpacTable(opacSiH); 
  }
  
  /* Fill in SiO opacities */
  if(chemSelection[24] == 1){
    opacSiO.name = "SiO";
    opacSiO.T = dvector(0, NTEMP-1);
    opacSiO.P = dvector(0, NPRESSURE-1);
    opacSiO.Plog10 = dvector(0, NPRESSURE-1);
    opacSiO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacSiO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacSiO.abundance[j] = chem.SiO[j];
    }
    ReadOpacTable(opacSiO, fileArray[28]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacSiO.kappa[i][j][k];
    
    printf("Read SiO Opacity done\n");

    FreeOpacTable(opacSiO);
  }
  
  /* Fill in SO2 opacities */
  if(chemSelection[25] == 1){
    opacSO2.name = "SO2";
    opacSO2.T = dvector(0, NTEMP-1);
    opacSO2.P = dvector(0, NPRESSURE-1);
    opacSO2.Plog10 = dvector(0, NPRESSURE-1);
    opacSO2.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacSO2.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacSO2.abundance[j] = chem.SO2[j];
    }
    ReadOpacTable(opacSO2, fileArray[26]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacSO2.kappa[i][j][k];
    
    printf("Read SO2 Opacity done\n");

    FreeOpacTable(opacSO2);
  }
  
  /* Fill in TiO opacities */
  if(chemSelection[26] == 1){
    opacTiO.name = "TiO";
    opacTiO.T = dvector(0, NTEMP-1);
    opacTiO.P = dvector(0, NPRESSURE-1);
    opacTiO.Plog10 = dvector(0, NPRESSURE-1);
    opacTiO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacTiO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacTiO.abundance[j] = chem.TiO[j];
    }
    ReadOpacTable(opacTiO, fileArray[29]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacTiO.kappa[i][j][k];
    
    printf("Read TiO Opacity done\n");

    FreeOpacTable(opacTiO);
  }
  
  /* Fill in VO opacities */
  if(chemSelection[27] == 1){
    opacVO.name = "VO";
    opacVO.T = dvector(0, NTEMP-1);
    opacVO.P = dvector(0, NPRESSURE-1);
    opacVO.Plog10 = dvector(0, NPRESSURE-1);
    opacVO.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacVO.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
	//opacVO.abundance[j] = chem.VO[j];
    }
    ReadOpacTable(opacVO, fileArray[30]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
	  opac.kappa[i][j][k] += opacVO.kappa[i][j][k];
    
    printf("Read VO Opacity done\n");
    
    FreeOpacTable(opacVO);
  }

  /* Atomic opacities */

  /* Fill in Na opacities */
  if(chemSelection[28] == 1){
    opacNa.name = "Na";
    opacNa.T = dvector(0, NTEMP-1);
    opacNa.P = dvector(0, NPRESSURE-1);
    opacNa.Plog10 = dvector(0, NPRESSURE-1);
    opacNa.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacNa.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
  	//opacNa.abundance[j] = chem.Na[j];
    }
    ReadOpacTable(opacNa, fileArray[31]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
  	  opac.kappa[i][j][k] += opacNa.kappa[i][j][k];
    
    printf("Read Na Opacity done\n");
    
    FreeOpacTable(opacNa);
  }

  /* Fill in K opacities */
  if(chemSelection[29] == 1){
    opacK.name = "K";
    opacK.T = dvector(0, NTEMP-1);
    opacK.P = dvector(0, NPRESSURE-1);
    opacK.Plog10 = dvector(0, NPRESSURE-1);
    opacK.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacK.abundance = dvector(0, NTAU-1);
    for(j=0; j<NPRESSURE; j++){
  	//opacK.abundance[j] = chem.K[j];
    }
    ReadOpacTable(opacK, fileArray[32]);
    
    //Populate opac.kappa
    for (i=0; i<NLAMBDA; i++)
      for (j=0; j<NPRESSURE; j++)
          for (k=0; k<NTEMP; k++)
  	  opac.kappa[i][j][k] += opacK.kappa[i][j][k];
    
    printf("Read K Opacity done\n");
    
    FreeOpacTable(opacK);
  }
  
  /* Fill in total opacities */
  
  for(k=0; k<NTEMP; k++)
    opac.T[k] = chem.T[k];	      //insert temperatures
  
  for(j=0; j<NPRESSURE; j++){	      //insert pressues
    opac.P[j] = chem.P[j];
    opac.Plog10[j] = log10(chem.P[j]);
  }
  
  /* Fill in collision-induced opacities */
  if (chemSelection[31] == 1) {
    /* Allocate collison induced opacities */
    opacCIA.name = "CIA";
    opacCIA.T = dvector(0, NTEMP-1);
    opacCIA.P = dvector(0, NPRESSURE-1);
    opacCIA.Plog10 = dvector(0, NPRESSURE-1);
    opacCIA.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacCIA.abundance = dvector(0, NTAU-1);
    
    ///* populate with zeros */	
    //for (i=0; i<NLAMBDA; i++) {
    //  for (j=0; j<NTAU; j++) {
    //        for (k=0; k<NTEMP; k++) {
    //            opacCIA.kappa[i][j][k] = 0.;
    //        };
    //  };
    //};
    
    /* Read in CIA opacities */
    
    f1 = fopen(fileArray[33], "r");
    if(f1 == NULL){
      printf("\n totalopac.c:\nError opening file: %s -- No such file or"
              "directory\n\n", fileArray[33]);
      exit(1);
    }
    
    for(i=0; i<NTEMP; i++){
      fscanf(f1, "%le", &opacCIA.T[i]);
      printf("%1e\n", opacCIA.T[i]);
    }

    double dum;

    for (k=0; k<NTEMP; k++){
      fscanf(f1, "%le", &opacCIA.T[k]);
      for (i=0; i<NLAMBDA; i++){
        fscanf(f1, "%le %le %le %le %le %le %le %le %le %le %le %le %le %le %le",
                &dum, &opac_CIA_H2H2[k][i],
                &opac_CIA_H2He[k][i], &opac_CIA_H2H[k][i],
                &opac_CIA_H2CH4[k][i], &opac_CIA_CH4Ar[k][i],
                &opac_CIA_CH4CH4[k][i], &opac_CIA_CO2CO2[k][i],
                &opac_CIA_HeH[k][i], &opac_CIA_N2CH4[k][i],
                &opac_CIA_N2H2[k][i], &opac_CIA_N2N2[k][i],
                &opac_CIA_O2CO2[k][i], &opac_CIA_O2N2[k][i],
                &opac_CIA_O2O2[k][i]);
      }
    };

    for (i=0; i<NLAMBDA; i++){
      for (j=0; j<NTAU; j++){
          Locate(NTEMP, opacCIA.T, atmos.T[j], &a);
          Locate(NPRESSURE, opacCIA.P, atmos.P[j], &b);

          atmos.kappa_nu[i][j] += opac_CIA_H2H2[a][i] * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_H2He[a][i] * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.He[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_H2H[a][i] * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.H[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_H2CH4[a][i] * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.CH4[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          /* atmos.kappa_nu[i][j] += opac_CIA_CH4Ar[a][i] *  */
          /*   chem.CH4[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) *  */
          /*   chem.Ar[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]); */
          atmos.kappa_nu[i][j] += opac_CIA_CH4CH4[a][i] * 
            chem.CH4[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.CH4[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_CO2CO2[a][i] * 
            chem.CO2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.CO2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_HeH[a][i] * 
            chem.He[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.H[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_N2CH4[a][i] * 
            chem.N2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.CH4[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_N2H2[a][i] * 
            chem.N2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.H2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_N2N2[a][i] * 
            chem.N2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.N2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_O2CO2[a][i] * 
            chem.O2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.CO2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_O2N2[a][i] * 
            chem.O2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.N2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
          atmos.kappa_nu[i][j] += opac_CIA_O2O2[a][i] * 
            chem.O2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]) * 
            chem.O2[j] * atmos.P[j] / (KBOLTZMANN * opac.T[a]);
      }
    }
    
    

    ///* Populate atmos.kappa_nu */
    //for (i=0; i<NLAMBDA; i++){
    //  for (j=0; j<NTAU; j++) {
    //      /* Interpolate from TP grid onto the altitude grid */
    //      Locate(NTEMP, opacCIA.T, atmos.T[j], &a);
    //      Locate(NPRESSURE, opacCIA.P, atmos.P[j], &b);


    //      kappa_nu[i][j] = lint2D(opac.T[a], opac.T[a+1], opac.P[b], opac.P[b+1],
    //  			      opac.kappa[i][b][a], opac.kappa[i][b][a+1],
    //  			      opac.kappa[i][b+1][a], opac.kappa[i][b+1][a+1],
    //  			      atmos.T[j], atmos.P[j]);

    //      if (j == 0) {
    //        printf("i = %d, j = %d, a = %d, b = %d, T = %e, P = %e, MR = %e\n",
    //              i, j, a, b, opacCIA.T[a], opacCIA.P[b], chem.K[j]);
    //      };

    //      /* Add to overall opac.kappa */
    //      atmos.kappa_nu[i][j] += opacCIA.kappa[i][a][b];
    //  }
    //};
    
    FreeOpacTable(opacCIA);
  }
 
  /* Rayleigh scattering */
  
  /* (Polarizabilities from the CRC Handbook) */
  
  if(chemSelection[30] == 1){		// If Scattering is activated...
    
    /* Declare memory for opacscat structure */
    
    opacscat.name = "Scat";
    opacscat.T = dvector(0, NTEMP-1);
    opacscat.P = dvector(0, NPRESSURE-1);
    opacscat.Plog10 = dvector(0, NPRESSURE-1);
    opacscat.kappa = d3tensor(0, NLAMBDA-1, 0, NPRESSURE-1, 0, NTEMP-1);
    //opacscat.abundance = dvector(0, NTAU-1);
    
    //populate with zeros	
    for (i=0; i<NLAMBDA; i++)
        for (j=0; j<NTAU; j++)
            kappa_nu[i][j] = 0.;
    
    /* Fill in scattering coefficients */
    for (i=0; i<NLAMBDA; i++) {
      for (j=0; j<NTAU; j++) {
	  /* Add Rayleigh scattering polarizability to overall kappa */
	  kappa_nu[i][j] +=
	    (8.0*PI/3.0) * SQ(0.80e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.H2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(0.21e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.He[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(1.74e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.N2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(1.45e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.H2O[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(1.95e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.CO[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(2.91e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.CO2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(2.26e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.NH3[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(2.59e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.CH4[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(1.58e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.O2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(3.21e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.O3[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(3.33e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.C2H2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(4.25e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.C2H4[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(4.47e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.C2H6[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(2.59e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.HCN[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(2.63e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.HCl[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(0.80e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.HF[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(3.78e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.H2S[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(1.70e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.NO[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(3.02e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.NO2[j]*chem.P[j] / (KBOLTZMANN * chem.T[j])
	    +
	    (8.0*PI/3.0) * SQ(4.84e-30) *
	    SQ(2.0*PI/ atmos.lambda[i]) * SQ(2.0*PI/ atmos.lambda[i]) *
	    chem.PH3[j]*chem.P[j] / (KBOLTZMANN * chem.T[j]);
	  
	  kappa_nu[i][j] *= RAYLEIGH;
	  atmos.kappa_nu[i][j] += kappa_nu[i][j];
      }
    }
    
    FreeOpacTable(opacscat);
  }

  /*  Free memory */

  free_dmatrix(opac_CIA_H2H2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_H2He, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_H2H, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_H2CH4, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_CH4Ar, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_CH4CH4, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_CO2CO2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_HeH, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_N2CH4, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_N2H2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_N2N2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_O2CO2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_O2N2, 0, NTEMP-1, 0, NLAMBDA-1);
  free_dmatrix(opac_CIA_O2O2, 0, NTEMP-1, 0, NLAMBDA-1);
  
}

/* ------- end -------------- TotalOpac.c ------------------------ */

