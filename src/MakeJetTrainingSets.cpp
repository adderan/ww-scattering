#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <assert.h>
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "Common.h"

#include "ExRootTreeReader.h"
#include "ExRootTreeWriter.h"
#include "classes/DelphesClasses.h"
#include "ExRootClasses.h"
#include "ExRootTreeBranch.h"
#include "TrainJet.h"


using namespace std;

void CopyFromJet(TrainJet *trainJet, Jet *jet) {
    //simulate tracker cutoff
    if (abs(jet->Eta) > JVF_CUTOFF) {
        trainJet->BetaStar = 1.0;
    }
    else {
        trainJet->BetaStar = jet->BetaStar;
    }
    trainJet->AbsEta = abs(jet->Eta);
    trainJet->Energy = GetJetEnergy(jet);
    trainJet->Mass = jet->Mass;
    trainJet->PT = jet->PT;
}

void createTrainingSets(ExRootTreeReader *vbfnloReader, ExRootTreeReader *delphesReader, 
        ExRootTreeWriter *taggingJets, ExRootTreeWriter *backgroundJets, int start, int stop) {
    //assert(vbfnloReader->GetEntries() == delphesReader->GetEntries());

    ExRootTreeBranch *taggingJetsBranch = taggingJets->NewBranch("TrainJet", TrainJet::Class());
    ExRootTreeBranch *backgroundJetsBranch = backgroundJets->NewBranch("TrainJet", TrainJet::Class());


    TClonesArray *jetBranch = delphesReader->UseBranch("Jet");
    TClonesArray *particleBranch = vbfnloReader->UseBranch("Particle");
    
    for (int i = start; i < stop; i++) {
        cerr << "Processing event: " << i << "\n";
        vbfnloReader->ReadEntry(i);
        delphesReader->ReadEntry(i);
        
        //Find the tagging partons for this event
        TRootLHEFParticle *positiveParton = NULL;
        TRootLHEFParticle *negativeParton = NULL;
        Float_t positive_maxEta = 0.0;
        Float_t negative_minEta = 0.0;
        int nParticles = particleBranch->GetEntriesFast();
        for (int k = 0; k < nParticles; k++) {
            TRootLHEFParticle *particle = (TRootLHEFParticle*)particleBranch->At(k);
            if (particle->PID > MAX_QUARK) {
                continue;
            }
            if (particle->Status != 1) {
                continue;
            }
            if (particle->Eta > positive_maxEta) {
                positiveParton = particle;
                positive_maxEta = particle->Eta;
            }
            else if (particle->Eta < negative_minEta) {
                negativeParton = particle;
                negative_minEta = particle->Eta;
            }

        }
        if (!positiveParton && negativeParton) continue;
        //Try to match the tagging partons with tagging jets in the Delphes output
        int nJets = jetBranch->GetEntriesFast();
        int positiveJetIndex = -1;
        int negativeJetIndex = -1;
        for (int i = 0; i < nJets; i++) {
            Jet *jet = (Jet*)jetBranch->At(i);
            Float_t positiveDr = deltaR(jet->Eta, jet->Phi, positiveParton->Eta, positiveParton->Phi);
            Float_t negativeDr = deltaR(jet->Eta, jet->Phi, negativeParton->Eta, negativeParton->Phi);
            if (positiveDr < MAX_DELTA_R) positiveJetIndex = i;
            else if (negativeDr < MAX_DELTA_R) negativeJetIndex = i;
        }

        if (positiveJetIndex == -1 || negativeJetIndex == -1) continue;

        //Since this event has two matched tagging jets, use those as signal and the 
        //other jets in the event as background.
        for (int i = 0; i < nJets; i++) {
            Jet *jet = (Jet*)jetBranch->At(i);
            if (i == positiveJetIndex || i == negativeJetIndex) {
                TrainJet *newJet = (TrainJet*)taggingJetsBranch->NewEntry();
                CopyFromJet(newJet, jet);
                taggingJets->Fill();
                taggingJets->Clear();
            }
            else {
                TrainJet *newJet = (TrainJet*)backgroundJetsBranch->NewEntry();
                CopyFromJet(newJet, jet);
                backgroundJets->Fill();
                backgroundJets->Clear();
            }

        }

    }


}

int main(int argc, char **argv) {
    char *vbfnloFileName = NULL;
    char *delphesFileName = NULL;
    char *outputFileName = NULL;
    int start, stop;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputDelphesFile", required_argument, 0, 'a'},
            {"inputVBFNLOFile", required_argument, 0, 'b'},
            {"outputFile", required_argument, 0, 'c'},
            {"start", required_argument, 0, 'd'},
            {"stop", required_argument, 0, 'e'}
        };
        c = getopt_long(argc, argv, "abcde:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                delphesFileName = optarg;
                break;
            case 'b':
                vbfnloFileName = optarg; 
                break;
            case 'c':
                outputFileName = optarg;
                break;
            case 'd':
                start = atoi(optarg);
                break;
            case 'e':
                stop = atoi(optarg);
                break;
        }
    }

    TFile outputFile(outputFileName, "RECREATE");
    TFile *delphesFile = new TFile(delphesFileName);
    TFile *vbfnloFile = new TFile(vbfnloFileName);
    TTree *vbfnlo = (TTree*)vbfnloFile->Get("LHEF");
    TTree *delphes = (TTree*)delphesFile->Get("Delphes");
    ExRootTreeReader *vbfnloReader = new ExRootTreeReader(vbfnlo);
    ExRootTreeReader *delphesReader = new ExRootTreeReader(delphes);
    ExRootTreeWriter *tagJets = new ExRootTreeWriter(&outputFile, "Tagging");
    ExRootTreeWriter *backgroundJets = new ExRootTreeWriter(&outputFile, "Background");

    createTrainingSets(vbfnloReader, delphesReader, tagJets, backgroundJets, start, stop);
    tagJets->Write();
    //backgroundJets->Write();
    outputFile.Close();
}
