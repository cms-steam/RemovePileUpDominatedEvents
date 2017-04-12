// -*- C++ -*-
//
// Package:    RemovePileUpDominatedEventsGenV2/RemovePileUpDominatedEventsGenV2
// Class:      RemovePileUpDominatedEventsGenV2
// 
/**\class RemovePileUpDominatedEventsGenV2 RemovePileUpDominatedEventsGenV2.cc RemovePileUpDominatedEventsGenV2/RemovePileUpDominatedEventsGenV2/plugins/RemovePileUpDominatedEventsGenV2.cc

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
#include <iostream>
#include <fstream>
#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>
#include <DataFormats/JetReco/interface/GenJet.h>
#include <DataFormats/Provenance/interface/EventID.h>

std::string convertInt(int number)
{
    using namespace std;
    if (number == 0)
        return "0";
    string temp="";
    string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (unsigned int i=0;i<temp.length();i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}

class RemovePileUpDominatedEventsGenV2 : public edm::EDFilter {
   public:
      explicit RemovePileUpDominatedEventsGenV2(const edm::ParameterSet&);
      ~RemovePileUpDominatedEventsGenV2();
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
      void LoadFile(std::string newFileName);
      float getGenPt(int eventNumber);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryInfos_;
      const edm::EDGetTokenT<std::vector<reco::GenJet>  > genJets_;
      std::string fileListFolder_,fileName;
      std::ifstream theFile;
      std::map<int,float> genPtMap;
      int countPUTotal;
      int countPUMissing;
      unsigned int bunchCrossing;
      
};

RemovePileUpDominatedEventsGenV2::RemovePileUpDominatedEventsGenV2(const edm::ParameterSet& iConfig):
pileupSummaryInfos_(    consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileupSummaryInfos"))    ),
genJets_(         consumes<std::vector<reco::GenJet> >(iConfig.getParameter<edm::InputTag>("genJets"))                     ),
fileListFolder_(         iConfig.getParameter< std::string  >("fileListFolder")                                                              )
{
    bunchCrossing=0;
    countPUTotal=0;
    countPUMissing=0;
    produces<float>();
}

RemovePileUpDominatedEventsGenV2::~RemovePileUpDominatedEventsGenV2() {
   using namespace std;
   cout<< "RemovePileUpDominatedEventsGenV2: Total genJetPtPU searched: "<<countPUTotal<<endl;
   cout<< "RemovePileUpDominatedEventsGenV2: Missing genJetPtPU (replaced with pt-hat): "<<countPUMissing<<endl;
   cout<< "RemovePileUpDominatedEventsGenV2: Missing genJetPtPU (percentage): "<< 100 * float(countPUMissing)/countPUTotal <<endl;
}

bool RemovePileUpDominatedEventsGenV2::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   using namespace edm;
   using namespace std;
   

//    LoadFile("/scratch/sdonato/genJetPU/CMSSW_7_4_10_patch2/src/files/0.txt");
   
   edm::Handle<std::vector<reco::GenJet> > genJets;
   iEvent.getByToken(genJets_,genJets);

   edm::Handle<std::vector<PileupSummaryInfo> > pileupSummaryInfos;
   iEvent.getByToken(pileupSummaryInfos_,pileupSummaryInfos);
   
   //find in-time pile-up
   if(bunchCrossing>=pileupSummaryInfos.product()->size() || pileupSummaryInfos.product()->at(bunchCrossing).getBunchCrossing()!=0)
    {
       bool found=false;
       for(bunchCrossing=0; bunchCrossing<pileupSummaryInfos.product()->size() && !found; ++bunchCrossing)
       {
            if( pileupSummaryInfos.product()->at(bunchCrossing).getBunchCrossing() == 0 ){
                found=true;
                bunchCrossing--;
            }
       }
       if(!found){
            edm::LogInfo("RemovePileUpDominatedEventsGenV2") << "In-time pile-up not found!" << endl;
            return true;
       }
   }
   
   //get the PU gen-pt max
   float signal_genJetPt = -1;
   float pu_genJetPt_max = -1;
   
   PileupSummaryInfo puSummary_onTime = pileupSummaryInfos.product()->at(bunchCrossing);
//   for(const auto& PUevent : puSummary_onTime.getPU_EventID()){
   for(int i=0; i<puSummary_onTime.getPU_NumInteractions(); i++ ){
        int PUeventNumber = puSummary_onTime.getPU_EventID().at(i).event();
        float PUptHat = puSummary_onTime.getPU_pT_hats().at(i);
        float pu_genJetPt = getGenPt(PUeventNumber%1000000);
        if (pu_genJetPt<0) pu_genJetPt = PUptHat; //use pt-hat as fallback solution
        if(pu_genJetPt>pu_genJetPt_max) pu_genJetPt_max = pu_genJetPt;
   } 
   //get the signal genJetPt
   if(genJets.product()->size()>0) signal_genJetPt = genJets.product()->at(0).pt();

   //save PU - signal genJetPt
   auto pOut = std::make_unique<float>();
   *pOut=signal_genJetPt-pu_genJetPt_max;   
   iEvent.put(std::move(pOut));

   //filter the event
   if (signal_genJetPt>pu_genJetPt_max) return true;
   return false;
}

void RemovePileUpDominatedEventsGenV2::LoadFile(std::string newFileName) {
   using namespace std;
   if(newFileName!=fileName){
        genPtMap.clear();
        fileName = newFileName;
        theFile.close();
        cout << "Opening " << fileName << endl; 
        theFile.open(fileName);
        string line;
        string delimiter = "\t";
        if (theFile.is_open()){
            while ( getline (theFile,line) )
            {
                string eventTxt = line.substr(0, line.find(delimiter));
                string genPtTxt = line.substr(line.find(delimiter), line.size());
                genPtMap.insert( pair<int,float>(atoi(eventTxt.c_str()),atof(genPtTxt.c_str()) ) );
            }
        } else {
            edm::LogWarning("NoFile") << "File " << fileName.c_str() <<" not found. Events will be rejected!!\n";
        }
    }
    return;
}

float RemovePileUpDominatedEventsGenV2::getGenPt(int eventNumber) {
   using namespace std;
   countPUTotal++;
   string newFileName = fileListFolder_+"/"+convertInt(eventNumber/1000000)+".txt"; // it depends on eventNumber
   LoadFile(newFileName);
   if(genPtMap.count(eventNumber)>0){
        return genPtMap[eventNumber];
   } else {
//        edm::LogWarning("NoEventNumeber") << "EventNumber " << eventNumber <<" not found in file "<< fileName.c_str() << ". getGenPtPU is set to 0!!\n";            
        countPUMissing++;
        return -1;
   }
}

      
      
void RemovePileUpDominatedEventsGenV2::beginJob(){
}
void RemovePileUpDominatedEventsGenV2::endJob() {}

void RemovePileUpDominatedEventsGenV2::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag> ("pileupSummaryInfos",edm::InputTag("addPileupInfo"));
//  desc.add<edm::InputTag> ("genJets",edm::InputTag("ak4GenJetsNoNu"));
  desc.add<edm::InputTag> ("genJets",edm::InputTag("ak4GenJets"));
  desc.add<std::string> ("fileListFolder",std::string("/afs/cern.ch/user/s/sdonato/AFSwork/public/genJetPtHadPU_RunIISummer15GS"));
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemovePileUpDominatedEventsGenV2);
