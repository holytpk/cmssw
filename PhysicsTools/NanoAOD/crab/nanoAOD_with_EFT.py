# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: test94X -s NANO --mc --eventcontent NANOAODSIM --datatier NANOAODSIM --filein /store/mc/RunIIFall17MiniAOD/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/60000/A0D71AEE-13E1-E711-B3C9-FA163E629498.root --no_exec --conditions auto:phase1_2017_realistic -n 1000 --era Run2_2017,run2_nanoAOD_94XMiniAODv1
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('NANO',eras.Run2_2017_ppRef,eras.run2_HI_specific) # Run2_2017, eras.run2_nanoAOD_94XMiniAODv1

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('PhysicsTools.NanoAOD.nano_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(100)
#)

# Input source
#process.inputDataset = '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'

# Input source
#process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring('file:HIG-RunIIFall17MiniAOD-00821ND_24549.root'),
#    secondaryFileNames = cms.untracked.vstring()
#)

process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring())
process.source.fileNames = [
	#'../../NanoAOD/test/lzma.root' ##you can change only this line
]



#process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring('/store/mc/RunIIFall17MiniAOD/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/60000/A0D71AEE-13E1-E711-B3C9-FA163E629498.root'),
#    secondaryFileNames = cms.untracked.vstring()
#)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('nanoAOD by Xuan'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.NANOAODSIMoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    fakeNameForCrab = cms.untracked.bool(True),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('tree.root'),
    outputCommands = process.NANOAODSIMEventContent.outputCommands
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data_relval', '')
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2017_realistic', '')

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.nanoAOD_step,process.endjob_step,process.NANOAODSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeMC 

#call to customisation function nanoAOD_customizeMC imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeMC(process)

# End of customisation functions

# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
