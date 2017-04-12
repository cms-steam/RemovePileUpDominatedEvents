// -*- C++ -*-
//
// Package:    RemovePileUpDominatedEvents/RemovePileUpDominatedEvents
// Class:      RemovePileUpDominatedEvents
// 
/**\class RemovePileUpDominatedEvents RemovePileUpDominatedEvents.cc RemovePileUpDominatedEvents/RemovePileUpDominatedEvents/plugins/RemovePileUpDominatedEvents.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Silvio DONATO
//         Created:  Fri, 12 Dec 2014 12:48:57 GMT
//
//

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/L1Candidate.h"

class RemovePileUpDominatedEvents : public edm::EDFilter {
   public:
      explicit RemovePileUpDominatedEvents(const edm::ParameterSet&);
      ~RemovePileUpDominatedEvents();
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      edm::EDGetTokenT<std::vector<reco::GenJet> > genjetsToken_;
      edm::EDGetTokenT<BXVector<l1t::Jet> > l1jetsToken_;
      edm::EDGetTokenT<BXVector<l1t::Tau> > l1tausToken_;
      edm::EDGetTokenT<BXVector<l1t::EGamma> > l1egsToken_;
      edm::EDGetTokenT<BXVector<l1t::Muon> > l1muonsToken_;

      double deltaR_,minGenjetPt_;
};

RemovePileUpDominatedEvents::RemovePileUpDominatedEvents(const edm::ParameterSet& iConfig) {
   using namespace std;
   deltaR_ = iConfig.getParameter<double>("deltaR");
   minGenjetPt_ = iConfig.getParameter<double>("minGenjetPt");
   genjetsToken_ = consumes<vector<reco::GenJet> >(iConfig.getParameter<edm::InputTag>("genjets"));
   l1jetsToken_ = consumes<BXVector<l1t::Jet> >(iConfig.getParameter<edm::InputTag>("l1jets"));
   l1egsToken_ = consumes<BXVector<l1t::EGamma> >(iConfig.getParameter<edm::InputTag>("l1egs"));
   l1tausToken_ = consumes<BXVector<l1t::Tau> >(iConfig.getParameter<edm::InputTag>("l1taujets"));
   l1muonsToken_ = consumes<BXVector<l1t::Muon> >(iConfig.getParameter<edm::InputTag>("l1muonjets"));

   produces<float>();
}

RemovePileUpDominatedEvents::~RemovePileUpDominatedEvents() {}

bool RemovePileUpDominatedEvents::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   using namespace edm;
   using namespace std;
   
   edm::Handle<vector<reco::GenJet> > genjets;
   iEvent.getByToken(genjetsToken_,genjets);

   edm::Handle<BXVector<l1t::Jet> > l1jets;
   iEvent.getByToken(l1jetsToken_,l1jets);

   edm::Handle<BXVector<l1t::EGamma> > l1egs;
   iEvent.getByToken(l1egsToken_,l1egs);

   edm::Handle<BXVector<l1t::Tau> > l1taus;
   iEvent.getByToken(l1tausToken_,l1taus);

   edm::Handle<BXVector<l1t::Muon> > l1muons;
   iEvent.getByToken(l1muonsToken_,l1muons);

   float tmp, dR = 1000;
   const l1t::L1Candidate* l1leading = NULL;
   
   if( (l1jets.product()->size(0) + l1egs.product()->size(0)  + l1taus.product()->size(0)  + l1muons.product()->size(0) )==0 ) {
   		edm::LogWarning("NoL1") << "No l1 objects found." << endl;
   		return false;
   }
   else if(genjets.product()->size()==0) {
   		edm::LogWarning("NoGenJets") << "No genjets found." << endl;
   		return false;
   }
   else {
   		float l1objMax=-1;
   		float pt;
       		for(auto l1obj = l1jets.product()->begin(0); l1obj != l1jets.product()->end(0); l1obj++){
       		    pt = l1obj->pt();
       		    if(pt>l1objMax){
       		        l1objMax = pt;
       		        l1leading = &(*l1obj);
       		    }
       		}
       		for(auto l1obj = l1egs.product()->begin(0); l1obj != l1egs.product()->end(0); l1obj++){
       		    pt = l1obj->pt();
       		    if(pt>l1objMax){
       		        l1objMax = pt;
       		        l1leading = &(*l1obj);
       		    }
       		}
       		for(auto l1obj = l1taus.product()->begin(0); l1obj != l1taus.product()->end(0); l1obj++){
       		    pt = l1obj->pt();
       		    if(pt>l1objMax){
       		        l1objMax = pt;
       		        l1leading = &(*l1obj);
       		    }
       		}
       		for(auto l1obj = l1muons.product()->begin(0); l1obj != l1muons.product()->end(0); l1obj++){
       		    pt = l1obj->pt();
       		    if(pt>l1objMax){
       		        l1objMax = pt;
       		        l1leading = &(*l1obj);
       		    }
       		}
   		for(vector<reco::GenJet>::const_iterator genjet= genjets->begin(); genjet!= genjets->end(); genjet++)
   		{
   			if(genjet->pt()<minGenjetPt_) continue;
   			tmp = reco::deltaR(l1leading->eta(),l1leading->phi(),genjet->eta(),genjet->phi());
   			if(tmp<dR) dR=tmp;
   		}
   
   auto pOut = std::make_unique<float>();
   *pOut=dR;
   iEvent.put(std::move(pOut));
   }
   if (dR<deltaR_) return true;
   return false;
}

void RemovePileUpDominatedEvents::beginJob(){}
void RemovePileUpDominatedEvents::endJob() {}

void RemovePileUpDominatedEvents::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag> ("genjets"        ,edm::InputTag("ak4GenJetsNoNu"));
  desc.add<edm::InputTag> ("l1jets"         ,edm::InputTag("hltGtStage2Digis","Jet"));
  desc.add<edm::InputTag> ("l1taujets"      ,edm::InputTag("hltGtStage2Digis","Tau"));
  desc.add<edm::InputTag> ("l1muonjets"     ,edm::InputTag("hltGtStage2Digis","Muon"));  
  desc.add<edm::InputTag> ("l1egs"          ,edm::InputTag("hltGtStage2Digis","EGamma"));  
  desc.add<double> ("deltaR",0.4);
  desc.add<double> ("minGenjetPt",10);
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemovePileUpDominatedEvents);

//BXVector<l1t::EGamma>                 "caloStage2Digis"           "EGamma"          "HLT2"    
//BXVector<l1t::EtSum>                  "caloStage2Digis"           "EtSum"           "HLT2"    
//BXVector<l1t::Jet>                    "caloStage2Digis"           "Jet"             "HLT2"    
//BXVector<l1t::Muon>                   "gmtStage2Digis"            "Muon"            "HLT2"    
//BXVector<l1t::Tau>                    "caloStage2Digis"           "Tau"             "HLT2"    

