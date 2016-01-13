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
      edm::EDGetTokenT<std::vector<l1extra::L1JetParticle> > l1centraljetsToken_, l1forwardjetsToken_,l1tausToken_;
      edm::EDGetTokenT<std::vector<l1extra::L1MuonParticle> > l1muonsToken_;
      double deltaR_,minGenjetPt_;

};

RemovePileUpDominatedEvents::RemovePileUpDominatedEvents(const edm::ParameterSet& iConfig) {
   using namespace std;
   deltaR_ = iConfig.getParameter<double>("deltaR");
   minGenjetPt_ = iConfig.getParameter<double>("minGenjetPt");
   genjetsToken_ = consumes<vector<reco::GenJet> >(iConfig.getParameter<edm::InputTag>("genjets"));
   l1centraljetsToken_ = consumes<vector<l1extra::L1JetParticle> >(iConfig.getParameter<edm::InputTag>("l1centraljets"));
   l1forwardjetsToken_ = consumes<vector<l1extra::L1JetParticle> >(iConfig.getParameter<edm::InputTag>("l1forwardjets"));
   l1tausToken_ = consumes<vector<l1extra::L1JetParticle> >(iConfig.getParameter<edm::InputTag>("l1taujets"));
   l1muonsToken_ = consumes<vector<l1extra::L1MuonParticle> >(iConfig.getParameter<edm::InputTag>("l1muonjets"));

   produces<float>();
}

RemovePileUpDominatedEvents::~RemovePileUpDominatedEvents() {}

bool RemovePileUpDominatedEvents::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   using namespace edm;
   using namespace std;
   
   edm::Handle<vector<reco::GenJet> > genjets;
   iEvent.getByToken(genjetsToken_,genjets);

   edm::Handle<vector<l1extra::L1JetParticle> > l1centraljets;
   iEvent.getByToken(l1centraljetsToken_,l1centraljets);

   edm::Handle<vector<l1extra::L1JetParticle> > l1forwardjets;
   iEvent.getByToken(l1forwardjetsToken_,l1forwardjets);

   edm::Handle<vector<l1extra::L1JetParticle> > l1taus;
   iEvent.getByToken(l1tausToken_,l1taus);

   edm::Handle<vector<l1extra::L1MuonParticle> > l1muons;
   iEvent.getByToken(l1muonsToken_,l1muons);

   float max,tmp, dR = 1000;
   const reco::Candidate* l1leading = NULL;
   
   if( (l1centraljets.product()->size() + l1forwardjets.product()->size() + l1taus.product()->size() + l1muons.product()->size() )==0 ) {
   		edm::LogWarning("NoL1") << "No l1 objects found." << endl;
   		return false;
   }
   else if(genjets.product()->size()==0) {
   		edm::LogWarning("NoGenJets") << "No genjets found." << endl;
   		return false;
   }
   else {
   		float ptFwd=0;
   		float ptCen=0;
   		float pttau=0;
   		float ptmuon=0;

   		if(l1centraljets.product()->size()>0) ptCen = l1centraljets.product()->at(0).pt();
   		if(l1forwardjets.product()->size()>0) ptFwd = l1forwardjets.product()->at(0).pt();
   		if(l1taus.product()->size()>0) pttau = l1taus.product()->at(0).pt();
   		if(l1muons.product()->size()>0) ptmuon = l1muons.product()->at(0).pt();

   		max = std::max(std::max(ptCen,ptFwd),std::max(pttau,ptmuon));

   		if(ptCen==max) l1leading = & l1centraljets.product()->at(0);
   		else if((ptFwd==max)) l1leading = & l1forwardjets.product()->at(0);
   		else if((pttau==max)) l1leading = & l1taus.product()->at(0);
   		else if((ptmuon==max)) l1leading = & l1muons.product()->at(0);
   		else edm::LogError("") << "Something wrong with L1 object pT." << endl;

   		for(vector<reco::GenJet>::const_iterator genjet= genjets->begin(); genjet!= genjets->end(); genjet++)
   		{
   			if(genjet->pt()<minGenjetPt_) continue;
   			tmp = reco::deltaR(l1leading->eta(),l1leading->phi(),genjet->eta(),genjet->phi());
   			if(tmp<dR) dR=tmp;
   		}
   }
   std::auto_ptr<float> pOut(new float());
   *pOut=dR;
   iEvent.put(pOut);
   if (dR<deltaR_) return true;
   return false;
}

void RemovePileUpDominatedEvents::beginJob(){}
void RemovePileUpDominatedEvents::endJob() {}

void RemovePileUpDominatedEvents::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag> ("genjets",edm::InputTag("ak5GenJets"));
  desc.add<edm::InputTag> ("l1centraljets",edm::InputTag("hltL1extraParticles","Central",""));
  desc.add<edm::InputTag> ("l1forwardjets",edm::InputTag("hltL1extraParticles","Forward",""));
  desc.add<edm::InputTag> ("l1taujets",edm::InputTag("hltL1extraParticles","Tau",""));
  desc.add<edm::InputTag> ("l1muonjets",edm::InputTag("hltL1extraParticles","",""));  
  desc.add<double> ("deltaR",0.5);
  desc.add<double> ("minGenjetPt",10);
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemovePileUpDominatedEvents);
