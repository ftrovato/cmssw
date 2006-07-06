

#include "DataFormats/L1CaloTrigger/interface/L1CaloRegionDetId.h"

unsigned const L1CaloRegionDetId::N_PHI=18;
unsigned const L1CaloRegionDetId::N_ETA=22;


// default constructor - null DetId
L1CaloRegionDetId::L1CaloRegionDetId() : DetId() { }


// construct from raw id
L1CaloRegionDetId::L1CaloRegionDetId(uint32_t rawid) : DetId(rawid) { }


// construct from ieta, iphi indices
// ieta runs from 0 (at -z) to 21 (at +z)
L1CaloRegionDetId::L1CaloRegionDetId(unsigned ieta, unsigned iphi) :
  DetId(Calo, 2) 
{ 
  id_ |= (ieta & 0x1f) | ((iphi & 0x1f)<<5);
}


// construct from RCT crate, card, region IDs
L1CaloRegionDetId::L1CaloRegionDetId(bool isForward, unsigned icrate, unsigned icard, unsigned irgn) :
  DetId(Calo, 2)
{

  int ieta=0;
  int iphi=0;

  /// TODO - calculate ieta and iphi from RCT crate/card/region #
  id_ |= (ieta & 0x1f) | ((iphi & 0x1f)<<5);
}

// construct from GCT card, region #s
L1CaloRegionDetId::L1CaloRegionDetId(bool isForward, unsigned icard, unsigned irgn) :
  DetId(Calo, 2)
{

  int ieta=0x1f;
  int iphi=0x1f;

  unsigned scType = icard%3 + 1;
  unsigned jfNum  = icard/3;

  if (jfNum<N_PHI) {
    if ((scType == 2) && (irgn<12)) {
      unsigned rctEta = 99;
      unsigned rctPhi = 99;

      if (irgn<2)  { rctEta=irgn+4; rctPhi=1; }     //cardType2: inputs 0-1
      if (irgn==2) { rctEta=6;      rctPhi=0; }     //cardType2: input  2
      if (irgn==3) { rctEta=6;      rctPhi=1; }     //cardType2: input  3
      if (irgn>3)  { rctEta=(irgn%4)+7;
                     rctPhi=(irgn/4)-1; }           //cardType2: inputs 4-11

      ieta = (jfNum < (N_PHI/2)) ? ((N_ETA/2)-1-rctEta) : ((N_ETA/2)+rctEta) ;
      iphi = (2*(N_PHI-jfNum) + 4 - rctPhi) % N_PHI;
    }
    if ((scType == 3) && (irgn<10)) {
      unsigned rctEta = 99;
      unsigned rctPhi = 99;

      if (irgn<6)  { rctEta=irgn;   rctPhi=0; }    //cardType3: inputs 0-5
      else         { rctEta=irgn-6; rctPhi=1; }    //cardType3: inputs 6-11

      ieta = (jfNum < (N_PHI/2)) ? ((N_ETA/2)-1-rctEta) : ((N_ETA/2)+rctEta) ;
      iphi = (2*(N_PHI-jfNum) + 4 - rctPhi) % N_PHI;
    }
  }

  id_ |= (ieta & 0x1f) | ((iphi & 0x1f)<<5);
}

// return RCT crate ID
unsigned L1CaloRegionDetId::rctCrate() const { // TODO - check this is correct!
  unsigned phiCrate = ((N_PHI + 4 - iphi()) % N_PHI) / 2;
  return (ieta()<(N_ETA/2) ? phiCrate : phiCrate + N_PHI/2) ;
}

// return GCT source card number
unsigned L1CaloRegionDetId::gctCard() const
{
  bool forwardEta = ((rctPhi() == 0) ? (rctEta() >= 6) : (rctEta() >= 4)) ;
  return ((rctCrate()*3) + (forwardEta ? 1 : 2));
}

// return GCT region index (within source card)
unsigned L1CaloRegionDetId::gctRegion() const 
{
  unsigned result=99;
  unsigned localEta=rctEta();
  unsigned localPhi=rctPhi();
  if (localPhi==0) {
    if (localEta<6)  { result = localEta; }   //cardType3: inputs 0-5
    if (localEta==6) { result = 2; }          //cardType2: input  2
    if (localEta>6)  { result = localEta-3; } //cardType2: inputs 4-7
  } else {
    if (localEta<4)  { result = localEta+6; } //cardType3: inputs 6-9
    if (localEta==4) { result = 0; }          //cardType2: input  0
    if (localEta==5) { result = 1; }          //cardType2: input  1
    if (localEta==6) { result = 3; }          //cardType2: input  3
    if (localEta>6)  { result = localEta+1; } //cardType2: inputs 8-11
  }
  return result;
}

