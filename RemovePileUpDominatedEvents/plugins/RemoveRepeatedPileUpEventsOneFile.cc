// -*- C++ -*-
//
// Package:    RemoveRepeatedPileUpEventsOneFile/RemoveRepeatedPileUpEventsOneFile
// Class:      RemoveRepeatedPileUpEventsOneFile
// 
/**\class RemoveRepeatedPileUpEventsOneFile RemoveRepeatedPileUpEventsOneFile.cc RemoveRepeatedPileUpEventsOneFile/RemoveRepeatedPileUpEventsOneFile/plugins/RemoveRepeatedPileUpEventsOneFile.cc

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

class RemoveRepeatedPileUpEventsOneFile : public edm::EDFilter {
   public:
      explicit RemoveRepeatedPileUpEventsOneFile(const edm::ParameterSet&);
      ~RemoveRepeatedPileUpEventsOneFile();
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      std::string fileAddress_,fileName;
      std::map<unsigned int,bool> goodEvents;
};

RemoveRepeatedPileUpEventsOneFile::RemoveRepeatedPileUpEventsOneFile(const edm::ParameterSet& iConfig):
fileAddress_(         iConfig.getParameter< std::string  >("fileAddress")                                                              )
{
}

RemoveRepeatedPileUpEventsOneFile::~RemoveRepeatedPileUpEventsOneFile() {}

bool RemoveRepeatedPileUpEventsOneFile::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   if(goodEvents[iEvent.id().event()]) return true;
   else return false;
}

void RemoveRepeatedPileUpEventsOneFile::beginJob(){
    std::string line;
    std::ifstream theFile(fileAddress_.c_str());
    if (theFile.is_open()){
        while ( getline (theFile,line) )
        {
          goodEvents[std::atoi(line.c_str())]=true;
        }
    } else {
        edm::LogError("NoFile") << "File " << fileName.c_str() <<" not found. Events will be rejected!!\n";
    }
    theFile.close();
    
    return;
}
void RemoveRepeatedPileUpEventsOneFile::endJob() {}

void RemoveRepeatedPileUpEventsOneFile::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string> ("fileAddress",std::string("/afs/cern.ch/user/s/sdonato/AFSwork/public/RepeatedPUFilter/makeGoodEventList/files/all.txt"));
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemoveRepeatedPileUpEventsOneFile);
