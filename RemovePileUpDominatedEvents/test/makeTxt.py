from ROOT import TChain
#_file = TFile("ntuple.root")
chain = TChain("minBiasNtuple/tree")
#chain.Add("ntuplePU_73.root")
chain.Add("ntuplePU_*.root")
leadingGenJets = {}
chain.SetBranchStatus("*",0)
chain.SetBranchStatus("nevent",1)
chain.SetBranchStatus("leadingGenJet",1)

count=0
for event in chain:
    leadingGenJets[event.nevent] = event.leadingGenJet
    count += 1

nfile = 0
prevfile = -1
file_ = 0
for nevent in sorted(leadingGenJets.keys()):
    nfile = nevent/1000000
    if(nfile != prevfile):
        if file_: file_.close()
        file_=file(str(nfile)+".txt","w+")
        prevfile = nfile
        print "Creating "+str(nfile)+".txt"
    file_.write(str(nevent)+"\t"+str(round(leadingGenJets[nevent],1))+"\n")

