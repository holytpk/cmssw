from ROOT import *
samples = [
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_24549.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_26688.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_30946.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_32399.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_33560.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_35098.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_37739.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_39302.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_40299.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_42581.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_43587.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_44513.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_45508.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_46545.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_47643.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_48384.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_49123.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_50510.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_51285.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_52141.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_52702.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_53616.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_54168.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_54861.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_56908.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_57104.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_57466.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_58588.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_58969.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_60105.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_60620.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_62038.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_62333.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_63605.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_63973.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_64430.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_64592.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_65026.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_66297.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_66506.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_66963.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_67429.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_67922.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_67923.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_67924.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_68432.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_68914.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_69136.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_69400.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_69677.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_70471.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_70770.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_70771.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_71055.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_71342.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_71842.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_71843.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_72134.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_72135.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_72475.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_72765.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_73499.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_73759.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_74051.root',
      'root://deepthought.crc.nd.edu//store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_HanV4ttXJetStartPtChecks_run2/HIG-RunIIFall17MiniAOD-00821ND_74052.root'
]

nEv = 0
for s in samples:
  f = TFile.Open(s)
  print('file = ', s)
  ev = f.Events.GetEntries()
  nEv += ev
  print('         >> nEvents = ', ev)
  f.Close()

print('nEv = ', nEv)
