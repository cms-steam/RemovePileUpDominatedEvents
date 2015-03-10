import FWCore.ParameterSet.Config as cms

process = cms.Process("SKIMEVENT")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

process.source = cms.Source("PoolSource",
    fileNames = 
cms.untracked.vstring(
#	"file:/scratch/sdonato/DQM4b/CMSSW_7_2_1_old/src/GEN-SIM-RECO-TTBar-730pre1.root",
#	"file:/gpfs/ddn/srm/cms/store/user/sdonato/QCD_Pt-15to30_Tune4C_13TeV_pythia8/QCD_L1HadronicSkim_Pt15to30/140909_180946/0000/SimL1Emulator_Stage1_PP_1.root",
#	"file:/gpfs/ddn/srm/cms/store/user/sdonato/QCD_Pt-30to50_Tune4C_13TeV_pythia8/QCD_L1HadronicSkim_Pt30to50/140909_181036/0000/SimL1Emulator_Stage1_PP_1.root",
#	"file:/gpfs/ddn/srm/cms/store/user/sdonato/QCD_Pt-50to80_Tune4C_13TeV_pythia8/QCD_L1HadronicSkim_Pt50to80/140909_181053/0000/SimL1Emulator_Stage1_PP_1.root"
	"file:/gpfs/ddn/srm/cms/store/user/sdonato/QCD_Pt-50to80_Tune4C_13TeV_pythia8/QCD_L1METSkim4_Pt50to80/141008_090607/0000/SkimMET_1.root"
),
)

process.test = cms.EDFilter("RemovePileUpDominatedEvents")
process.p = cms.Path( process.test )

process.outp1=cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string('skimmed30ev.root'),
        )
        
        
process.options = cms.untracked.PSet(
 wantSummary = cms.untracked.bool( True )
)
#process.out = cms.EndPath( process.outp1 )
