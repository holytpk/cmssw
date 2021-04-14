import os

# ttH
eftsamples = {
  'ttH' : '/store/user/kmohrman/FullProduction/Round6/Batch7/postLHE_step/v2/mAOD_step_ttHJet_*',

  'ttlnu_0' : '/store/user/kmohrman/FullProduction/Round6/Batch1/postLHE_step/v1/mAOD_step_ttlnuJet_*',
  'ttlnu_1' : '/store/user/kmohrman/FullProduction/Round6/Batch2/postLHE_step/v1/mAOD_step_ttlnuJet_*',
  'ttlnu_2' : '/store/user/kmohrman/FullProduction/Round6/Batch5/postLHE_step/v1/mAOD_step_ttlnuJet_*',
  'ttlnu_3' : '/store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttlnuJet_*',

  'tHq'   : '/store/user/kmohrman/FullProduction/Round6/Batch3/postLHE_step/v1/mAOD_step_tHq4f_*',

  'ttll_0' : '/store/user/kmohrman/FullProduction/Round6/Batch1/postLHE_step/v1/mAOD_step_ttllNuNuJetNoHiggs_*',
  'ttll_1' : '/store/user/kmohrman/FullProduction/Round6/Batch2/postLHE_step/v1/mAOD_step_ttllNuNuJetNoHiggs_*',
  'ttll_2' : '/store/user/kmohrman/FullProduction/Round6/Batch5/postLHE_step/v1/mAOD_step_ttllNuNuJetNoHiggs_*',
  'ttll_3' : '/store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_ttllNuNuJetNoHiggs_*',

  'tllq_0' : '/store/user/awightma/postLHE_step/2019_04_19/tllq4f-tch-NoHiggs_0partons_xqcut0/v2/mAOD_step_tllq4fNoSchanWNoHiggs0p_*',
  'tllq_1' : '/store/user/awightma/postLHE_step/2019_04_19/tllq4f-tch-NoHiggs_0partons_xqcut0_extra/v2/mAOD_step_tllq4fNoSchanWNoHiggs0p_*',
  'tllq_2' : '/store/user/awightma/postLHE_step/2019_04_19/tllq4f-tch-NoHiggs_0partons_xqcut0_extra2/v1/mAOD_step_tllq4fNoSchanWNoHiggs0p_*',
  'tllq_3' : '/store/user/kmohrman/FullProduction/Round6/Batch4/postLHE_step/v1/mAOD_step_tllq4fNoSchanWNoHiggs0p_*',
  'tllq_4' : '/store/user/kmohrman/FullProduction/Round6/Batch6/postLHE_step/v2/mAOD_step_tllq4fNoSchanWNoHiggs0p_*',
}

gfalprefixnd = 'gfal-ls gsiftp://deepthought.crc.nd.edu'
rootprefixnd = 'root://deepthought.crc.nd.edu/'
outdir = 'eftfiles/'
if not os.path.isdir(outdir): os.mkdir(outdir)

def ProduceSample(path, outname='temp'):
  if   path == None: return
  elif isinstance(path, dict):
    for k in path: ProduceSample(path[k], k)
    return
  if not outname.endswith('.txt'): outname+='.txt'
  if not path.endswith('/'): path += '/'
  print('Creating file: ', outname)
  f = open(outdir+outname, 'w')
  command = gfalprefixnd + path
  out = os.popen(command).read().split('\n')
  samples = []
  for s in out:
    if not s.endswith('.root'): continue
    samples.append(s)
    f.write(rootprefixnd+path+s+"\n")
  f.close()

def CheckDir(name):
  path = eftsamples[name]
  if not path.endswith('/'):
    p1 = path[:path.rfind('/')+1]
    p2 = path[path.rfind('/')+1:]
  command = gfalprefixnd+p1
  if p2.endswith('*'): p2 = p2[:-1]
  dirs = os.popen(command).read().split('\n')
  goodDirs = []
  for d in dirs:
    if not d.startswith(p2): continue
    goodDirs.append(d)
  if   len(goodDirs) == 0: 
    print("NOT FOUND: %s"%p2)
    return None
  elif len(goodDirs) == 1: 
    print("OK: %s"%p2)
    ProduceSample({name:p1 + goodDirs[0]})
  else                   : 
    print("WARNING - more than one dir: %s"%p2)
    names = {}
    for d in goodDirs:
      names[name+'_'+d] = p1 + d
    return ProduceSample(names)
  return None

#gfal-ls gsiftp://deepthought.crc.nd.edu/store/user/kmohrman/FullProduction/Round6/Batch1/postLHE_step/v1/
for k in eftsamples: CheckDir(k)
#CheckDir('ttH')
