import FWCore.ParameterSet.Config as cms

process = cms.Process("NtuplePU")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
    SkipEvent = cms.untracked.vstring('FileReadError'),
)

process.source = cms.Source("PoolSource",
#    skipEvents =  cms.untracked.uint32(1752),
    fileNames = 
cms.untracked.vstring(
#"root://xrootd.ba.infn.it//store/mc/RunIIFall14GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v3/00000/0259A65C-4A71-E411-9469-0025905B860E.root",
"file:MinBias_GEN_SIM.root"
),

)

process.TFileService = cms.Service("TFileService", fileName = cms.string("ntuplePU.root") )
process.tree = cms.EDAnalyzer("NtuplerPUSingleEvent")
process.path = cms.Path( process.tree )
#process.outp1=cms.OutputModule("PoolOutputModule",
#        fileName = cms.untracked.string('skimmed30ev.root'),
#        )
#process.out = cms.EndPath( process.outp1 )
