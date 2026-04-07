#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/NanoAOD/interface/MergeableCounterTable.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/transform.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoHeader.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "boost/algorithm/string.hpp"


#include "PhysicsTools/NanoAOD/interface/ConvertHex.h"

// From https://github.com/TopEFT/EFTGenReader
#include "EFTGenReader/EFTHelperUtilities/interface/WCPoint.h"
#include "EFTGenReader/EFTHelperUtilities/interface/WCFit.h"
//#include "PhysicsTools/NanoAOD/interface/WCPoint.h"
//#include "PhysicsTools/NanoAOD/interface/WCFit.h"

#include <memory>

#include <vector>
#include <unordered_map>
#include <iostream>
#include <regex>

namespace {
  ///  ---- Cache object for running sums of weights ----
  struct Counter {
    Counter() : num(0), sumw(0), sumw2(0), sumPDF(), sumScale(), sumRwgt(), sumNamed(), sumPS() {}

    // the counters
    long long num;
    long double sumw;
    long double sumw2;
    std::vector<long double> sumPDF, sumScale, sumRwgt, sumNamed, sumPS;

    void clear() {
      num = 0;
      sumw = 0;
      sumw2 = 0;
      sumPDF.clear();
      sumScale.clear();
      sumRwgt.clear();
      sumNamed.clear(), sumPS.clear();
    }

    // inc the counters
    void incGenOnly(double w) {
      num++;
      sumw += w;
      sumw2 += (w * w);
    }

    void incPSOnly(double w0, const std::vector<double>& wPS) {
      if (!wPS.empty()) {
        if (sumPS.empty())
          sumPS.resize(wPS.size(), 0);
        for (unsigned int i = 0, n = wPS.size(); i < n; ++i)
          sumPS[i] += (w0 * wPS[i]);
      }
    }

    void incLHE(double w0,
                const std::vector<double>& wScale,
                const std::vector<double>& wPDF,
                const std::vector<double>& wRwgt,
                const std::vector<double>& wNamed,
                const std::vector<double>& wPS) {
      // add up weights
      incGenOnly(w0);
      // then add up variations
      if (!wScale.empty()) {
        if (sumScale.empty())
          sumScale.resize(wScale.size(), 0);
        for (unsigned int i = 0, n = wScale.size(); i < n; ++i)
          sumScale[i] += (w0 * wScale[i]);
      }
      if (!wPDF.empty()) {
        if (sumPDF.empty())
          sumPDF.resize(wPDF.size(), 0);
        for (unsigned int i = 0, n = wPDF.size(); i < n; ++i)
          sumPDF[i] += (w0 * wPDF[i]);
      }
      if (!wRwgt.empty()) {
        if (sumRwgt.empty())
          sumRwgt.resize(wRwgt.size(), 0);
        for (unsigned int i = 0, n = wRwgt.size(); i < n; ++i)
          sumRwgt[i] += (w0 * wRwgt[i]);
      }
      if (!wNamed.empty()) {
        if (sumNamed.empty())
          sumNamed.resize(wNamed.size(), 0);
        for (unsigned int i = 0, n = wNamed.size(); i < n; ++i)
          sumNamed[i] += (w0 * wNamed[i]);
      }
      incPSOnly(w0, wPS);
    }

    void merge(const Counter& other) {
      num += other.num;
      sumw += other.sumw;
      sumw2 += other.sumw2;
      if (sumScale.empty() && !other.sumScale.empty())
        sumScale.resize(other.sumScale.size(), 0);
      if (sumPDF.empty() && !other.sumPDF.empty())
        sumPDF.resize(other.sumPDF.size(), 0);
      if (sumRwgt.empty() && !other.sumRwgt.empty())
        sumRwgt.resize(other.sumRwgt.size(), 0);
      if (sumNamed.empty() && !other.sumNamed.empty())
        sumNamed.resize(other.sumNamed.size(), 0);
      if (sumPS.empty() && !other.sumPS.empty())
        sumPS.resize(other.sumPS.size(), 0);
      if (!other.sumScale.empty())
        for (unsigned int i = 0, n = sumScale.size(); i < n; ++i)
          sumScale[i] += other.sumScale[i];
      if (!other.sumPDF.empty())
        for (unsigned int i = 0, n = sumPDF.size(); i < n; ++i)
          sumPDF[i] += other.sumPDF[i];
      if (!other.sumRwgt.empty())
        for (unsigned int i = 0, n = sumRwgt.size(); i < n; ++i)
          sumRwgt[i] += other.sumRwgt[i];
      if (!other.sumNamed.empty())
        for (unsigned int i = 0, n = sumNamed.size(); i < n; ++i)
          sumNamed[i] += other.sumNamed[i];
      if (!other.sumPS.empty())
        for (unsigned int i = 0, n = sumPS.size(); i < n; ++i)
          sumPS[i] += other.sumPS[i];
    }
  };

  struct CounterMap {
    std::map<std::string, Counter> countermap;
    Counter* active_el = nullptr;
    std::string active_label = "";
    void merge(const CounterMap& other) {
      for (const auto& y : other.countermap)
        countermap[y.first].merge(y.second);
      active_el = nullptr;
    }
    void clear() {
      for (auto x : countermap)
        x.second.clear();
      active_el = nullptr;
      active_label = "";
    }
    void setLabel(std::string label) {
      active_el = &(countermap[label]);
      active_label = label;
    }
    void checkLabelSet() {
      if (!active_el)
        throw cms::Exception("LogicError", "Called CounterMap::get() before setting the active label\n");
    }
    Counter* get() {
      checkLabelSet();
      return active_el;
    }
    std::string& getLabel() {
      checkLabelSet();
      return active_label;
    }
  };

  ///  ---- RunCache object for dynamic choice of LHE IDs ----
  struct DynamicWeightChoice {
    // choice of LHE weights
    // ---- scale ----
    std::vector<std::string> scaleWeightIDs;
    std::string scaleWeightsDoc;
    // ---- pdf ----
    std::vector<std::string> pdfWeightIDs;
    std::string pdfWeightsDoc;
    // ---- rwgt ----
    std::vector<std::string> rwgtIDs;
    std::string rwgtWeightDoc;
  };

