from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'NtuplePU_v3'
config.General.workArea = 'crab_projects'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'ntuplePU.py'
config.JobType.outputFiles = ['ntuplePU.root']

config.section_("Data")
#config.Data.inputDataset = '/MinBias_TuneCUETP8M1_13TeV-pythia8/RunIIFall14GS-MCRUN2_71_V1-v3/GEN-SIM'
config.Data.inputDataset = '/MinBias_TuneCUETP8M1_13TeV-pythia8/RunIIWinter15GS-MCRUN2_71_V1-v1/GEN-SIM'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.publication = True
config.Data.unitsPerJob = 200
config.Data.totalUnits = -1
config.Data.publishDataName = config.General.requestName
config.section_("Site")
config.Site.storageSite = 'T2_IT_Pisa'
config.Data.ignoreLocality = False #use remote files by xrootd?
