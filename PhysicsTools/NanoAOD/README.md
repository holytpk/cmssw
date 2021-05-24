# Start a new nanoAOD env

    cmsrel CMSSW_10_6_8
    cd CMSSW_10_6_8/src/
    export SCRAM_ARCH=slc7_amd64_gcc700
    cmsenv

    git cms-addpkg PhysicsTools/NanoAOD
    git remote add eftfit https://github.com/GonzalezFJR/cmssw.git
    git fetch eftfit
    git checkout eftfit/ULWCFit

### Or just clone the full repo

    git clone --branch ULWCFit https://github.com/GonzalezFJR/cmssw.git

### Clone the EFTGenReader repo to get some needed scripts

    cd CMSSW_10_6_8/src/
    git clone https://github.com/TopEFT/EFTGenReader

### And install

    scram b -j4

# How to run the code

- Count number of evets with `countEvents.py`

- Produce nanoAOD with `NANO_ttHEFT.py`

- Run on miniAOD with `ReadMiniAODweights.py`
