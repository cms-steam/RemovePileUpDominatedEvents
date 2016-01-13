// -*- C++ -*-
//
// Package:    NtuplerPUSingleEvent/NtuplerPUSingleEvent
// Class:      NtuplerPUSingleEvent
// 
/**\class NtuplerPUSingleEvent NtuplerPUSingleEvent.cc NtuplerPUSingleEvent/NtuplerPUSingleEvent/plugins/NtuplerPUSingleEvent.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  root
//         Created:  Tue, 15 Apr 2014 14:59:49 GMT
//
//


// system include files
#include <memory>
#include <iostream>
//#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/FileBlock.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include <SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h>
#include <SimDataFormats/Vertex/interface/SimVertex.h>
#include "DataFormats/JetReco/interface/GenJet.h"
 
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#define PUMAX       100  

class NtuplerPUSingleEvent : public edm::EDAnalyzer {
   public:
      explicit NtuplerPUSingleEvent(const edm::ParameterSet&);
      ~NtuplerPUSingleEvent();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      virtual void respondToOpenInputFile(edm::FileBlock const &) override;
      
      int nevent,nlumi,nrun,pu;
      float ptHat, z, leadingGenJet;
      std::string fileName;

//      int neventPU[PUMAX],nlumiPU[PUMAX],nrunPU[PUMAX];
//      float ptHatPU[PUMAX], zPU[PUMAX];
      

//getPU_zpositions ()

      const PileupSummaryInfo *puObj;

//      const edm::EDGetTokenT<std::string > fileName_;
//      const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryInfos_;
      const edm::EDGetTokenT<GenEventInfoProduct > generatorInfo_;
      const edm::EDGetTokenT<std::vector<SimVertex> > simvertexs_;
      const edm::EDGetTokenT<std::vector<reco::GenJet> > genjets_;
//      const unsigned int bunchCrossing_;
      
    TTree *tree;
    edm::Service<TFileService> file;
//    const char* fileName;

};

NtuplerPUSingleEvent::NtuplerPUSingleEvent(const edm::ParameterSet& iConfig):
//fileName_(    consumes<string>(iConfig.getParameter<std::string>("fileName"))    ),
//pileupSummaryInfos_(    consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileupSummaryInfos"))    ),
generatorInfo_(         consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generatorInfo"))                     ),
simvertexs_(    consumes<std::vector<SimVertex> >(iConfig.getParameter<edm::InputTag>("simvertex"))    ),
genjets_(    consumes<std::vector<reco::GenJet> >(iConfig.getParameter<edm::InputTag>("genJets"))    )
//bunchCrossing_(         iConfig.getParameter<int>("bunchCrossing")                                                              )
{
    tree=file->make<TTree>("tree","tree");
    tree->Branch("nevent",&nevent,"nevent/I");
    tree->Branch("nlumi",&nlumi,"nlumi/I");
    tree->Branch("nrun",&nrun,"nrun/I");
    tree->Branch("ptHat",&ptHat,"ptHat/F");
    tree->Branch("leadingGenJet",&leadingGenJet,"leadingGenJet/F");
    tree->Branch("z",&z,"z/F");
    tree->Branch("pu",&pu,"pu/I");

//    tree->Branch("neventPU",neventPU,"neventPU[pu]/I");
//    tree->Branch("nlumiPU",nlumiPU,"nlumiPU[pu]/I");
//    tree->Branch("nrunPU",nrunPU,"nrunPU[pu]/I");
//    tree->Branch("ptHatPU",ptHatPU,"ptHatPU[pu]/F");
//    tree->Branch("zPU",zPU,"zPU[pu]/F");

    tree->Branch("fileName",&(fileName));
    fileName="";
}


NtuplerPUSingleEvent::~NtuplerPUSingleEvent()
{ 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}

void NtuplerPUSingleEvent::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    nevent=iEvent.id().event();
    nlumi=iEvent.id().luminosityBlock();
    nrun=iEvent.id().run();

    z=0;
    edm::Handle<std::vector<SimVertex> > simvtx;
    iEvent.getByToken(simvertexs_,simvtx);
    if(simvtx.isValid() && simvtx->size()>0) z=simvtx->begin()->position().z();

//    recoGenJets_ak4GenJets__SIM.
    leadingGenJet = 0;
    edm::Handle<std::vector<reco::GenJet> > genjets;
    iEvent.getByToken(genjets_,genjets);
    if(genjets.isValid() && genjets->size()>0) leadingGenJet=genjets->begin()->pt();

    ptHat=-1;
    edm::Handle<GenEventInfoProduct > generatorInfo;
    iEvent.getByToken(generatorInfo_,generatorInfo);
    if(generatorInfo.isValid()) ptHat=generatorInfo->qScale();

//    edm::Handle < std::vector < PileupSummaryInfo  > > puHandle;
//    iEvent.getByToken(pileupSummaryInfos_, puHandle);
//    puObj = &((*puHandle).at(bunchCrossing_));
//    if(puObj->getBunchCrossing()!=0) std::cout<< "Bunch crossing is not 0!" << endl;

//    pu = puObj->getPU_NumInteractions();
//    int count=0;
//    for(const auto& event :puObj->getPU_EventID())
//    {
//        neventPU[count]=event.event();
//        nlumiPU[count]=event.luminosityBlock();
//        nrunPU[count]=event.run();
//        count++;
//    }
//    if(count!=pu) std::cout<< "Something is wrong!! PU=" << pu << " and count=" << count << endl;

//    count=0;
//    for(const auto& ptHat :puObj->getPU_pT_hats())
//    {
//        ptHatPU[count]=ptHat;
//        count++;
//    }
//    if(count!=pu) std::cout<< "Something is wrong (ptHatPU)! PU=" << pu << " and count=" << count << endl;

//    count=0;
//    for(const auto& z :puObj->getPU_zpositions())
//    {
//        zPU[count]=z;
//        count++;
//    }
//    if(count!=pu) std::cout<< "Something is wrong (zPU)! PU=" << pu << " and count=" << count << endl;
    
    tree->Fill();

    return;
}


// ------------ method called once each job just before starting event loop  ------------
void 
NtuplerPUSingleEvent::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
NtuplerPUSingleEvent::endJob() 
{
}


void NtuplerPUSingleEvent::respondToOpenInputFile(edm::FileBlock const & file){
//    fileName=file.fileName().c_str();
    fileName=file.fileName();
   return;
}


// ------------ method called when starting to processes a run  ------------
/*
void 
NtuplerPUSingleEvent::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
NtuplerPUSingleEvent::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
NtuplerPUSingleEvent::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
NtuplerPUSingleEvent::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
NtuplerPUSingleEvent::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
//  desc.add<edm::InputTag> ("fileName",string(""));
  desc.add<edm::InputTag> ("pileupSummaryInfos",edm::InputTag("addPileupInfo"));
  desc.add<edm::InputTag> ("simvertex",edm::InputTag("g4SimHits"));
  desc.add<edm::InputTag> ("generatorInfo",edm::InputTag("generator"));
  desc.add<edm::InputTag> ("genJets",edm::InputTag("ak4GenJets"));
  desc.add<int> ("bunchCrossing",12); //12 correspond to in-time pile-up
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerPUSingleEvent);