  struct DynamicWeightChoiceGenInfo {
    // choice of LHE weights
    // ---- scale ----
    std::vector<unsigned int> scaleWeightIDs;
    std::string scaleWeightsDoc;
    // ---- pdf ----
    std::vector<unsigned int> pdfWeightIDs;
    std::string pdfWeightsDoc;
    // ---- ps ----
    std::vector<unsigned int> defPSWeightIDs = {6, 7, 8, 9};
    std::vector<unsigned int> defPSWeightIDs_alt = {27, 5, 26, 4};
    bool matchPS_alt = false;
    std::vector<unsigned int> psWeightIDs;
    unsigned int psBaselineID = 1;
    std::string psWeightsDoc;

    void setMissingWeight(int idx) { psWeightIDs[idx] = (matchPS_alt) ? defPSWeightIDs_alt[idx] : defPSWeightIDs[idx]; }

    bool empty() const { return scaleWeightIDs.empty() && pdfWeightIDs.empty() && psWeightIDs.empty(); }
  };

  struct LumiCacheInfoHolder {
    CounterMap countermap;
    DynamicWeightChoiceGenInfo weightChoice;
    void clear() {
      countermap.clear();
      weightChoice = DynamicWeightChoiceGenInfo();
    }
  };

  float stof_fortrancomp(const std::string& str) {
    std::string::size_type match = str.find('d');
    if (match != std::string::npos) {
      std::string pre = str.substr(0, match);
      std::string post = str.substr(match + 1);
      return std::stof(pre) * std::pow(10.0f, std::stof(post));
    } else {
      return std::stof(str);
    }
  }
  ///  -------------- temporary objects --------------
  struct ScaleVarWeight {
    std::string wid, label;
    std::pair<float, float> scales;
    ScaleVarWeight(const std::string& id, const std::string& text, const std::string& muR, const std::string& muF)
        : wid(id), label(text), scales(stof_fortrancomp(muR), stof_fortrancomp(muF)) {}
    bool operator<(const ScaleVarWeight& other) {
      return (scales == other.scales ? wid < other.wid : scales < other.scales);
    }
  };
  struct PDFSetWeights {
    std::vector<std::string> wids;
    std::pair<unsigned int, unsigned int> lhaIDs;
    PDFSetWeights(const std::string& wid, unsigned int lhaID) : wids(1, wid), lhaIDs(lhaID, lhaID) {}
    bool operator<(const PDFSetWeights& other) const { return lhaIDs < other.lhaIDs; }
    void add(const std::string& wid, unsigned int lhaID) {
      wids.push_back(wid);
      lhaIDs.second = lhaID;
    }
    bool maybe_add(const std::string& wid, unsigned int lhaID) {
      if (lhaID == lhaIDs.second + 1) {
        lhaIDs.second++;
        wids.push_back(wid);
        return true;
      } else {
        return false;
      }
    }
  };
}  // namespace

namespace {
  inline bool isEFTWeightId(const std::string& id) {
    return (id.rfind("EFTrwgt", 0) == 0 || id.rfind("eftrwgt", 0) == 0);
  }

