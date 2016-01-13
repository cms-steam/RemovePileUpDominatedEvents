// -*- C++ -*-
//
// Package:    RemoveRepeatedPileUpEvents/RemoveRepeatedPileUpEvents
// Class:      RemoveRepeatedPileUpEvents
// 
/**\class RemoveRepeatedPileUpEvents RemoveRepeatedPileUpEvents.cc RemoveRepeatedPileUpEvents/RemoveRepeatedPileUpEvents/plugins/RemoveRepeatedPileUpEvents.cc

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
#include <string>
#include <cstdlib>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/FileBlock.h"

#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>
#include <SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h>

#include "FWCore/Framework/interface/MakerMacros.h"

#include "TString.h"

#include <iostream>
#include <fstream>

class RemoveRepeatedPileUpEvents : public edm::EDFilter {
   public:
      explicit RemoveRepeatedPileUpEvents(const edm::ParameterSet&);
      ~RemoveRepeatedPileUpEvents();
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      virtual void respondToOpenInputFile(edm::FileBlock const &) override;
      
      std::string fileListFolder_,fileName;
      std::map<unsigned int,bool> goodEvents;
};

RemoveRepeatedPileUpEvents::RemoveRepeatedPileUpEvents(const edm::ParameterSet& iConfig):
fileListFolder_(         iConfig.getParameter< std::string  >("fileListFolder")                                                              )
{
}

RemoveRepeatedPileUpEvents::~RemoveRepeatedPileUpEvents() {}

bool RemoveRepeatedPileUpEvents::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   if(goodEvents[iEvent.id().event()]) return true;
   else return false;
}

void RemoveRepeatedPileUpEvents::beginJob(){}
void RemoveRepeatedPileUpEvents::endJob() {}

void RemoveRepeatedPileUpEvents::respondToOpenInputFile(edm::FileBlock const & file){
    goodEvents.erase(goodEvents.begin(),goodEvents.end());
    bool ok;
    fileName=file.fileName();
    do{
        ok=true;
        if(fileName.find("/")<fileName.size()) {fileName[fileName.find("/")]='_'; ok=false;}
    }while(!ok);
    fileName = fileName.substr(fileName.find("store_mc"));
    fileName = fileName.substr(0,fileName.size()-5);
    fileName.append(".txt");
    fileName.insert(0,fileListFolder_);

    std::string line;
    std::ifstream theFile(fileName.c_str());
    if (theFile.is_open()){
        while ( getline (theFile,line) )
        {
          goodEvents[std::atoi(line.c_str())]=true;
        }
    } else {
        edm::LogWarning("NoFile") << "File " << fileName.c_str() <<" not found. Events will be rejected!!\n";
    }
    theFile.close();
    
    return;
}
void RemoveRepeatedPileUpEvents::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string> ("fileListFolder",std::string("/afs/cern.ch/user/s/sdonato/AFSwork/public/RepeatedPUFilter/makeGoodEventList/files/"));
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemoveRepeatedPileUpEvents);
