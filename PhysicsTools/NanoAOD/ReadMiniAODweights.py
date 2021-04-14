#!/bin/env python

import os, re, ROOT, sys, pickle, time
from pprint import pprint
from math import *
from array import array
from DataFormats.FWLite import Events, Handle
import numpy as np

############################################
################## User Input

studies = {
    'ttH':{
        'files':[
            'HIG-RunIIFall17MiniAOD-00821ND_24549.root',
            ],
        'name':'ttbar'
        },
    }

LHEInfo = Handle("<LHEEventProduct>")

n_events_limit = 20

ROOT.gROOT.SetBatch(True)

###############################################
########### Main part
for study,info in studies.items():
    print "Processing %s" % study
    title = study
    files = info['files']

    print "Number of files: %d" % len(files)

    events = Events(files)
    nevents = 0

    # loop over events
    for event in events:
        if n_events_limit and nevents>=n_events_limit: break
        if (nevents+1)%10000==0: print "Event: ",nevents+1

        events.getByLabel("externalLHEProducer", LHEInfo)
        #originalXWGTUP_intree = LHEInfo.originalXWGTUP()

        L = LHEInfo.product()
        print 'len = ', len(L.weights())
        nEFTw = 0
        for w in L.weights():
          if 'EFTrwgt' in str(w.id):
            print w.id, ' : ', w.wgt
            nEFTw+=1
        print 'nEFTw = ', nEFTw
        exit()

        nevents += 1