  inline void addUniqueRwgtID(std::vector<std::string>& ids, const std::string& id) {
    if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
      ids.emplace_back(id);
    }
  }
}  // namespace
class GenWeightsTableProducer : public edm::global::EDProducer<edm::StreamCache<LumiCacheInfoHolder>,
                                                               edm::RunCache<DynamicWeightChoice>,
                                                               edm::RunSummaryCache<CounterMap>,
                                                               edm::EndRunProducer> {
public:
  GenWeightsTableProducer(edm::ParameterSet const& params)
      : genTag_(consumes<GenEventInfoProduct>(params.getParameter<edm::InputTag>("genEvent"))),
        lheLabel_(params.getParameter<std::vector<edm::InputTag>>("lheInfo")),
        lheTag_(edm::vector_transform(lheLabel_,
                                      [this](const edm::InputTag& tag) { return mayConsume<LHEEventProduct>(tag); })),
        lheRunTag_(edm::vector_transform(
            lheLabel_, [this](const edm::InputTag& tag) { return mayConsume<LHERunInfoProduct, edm::InRun>(tag); })),
        genLumiInfoHeadTag_(
            mayConsume<GenLumiInfoHeader, edm::InLumi>(params.getParameter<edm::InputTag>("genLumiInfoHeader"))),
        namedWeightIDs_(params.getParameter<std::vector<std::string>>("namedWeightIDs")),
        namedWeightLabels_(params.getParameter<std::vector<std::string>>("namedWeightLabels")),
        lheWeightPrecision_(params.getParameter<int32_t>("lheWeightPrecision")),
        maxPdfWeights_(params.getParameter<uint32_t>("maxPdfWeights")),
        keepAllPSWeights_(params.getParameter<bool>("keepAllPSWeights")),
        debug_(params.getUntrackedParameter<bool>("debug", false)),
        debugRun_(debug_.load()),
        hasIssuedWarning_(false),
        psWeightWarning_(false) {
    produces<nanoaod::FlatTable>();
    produces<std::string>("genModel");
    produces<nanoaod::FlatTable>("LHEScale");
    produces<nanoaod::FlatTable>("LHEPdf");
    produces<nanoaod::FlatTable>("LHEEFT");
    produces<nanoaod::FlatTable>("WCnames");
    produces<nanoaod::FlatTable>("LHEReweighting");
    produces<nanoaod::FlatTable>("LHENamed");
    produces<nanoaod::FlatTable>("PS");
    produces<nanoaod::MergeableCounterTable, edm::Transition::EndRun>();
    if (namedWeightIDs_.size() != namedWeightLabels_.size()) {
      throw cms::Exception("Configuration", "Size mismatch between namedWeightIDs & namedWeightLabels");
    }
    for (const edm::ParameterSet& pdfps : params.getParameter<std::vector<edm::ParameterSet>>("preferredPDFs")) {
      const std::string& name = pdfps.getParameter<std::string>("name");
      uint32_t lhaid = pdfps.getParameter<uint32_t>("lhaid");
      preferredPDFLHAIDs_.push_back(lhaid);
      lhaNameToID_[name] = lhaid;
      lhaNameToID_[name + ".LHgrid"] = lhaid;
    }
  }

  ~GenWeightsTableProducer() override {}

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    // get my counter for weights
    Counter* counter = streamCache(id)->countermap.get();

    // generator information (always available)
    edm::Handle<GenEventInfoProduct> genInfo;
    iEvent.getByToken(genTag_, genInfo);
    double weight = genInfo->weight();

    // table for gen info, always available
    auto out = std::make_unique<nanoaod::FlatTable>(1, "genWeight", true);
    out->setDoc("generator weight");
    out->addColumnValue<float>("", weight, "generator weight", nanoaod::FlatTable::FloatColumn);
    iEvent.put(std::move(out));

    std::string model_label = streamCache(id)->countermap.getLabel();
    auto outM = std::make_unique<std::string>((!model_label.empty()) ? std::string("GenModel_") + model_label : "");
    iEvent.put(std::move(outM), "genModel");
    bool getLHEweightsFromGenInfo = !model_label.empty();

    // tables for LHE weights, may not be filled
    std::unique_ptr<nanoaod::FlatTable> lheScaleTab, lhePdfTab, lheRwgtTab, lheNamedTab, lheEFTTab, wcnamesTab; 
    std::unique_ptr<nanoaod::FlatTable> genPSTab;

    edm::Handle<LHEEventProduct> lheInfo;
    
    std::cerr << "[DEBUG GenWeightsTableProducer] produce(): start LHEEventProduct lookup" << std::endl;
    for (const auto& lheTag : lheTag_) {
      std::cerr << "[DEBUG GenWeightsTableProducer] produce(): trying event-level LHE tag" << std::endl;

      std::cerr << "[DEBUG GenWeightsTableProducer] produce(): before iEvent.getByToken" << std::endl;
      iEvent.getByToken(lheTag, lheInfo);
      std::cerr << "[DEBUG GenWeightsTableProducer] produce(): after iEvent.getByToken valid=" << lheInfo.isValid() << std::endl;
      std::cerr << "[DEBUG GenWeightsTableProducer] found valid LHERunInfoProduct" << std::endl;
      if (lheInfo.isValid()) {
        break;
      }
    }

    const auto genWeightChoice = &(streamCache(id)->weightChoice);
    if (lheInfo.isValid()) {
      if (getLHEweightsFromGenInfo && !hasIssuedWarning_.exchange(true))
        edm::LogWarning("LHETablesProducer")
            << "Found both a LHEEventProduct and a GenLumiInfoHeader: will only save weights from LHEEventProduct.\n";
      // get the dynamic choice of weights
      const DynamicWeightChoice* weightChoice = runCache(iEvent.getRun().index());
      if (weightChoice == nullptr) {
        edm::LogWarning("LHETablesProducer")
            << "Run cache for DynamicWeightChoice is null. "
            << "Falling back to empty run-level weight metadata.";
        static const DynamicWeightChoice emptyChoice;
        weightChoice = &emptyChoice;
      }        
      // go fill tables
      // fillLHEWeightTables(counter, weightChoice, genWeightChoice, weight, *lheInfo, *genInfo, lheScaleTab, lhePdfTab, lheRwgtTab, lheNamedTab, genPSTab);
      fillLHEWeightTables(counter, weightChoice, genWeightChoice, weight, *lheInfo, *genInfo, lheScaleTab, lhePdfTab, lheEFTTab, wcnamesTab, lheRwgtTab, lheNamedTab, genPSTab);
    } else if (getLHEweightsFromGenInfo) {
      fillLHEPdfWeightTablesFromGenInfo(
          counter, genWeightChoice, weight, *genInfo, lheScaleTab, lhePdfTab, lheNamedTab, genPSTab);
      lheRwgtTab = std::make_unique<nanoaod::FlatTable>(1, "LHEReweightingWeights", true);
      //lheNamedTab = std::make_unique<nanoaod::FlatTable>(1, "LHENamedWeights", true);
      //genPSTab = std::make_unique<nanoaod::FlatTable>(1, "PSWeight", true);
    } else {
      // Still try to add the PS weights
      fillOnlyPSWeightTable(counter, genWeightChoice, weight, *genInfo, genPSTab);
      // make dummy values
      lheScaleTab = std::make_unique<nanoaod::FlatTable>(1, "LHEScaleWeights", true);
      lhePdfTab = std::make_unique<nanoaod::FlatTable>(1, "LHEPdfWeights", true);
      lheEFTTab.reset(new nanoaod::FlatTable(1, "LHEEFTWeights", true));
      wcnamesTab.reset(new nanoaod::FlatTable(1, "WCNames", true));
      lheRwgtTab = std::make_unique<nanoaod::FlatTable>(1, "LHEReweightingWeights", true);
      lheNamedTab = std::make_unique<nanoaod::FlatTable>(1, "LHENamedWeights", true);
      if (!hasIssuedWarning_.exchange(true)) {
        edm::LogWarning("LHETablesProducer") << "No LHEEventProduct, so there will be no LHE Tables\n";
      }
    }

    iEvent.put(std::move(lheScaleTab), "LHEScale");
    iEvent.put(std::move(lhePdfTab), "LHEPdf");
    iEvent.put(std::move(lheEFTTab), "LHEEFT");
    iEvent.put(std::move(wcnamesTab), "WCnames");
    iEvent.put(std::move(lheRwgtTab), "LHEReweighting");
    iEvent.put(std::move(lheNamedTab), "LHENamed");
    iEvent.put(std::move(genPSTab), "PS");
  }

  void fillLHEWeightTables(Counter* counter,
                           const DynamicWeightChoice* weightChoice,
                           const DynamicWeightChoiceGenInfo* genWeightChoice,
                           double genWeight,
                           const LHEEventProduct& lheProd,
                           const GenEventInfoProduct& genProd,
                           std::unique_ptr<nanoaod::FlatTable>& outScale,
                           std::unique_ptr<nanoaod::FlatTable>& outPdf,
                           std::unique_ptr<nanoaod::FlatTable> & outEFT,
                           std::unique_ptr<nanoaod::FlatTable> & outWCnam,
                           std::unique_ptr<nanoaod::FlatTable>& outRwgt,
                           std::unique_ptr<nanoaod::FlatTable>& outNamed,
                           std::unique_ptr<nanoaod::FlatTable>& outPS) const {
    bool lheDebug = debug_.exchange(
        false);  // make sure only the first thread dumps out this (even if may still be mixed up with other output, but nevermind)

    const std::vector<std::string>& scaleWeightIDs = weightChoice->scaleWeightIDs;
    const std::vector<std::string>& pdfWeightIDs = weightChoice->pdfWeightIDs;
    std::cerr << "[DEBUG GenWeightsTableProducer] reached rwgtWeightIDs_ref assignment" << std::endl;
    const std::vector<std::string>& rwgtWeightIDs_ref = weightChoice->rwgtIDs;
    std::cerr << "[DEBUG GenWeightsTableProducer] weightChoice ptr=" << (void*)weightChoice << std::endl;
    std::vector<std::string> rwgtWeightIDs_local;
    const std::vector<std::string>* rwgtWeightIDs_ptr = &rwgtWeightIDs_ref;

    if (rwgtWeightIDs_ref.empty()) {
      std::cerr << "[DEBUG GenWeightsTableProducer] rwgtWeightIDs_ref is empty, building fallback from event-level weights" << std::endl;
      for (const auto& weight : lheProd.weights()) {
        addUniqueRwgtID(rwgtWeightIDs_local, weight.id);
      }
      std::cerr << "[DEBUG GenWeightsTableProducer] fallback rwgtWeightIDs_local size=" << rwgtWeightIDs_local.size() << std::endl;
      for (size_t __i = 0; __i < rwgtWeightIDs_local.size() && __i < 8; ++__i) {
        std::cerr << "[DEBUG GenWeightsTableProducer] fallback rwgt id[" << __i << "]=" << rwgtWeightIDs_local[__i] << std::endl;
      }
      rwgtWeightIDs_ptr = &rwgtWeightIDs_local;
    }

    const std::vector<std::string>& rwgtWeightIDs = *rwgtWeightIDs_ptr;

    std::cerr << "[DEBUG GenWeightsTableProducer] produce(): lheProd weight count=" << lheProd.weights().size() << std::endl;
    {
      size_t __dbg_n = 0;
      for (const auto& __dbg_w : lheProd.weights()) {
        std::cerr << "[DEBUG GenWeightsTableProducer] produce(): weight[" << __dbg_n << "] id=" << __dbg_w.id << " wgt=" << __dbg_w.wgt << std::endl;
        if (++__dbg_n >= 8) break;
      }
    }

    double w0 = lheProd.originalXWGTUP();

    // ==================== IMPROVED EFT REWEIGHTING (long names support) ====================
    std::vector<WCPoint> vwc;
    int nEFT_found = 0;

    std::cerr << "[DEBUG GenWeightsTableProducer] about to inspect lheProd.weights()" << std::endl;
    for (const auto& weight : lheProd.weights()) {
      if (isEFTWeightId(weight.id)) {
        nEFT_found++;

        std::string clean_id = weight.id;

        // Remove _nlo suffix if present
        if (clean_id.find("_nlo") != std::string::npos)
          clean_id = std::regex_replace(clean_id, std::regex("_nlo"), "");

        // For long names like EFTrwgt0_ctGIm_m0p936_... keep only after first "_"
        size_t pos = clean_id.find("_");
        if (pos != std::string::npos)
          clean_id = clean_id.substr(pos + 1);

        std::vector<std::string> wc_tokens;
        boost::split(wc_tokens, clean_id, boost::is_any_of("_"), boost::token_compress_on);

        std::string wcpoint_id;
        for (size_t i = 0; i + 1 < wc_tokens.size(); i += 2) {
          if (wc_tokens[i].empty()) continue;
          std::string val = wc_tokens[i + 1];
          if (!val.empty() && val[0] == 'm') val[0] = '-';
          for (auto& ch : val) {
            if (ch == 'p') ch = '.';
          }
          if (!val.empty() && val.back() == '.') val += '0';
          if (!wcpoint_id.empty()) wcpoint_id += "_";
          wcpoint_id += wc_tokens[i] + "_" + val;
        }

        if (debug_) {
          std::cout << "DEBUG EFT: raw='" << weight.id
                    << "' -> clean='" << wcpoint_id
                    << "' wgt=" << weight.wgt << std::endl;
        }
        vwc.emplace_back(std::string("EFTrwgt_") + wcpoint_id, weight.wgt);
      }
    }
    WCFit wcfit(vwc, "wcfit");
    std::vector<double> coefs = wcfit.getCoefficients();
    int nCoef = coefs.size();
    std::vector<std::string> wcnames_string = wcfit.getNames();
    if (!wcnames_string.empty()) {
      wcnames_string.erase(wcnames_string.begin()); // remove 'sm'
    }
    std::vector<int> wcnames = VectorStringToInt(wcnames_string);
    int nWC = wcnames.size();
    // =====================================================================

    std::vector<double> wScale(scaleWeightIDs.size(), 1), wPDF(pdfWeightIDs.size(), 1), wRwgt(rwgtWeightIDs.size(), 1), wNamed(namedWeightIDs_.size(), 1);

    std::cerr << "[DEBUG GenWeightsTableProducer] about to inspect lheProd.weights()" << std::endl;
    for (auto& weight : lheProd.weights()) {
      if (lheDebug)
        printf("Weight  %+9.5f   rel %+9.5f   for id %s\n", weight.wgt, weight.wgt / w0, weight.id.c_str());
      // now we do it slowly, can be optimized
      auto mScale = std::find(scaleWeightIDs.begin(), scaleWeightIDs.end(), weight.id);
      if (mScale != scaleWeightIDs.end())
        wScale[mScale - scaleWeightIDs.begin()] = weight.wgt / w0;

      auto mPDF = std::find(pdfWeightIDs.begin(), pdfWeightIDs.end(), weight.id);
      if (mPDF != pdfWeightIDs.end())
        wPDF[mPDF - pdfWeightIDs.begin()] = weight.wgt / w0;
      auto mRwgt = std::find(rwgtWeightIDs.begin(), rwgtWeightIDs.end(), weight.id);
      if (mRwgt != rwgtWeightIDs.end())
        wRwgt[mRwgt - rwgtWeightIDs.begin()] = weight.wgt / w0;

      auto mNamed = std::find(namedWeightIDs_.begin(), namedWeightIDs_.end(), weight.id);
      if (mNamed != namedWeightIDs_.end())
        wNamed[mNamed - namedWeightIDs_.begin()] = weight.wgt / w0;
    }

    std::vector<double> wPS;
    std::string psWeightDocStr;
    setPSWeightInfo(genProd.weights(), genWeightChoice, wPS, psWeightDocStr);

    outPS = std::make_unique<nanoaod::FlatTable>(wPS.size(), "PSWeight", false);
    outPS->addColumn<float>("", wPS, psWeightDocStr, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    outScale = std::make_unique<nanoaod::FlatTable>(wScale.size(), "LHEScaleWeight", false);
    outScale->addColumn<float>(
        "", wScale, weightChoice->scaleWeightsDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    outPdf = std::make_unique<nanoaod::FlatTable>(wPDF.size(), "LHEPdfWeight", false);
    outPdf->addColumn<float>(
        "", wPDF, weightChoice->pdfWeightsDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    outRwgt = std::make_unique<nanoaod::FlatTable>(wRwgt.size(), "LHEReweightingWeight", false);
    outRwgt->addColumn<float>(
        "", wRwgt, weightChoice->rwgtWeightDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    std::string EFTcoefDoc = "EFT fit coefficients";
    outEFT.reset(new nanoaod::FlatTable(nCoef, "EFTfitCoefficients", false));
    outEFT->addColumn<float>("", coefs, EFTcoefDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    std::string WCnamDoc = "EFT WC names";
    outWCnam.reset(new nanoaod::FlatTable(nWC, "WCnames", false));
    outWCnam->addColumn<int>("", wcnames, WCnamDoc, nanoaod::FlatTable::IntColumn, lheWeightPrecision_);

    outNamed = std::make_unique<nanoaod::FlatTable>(1, "LHEWeight", true);
    outNamed->addColumnValue<float>("originalXWGTUP",
                                    lheProd.originalXWGTUP(),
                                    "Nominal event weight in the LHE file",
                                    nanoaod::FlatTable::FloatColumn);
    float djr1= (genProd.DJRValues().size() > 0) ? genProd.DJRValues().at(0) : -1.;
    float djr2= (genProd.DJRValues().size() > 1) ? genProd.DJRValues().at(1) : -1.;
    float djr3= (genProd.DJRValues().size() > 2) ? genProd.DJRValues().at(2) : -1.;
    outNamed->addColumnValue<float>("DJR10", djr1,"djr 1 --> 0 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    outNamed->addColumnValue<float>("DJR21", djr2,"djr 2 --> 1 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    outNamed->addColumnValue<float>("DJR32", djr3,"djr 3 --> 2 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    for (unsigned int i = 0, n = wNamed.size(); i < n; ++i) {
      outNamed->addColumnValue<float>(namedWeightLabels_[i],
                                      wNamed[i],
                                      "LHE weight for id " + namedWeightIDs_[i] + ", relative to nominal",
                                      nanoaod::FlatTable::FloatColumn,
                                      lheWeightPrecision_);
    }

    counter->incLHE(genWeight, wScale, wPDF, wRwgt, wNamed, wPS);
  }

  void fillLHEPdfWeightTablesFromGenInfo(Counter* counter,
                                         const DynamicWeightChoiceGenInfo* weightChoice,
                                         double genWeight,
                                         const GenEventInfoProduct& genProd,
                                         std::unique_ptr<nanoaod::FlatTable>& outScale,
                                         std::unique_ptr<nanoaod::FlatTable>& outPdf,
                                         std::unique_ptr<nanoaod::FlatTable>& outNamed,
                                         std::unique_ptr<nanoaod::FlatTable>& outPS) const {
    const std::vector<unsigned int>& scaleWeightIDs = weightChoice->scaleWeightIDs;
    const std::vector<unsigned int>& pdfWeightIDs = weightChoice->pdfWeightIDs;

    auto weights = genProd.weights();
    double w0 = (weights.size() > 1) ? weights.at(1) : 1.;
    double originalXWGTUP = (weights.size() > 1) ? weights.at(1) : 1.;

    std::vector<double> wScale, wPDF, wPS;
    for (auto id : scaleWeightIDs)
      wScale.push_back(weights.at(id) / w0);
    for (auto id : pdfWeightIDs) {
      wPDF.push_back(weights.at(id) / w0);
    }

    std::string psWeightsDocStr;
    setPSWeightInfo(genProd.weights(), weightChoice, wPS, psWeightsDocStr);

    outScale = std::make_unique<nanoaod::FlatTable>(wScale.size(), "LHEScaleWeight", false);
    outScale->addColumn<float>(
        "", wScale, weightChoice->scaleWeightsDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    outPdf = std::make_unique<nanoaod::FlatTable>(wPDF.size(), "LHEPdfWeight", false);
    outPdf->addColumn<float>(
        "", wPDF, weightChoice->pdfWeightsDoc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    outPS = std::make_unique<nanoaod::FlatTable>(wPS.size(), "PSWeight", false);
    outPS->addColumn<float>("", wPS, psWeightsDocStr, nanoaod::FlatTable::FloatColumn,
                                    lheWeightPrecision_);
    outNamed = std::make_unique<nanoaod::FlatTable>(1, "LHEWeight", true);
    outNamed->addColumnValue<float>(
        "originalXWGTUP", originalXWGTUP, "Nominal event weight in the LHE file", nanoaod::FlatTable::FloatColumn);
    float djr1= (genProd.DJRValues().size() > 0) ? genProd.DJRValues().at(0) : -1.;
    float djr2= (genProd.DJRValues().size() > 1) ? genProd.DJRValues().at(1) : -1.;
    float djr3= (genProd.DJRValues().size() > 2) ? genProd.DJRValues().at(2) : -1.;
    outNamed->addColumnValue<float>("DJR10", djr1,"djr 1 --> 0 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    outNamed->addColumnValue<float>("DJR21", djr2,"djr 2 --> 1 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    outNamed->addColumnValue<float>("DJR32", djr3,"djr 3 --> 2 (-1 if the value is not available)", nanoaod::FlatTable::FloatColumn);
    /*for (unsigned int i = 0, n = wNamed.size(); i < n; ++i) {
      outNamed->addColumnValue<float>(namedWeightLabels_[i], wNamed[i], "LHE weight for id "+namedWeightIDs_[i]+", relative to nominal", nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);
      }*/

    counter->incLHE(genWeight, wScale, wPDF, std::vector<double>(), std::vector<double>(), wPS);
  }

  void fillOnlyPSWeightTable(Counter* counter,
                             const DynamicWeightChoiceGenInfo* genWeightChoice,
                             double genWeight,
                             const GenEventInfoProduct& genProd,
                             std::unique_ptr<nanoaod::FlatTable>& outPS) const {
    std::vector<double> wPS;
    std::string psWeightDocStr;
    setPSWeightInfo(genProd.weights(), genWeightChoice, wPS, psWeightDocStr);
    outPS = std::make_unique<nanoaod::FlatTable>(wPS.size(), "PSWeight", false);
    outPS->addColumn<float>("", wPS, psWeightDocStr, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    counter->incGenOnly(genWeight);
    counter->incPSOnly(genWeight, wPS);
  }

  void setPSWeightInfo(const std::vector<double>& genWeights,
                       const DynamicWeightChoiceGenInfo* genWeightChoice,
                       std::vector<double>& wPS,
                       std::string& psWeightDocStr) const {
    wPS.clear();
    // isRegularPSSet = keeping all weights and the weights are a usual size, ie
    //                  all weights are PS weights (don't use header incase missing names)
    bool isRegularPSSet = keepAllPSWeights_ && (genWeights.size() == 14 || genWeights.size() == 46);
    if (!genWeightChoice->psWeightIDs.empty() && !isRegularPSSet) {
      psWeightDocStr = genWeightChoice->psWeightsDoc;
      double psNom = genWeights.at(genWeightChoice->psBaselineID);
      for (auto wgtidx : genWeightChoice->psWeightIDs) {
        wPS.push_back(genWeights.at(wgtidx) / psNom);
      }
    } else {
      int vectorSize =
          keepAllPSWeights_ ? (genWeights.size() - 2) : ((genWeights.size() == 14 || genWeights.size() == 46) ? 4 : 1);

      if (vectorSize > 1) {
        double nominal = genWeights.at(1);  // Called 'Baseline' in GenLumiInfoHeader
        if (keepAllPSWeights_) {
          for (int i = 0; i < vectorSize; i++) {
            wPS.push_back(genWeights.at(i + 2) / nominal);
          }
          psWeightDocStr = "All PS weights (w_var / w_nominal)";
        } else {
          if (!psWeightWarning_.exchange(true))
            edm::LogWarning("LHETablesProducer")
                << "GenLumiInfoHeader not found: Central PartonShower weights will fill with the 6-10th entries \n"
                << "    This may incorrect for some mcs (madgraph 2.6.1 with its `isr:murfact=0.5` have a differnt "
                   "order )";
          for (std::size_t i = 6; i < 10; i++) {
            wPS.push_back(genWeights.at(i) / nominal);
          }
          psWeightDocStr =
              "PS weights (w_var / w_nominal);   [0] is ISR=2 FSR=1; [1] is ISR=1 FSR=2"
              "[2] is ISR=0.5 FSR=1; [3] is ISR=1 FSR=0.5;";
        }
      } else {
        wPS.push_back(1.0);
        psWeightDocStr = "dummy PS weight (1.0) ";
      }
    }
  }

  // create an empty counter
  std::shared_ptr<DynamicWeightChoice> globalBeginRun(edm::Run const&, edm::EventSetup const&) const override {
    std::cerr << "[DEBUG GenWeightsTableProducer] enter globalBeginRun" << std::endl;
    auto weightChoice = std::make_shared<DynamicWeightChoice>();
    bool lheDebug = debugRun_.exchange(false);

    // In single-process ExternalLHEProducer workflows, reading
    // LHERunInfoProduct here can trigger a beginRun LogicError.
    // Rely on event-level LHEEventProduct in produce() instead.
    if (lheDebug) {
      edm::LogInfo("LHETablesProducer")
          << "Skipping run-level LHERunInfoProduct parsing in globalBeginRun; "
          << "will rely on event-level LHEEventProduct instead.";
    }
    return weightChoice;
  }

  // create an empty counter
  std::unique_ptr<LumiCacheInfoHolder> beginStream(edm::StreamID) const override {
    return std::make_unique<LumiCacheInfoHolder>();
  }
  // inizialize to zero at begin run
  void streamBeginRun(edm::StreamID id, edm::Run const&, edm::EventSetup const&) const override {
    streamCache(id)->clear();
  }
  void streamBeginLuminosityBlock(edm::StreamID id,
                                  edm::LuminosityBlock const& lumiBlock,
                                  edm::EventSetup const& eventSetup) const override {
    auto counterMap = &(streamCache(id)->countermap);
    edm::Handle<GenLumiInfoHeader> genLumiInfoHead;
    lumiBlock.getByToken(genLumiInfoHeadTag_, genLumiInfoHead);
    if (!genLumiInfoHead.isValid())
      edm::LogWarning("LHETablesProducer")
          << "No GenLumiInfoHeader product found, will not fill generator model string.\n";

    std::string label;
    if (genLumiInfoHead.isValid()) {
      label = genLumiInfoHead->configDescription();
      boost::replace_all(label, "-", "_");
      boost::replace_all(label, "/", "_");
    }
    counterMap->setLabel(label);

    if (genLumiInfoHead.isValid()) {
      auto weightChoice = &(streamCache(id)->weightChoice);

      std::vector<ScaleVarWeight> scaleVariationIDs;
      std::vector<PDFSetWeights> pdfSetWeightIDs;
      weightChoice->psWeightIDs.clear();

      std::regex scalew("LHE,\\s+id\\s+=\\s+(\\d+),\\s+(.+)\\,\\s+mur=(\\S+)\\smuf=(\\S+)");
      std::regex pdfw("LHE,\\s+id\\s+=\\s+(\\d+),\\s+(.+),\\s+Member\\s+(\\d+)\\s+of\\ssets\\s+(\\w+\\b)");
      std::regex mainPSw("sr(Def|:murfac=)(Hi|Lo|_dn|_up|0.5|2.0)");
      std::smatch groups;
      auto weightNames = genLumiInfoHead->weightNames();
      std::unordered_map<std::string, uint32_t> knownPDFSetsFromGenInfo_;
      unsigned int weightIter = 0;
      for (const auto& line : weightNames) {
        if (std::regex_search(line, groups, scalew)) {  // scale variation
          auto id = groups.str(1);
          auto group = groups.str(2);
          auto mur = groups.str(3);
          auto muf = groups.str(4);
          if (group.find("Central scale variation") != std::string::npos)
            scaleVariationIDs.emplace_back(groups.str(1), groups.str(2), groups.str(3), groups.str(4));
        } else if (std::regex_search(line, groups, pdfw)) {  // PDF variation
          auto id = groups.str(1);
          auto group = groups.str(2);
          auto memberid = groups.str(3);
          auto pdfset = groups.str(4);
          if (group.find(pdfset) != std::string::npos) {
            if (knownPDFSetsFromGenInfo_.find(pdfset) == knownPDFSetsFromGenInfo_.end()) {
              knownPDFSetsFromGenInfo_[pdfset] = std::atoi(id.c_str());
              pdfSetWeightIDs.emplace_back(id, std::atoi(id.c_str()));
            } else
              pdfSetWeightIDs.back().add(id, std::atoi(id.c_str()));
          }
        } else if (line == "Baseline") {
          weightChoice->psBaselineID = weightIter;
        } else if (line.find("isr") != std::string::npos || line.find("fsr") != std::string::npos) {
          weightChoice->matchPS_alt = line.find("sr:") != std::string::npos;  // (f/i)sr: for new weights
          if (keepAllPSWeights_) {
            weightChoice->psWeightIDs.push_back(weightIter);  // PS variations
          } else if (std::regex_search(line, groups, mainPSw)) {
            if (weightChoice->psWeightIDs.empty())
              weightChoice->psWeightIDs = std::vector<unsigned int>(4, -1);
            int psIdx = (line.find("fsr") != std::string::npos) ? 1 : 0;
            psIdx += (groups.str(2) == "Hi" || groups.str(2) == "_up" || groups.str(2) == "2.0") ? 0 : 2;
            weightChoice->psWeightIDs[psIdx] = weightIter;
          }
        }
        weightIter++;
      }
      if (keepAllPSWeights_) {
        weightChoice->psWeightsDoc = "All PS weights (w_var / w_nominal)";
      } else if (weightChoice->psWeightIDs.size() == 4) {
        weightChoice->psWeightsDoc =
            "PS weights (w_var / w_nominal);   [0] is ISR=2 FSR=1; [1] is ISR=1 FSR=2"
            "[2] is ISR=0.5 FSR=1; [3] is ISR=1 FSR=0.5;";
        for (int i = 0; i < 4; i++) {
          if (static_cast<int>(weightChoice->psWeightIDs[i]) == -1)
            weightChoice->setMissingWeight(i);
        }
      } else {
        weightChoice->psWeightsDoc = "dummy PS weight (1.0) ";
      }

      weightChoice->scaleWeightIDs.clear();
      weightChoice->pdfWeightIDs.clear();

      std::sort(scaleVariationIDs.begin(), scaleVariationIDs.end());
      std::stringstream scaleDoc;
      scaleDoc << "LHE scale variation weights (w_var / w_nominal); ";
      for (unsigned int isw = 0, nsw = scaleVariationIDs.size(); isw < nsw; ++isw) {
        const auto& sw = scaleVariationIDs[isw];
        if (isw)
          scaleDoc << "; ";
        scaleDoc << "[" << isw << "] is " << sw.label;
        weightChoice->scaleWeightIDs.push_back(std::atoi(sw.wid.c_str()));
      }
      if (!scaleVariationIDs.empty())
        weightChoice->scaleWeightsDoc = scaleDoc.str();
      std::stringstream pdfDoc;
      pdfDoc << "LHE pdf variation weights (w_var / w_nominal) for LHA names ";
      bool found = false;
      for (const auto& pw : pdfSetWeightIDs) {
        if (pw.wids.size() == 1)
          continue;  // only consider error sets
        for (const auto& wantedpdf : lhaNameToID_) {
          auto pdfname = wantedpdf.first;
          if (knownPDFSetsFromGenInfo_.find(pdfname) == knownPDFSetsFromGenInfo_.end())
            continue;
          uint32_t lhaid = knownPDFSetsFromGenInfo_.at(pdfname);
          if (pw.lhaIDs.first != lhaid)
            continue;
          pdfDoc << pdfname;
          for (const auto& x : pw.wids)
            weightChoice->pdfWeightIDs.push_back(std::atoi(x.c_str()));
          if (maxPdfWeights_ < pw.wids.size()) {
            weightChoice->pdfWeightIDs.resize(maxPdfWeights_);  // drop some replicas
            pdfDoc << ", truncated to the first " << maxPdfWeights_ << " replicas";
          }
          weightChoice->pdfWeightsDoc = pdfDoc.str();
          found = true;
          break;
        }
        if (found)
          break;
      }
    }
  }
  // create an empty counter
  std::shared_ptr<CounterMap> globalBeginRunSummary(edm::Run const&, edm::EventSetup const&) const override {
    return std::make_shared<CounterMap>();
  }
  // add this stream to the summary
  void streamEndRunSummary(edm::StreamID id,
                           edm::Run const&,
                           edm::EventSetup const&,
                           CounterMap* runCounterMap) const override {
    runCounterMap->merge(streamCache(id)->countermap);
  }
  // nothing to do per se
  void globalEndRunSummary(edm::Run const&, edm::EventSetup const&, CounterMap* runCounterMap) const override {}
  // write the total to the run
  void globalEndRunProduce(edm::Run& iRun, edm::EventSetup const&, CounterMap const* runCounterMap) const override {
    auto out = std::make_unique<nanoaod::MergeableCounterTable>();

    for (const auto& x : runCounterMap->countermap) {
      auto runCounter = &(x.second);
      std::string label = (!x.first.empty()) ? (std::string("_") + x.first) : "";
      std::string doclabel = (!x.first.empty()) ? (std::string(", for model label ") + x.first) : "";

      out->addInt("genEventCount" + label, "event count" + doclabel, runCounter->num);
      out->addFloat("genEventSumw" + label, "sum of gen weights" + doclabel, runCounter->sumw);
      out->addFloat("genEventSumw2" + label, "sum of gen (weight^2)" + doclabel, runCounter->sumw2);

      double norm = runCounter->sumw ? 1.0 / runCounter->sumw : 1;
      auto sumScales = runCounter->sumScale;
      for (auto& val : sumScales)
        val *= norm;
      out->addVFloatWithNorm("LHEScaleSumw" + label,
                             "Sum of genEventWeight * LHEScaleWeight[i], divided by genEventSumw" + doclabel,
                             sumScales,
                             runCounter->sumw);
      auto sumPDFs = runCounter->sumPDF;
      for (auto& val : sumPDFs)
        val *= norm;
      out->addVFloatWithNorm("LHEPdfSumw" + label,
                             "Sum of genEventWeight * LHEPdfWeight[i], divided by genEventSumw" + doclabel,
                             sumPDFs,
                             runCounter->sumw);
      if (!runCounter->sumRwgt.empty()) {
        auto sumRwgts = runCounter->sumRwgt;
        for (auto& val : sumRwgts)
          val *= norm;
        out->addVFloatWithNorm("LHEReweightingSumw" + label,
                               "Sum of genEventWeight * LHEReweightingWeight[i], divided by genEventSumw" + doclabel,
                               sumRwgts,
                               runCounter->sumw);
      }
      if (!runCounter->sumNamed.empty()) {  // it could be empty if there's no LHE info in the sample
        for (unsigned int i = 0, n = namedWeightLabels_.size(); i < n; ++i) {
          out->addFloatWithNorm(
              "LHESumw_" + namedWeightLabels_[i] + label,
              "Sum of genEventWeight * LHEWeight_" + namedWeightLabels_[i] + ", divided by genEventSumw" + doclabel,
              runCounter->sumNamed[i] * norm,
              runCounter->sumw);
        }
      }
    }
    iRun.put(std::move(out));
  }
  // nothing to do here
  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("genEvent", edm::InputTag("generator"))
        ->setComment("tag for the GenEventInfoProduct, to get the main weight");
    desc.add<edm::InputTag>("genLumiInfoHeader", edm::InputTag("generator"))
        ->setComment("tag for the GenLumiInfoProduct, to get the model string");
    desc.add<std::vector<edm::InputTag>>("lheInfo", std::vector<edm::InputTag>{{"externalLHEProducer"}, {"source"}})
        ->setComment("tag(s) for the LHE information (LHEEventProduct and LHERunInfoProduct)");

    edm::ParameterSetDescription prefpdf;
    prefpdf.add<std::string>("name");
    prefpdf.add<uint32_t>("lhaid");
    desc.addVPSet("preferredPDFs", prefpdf, std::vector<edm::ParameterSet>())
        ->setComment(
            "LHA PDF Ids of the preferred PDF sets, in order of preference (the first matching one will be used)");
    desc.add<std::vector<std::string>>("namedWeightIDs")->setComment("set of LHA weight IDs for named LHE weights");
    desc.add<std::vector<std::string>>("namedWeightLabels")
        ->setComment("output names for the namedWeightIDs (in the same order)");
    desc.add<int32_t>("lheWeightPrecision")->setComment("Number of bits in the mantissa for LHE weights");
    desc.add<uint32_t>("maxPdfWeights")->setComment("Maximum number of PDF weights to save (to crop NN replicas)");
    desc.add<bool>("keepAllPSWeights")->setComment("Store all PS weights found");
    desc.addOptionalUntracked<bool>("debug")->setComment("dump out all LHE information for one event");
    descriptions.add("genWeightsTable", desc);
  }

protected:
  const edm::EDGetTokenT<GenEventInfoProduct> genTag_;
  const std::vector<edm::InputTag> lheLabel_;
  const std::vector<edm::EDGetTokenT<LHEEventProduct>> lheTag_;
  const std::vector<edm::EDGetTokenT<LHERunInfoProduct>> lheRunTag_;
  const edm::EDGetTokenT<GenLumiInfoHeader> genLumiInfoHeadTag_;

  std::vector<uint32_t> preferredPDFLHAIDs_;
  std::unordered_map<std::string, uint32_t> lhaNameToID_;
  std::vector<std::string> namedWeightIDs_;
  std::vector<std::string> namedWeightLabels_;
  int lheWeightPrecision_;
  unsigned int maxPdfWeights_;
  bool keepAllPSWeights_;

  mutable std::atomic<bool> debug_, debugRun_, hasIssuedWarning_, psWeightWarning_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenWeightsTableProducer);
