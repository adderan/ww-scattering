import os

repo = "/export/home/aldenderan/ww-scattering/"
events = "/export/share/data/nkgarner/"
signalEventsDir = os.path.join(events, "Delphes_OUTPUT/VBFNLO_100K_Events")

jetWeights = os.path.join(repo, "weights/JetClassification_BDT.weights.xml")
eventWeights = os.path.join(repo, "weights/EventClassification_BDT.weights.xml")

jetTrainFile = os.path.join(repo, "output/JetClassifierTrainingSets.root")
eventTrainFile = os.path.join(repo, "output/EventClassifierTrainingSets.root")
smSignalHistogram = os.path.join(repo, "output/smSignalHist.root")
ttbarHistogram = os.path.join(repo, "output/ttbarHist.root")
wpjetsHistogram = os.path.join(repo, "output/wpjetsHist.root")
opsSignalHistogram = os.path.join(repo, "output/opsHist.root")

signalNames = ['14TeV_201_100K_Events_1500mjj', '14TeV_201_100K_Events_4.6e-6CWWW_1500mjj',
        '14TeV_201_100K_Events_4e-6CWWW_1500mjj', '14TeV_201_100K_Events_5.33e-6CWWW_1500mjj',
        '14TeV_201_100K_Events_6.25e-6CWWW_1500mjj', '14TeV_201_100K_Events_8.16e-6CWWW_1500mjj']

smdelphes = os.path.join(events, "Delphes_OUTPUT/VBFNLO_100K_Events/14TeV_201_100K_Events_1500mjj")
smparton = os.path.join(events, "VBFNLO-2.7.0_OUTPUT/14TeV_201_100K_Events_1500mjj/")
ttbar = os.path.join(events, "Delphes_OUTPUT/MG5_v2_2_1_100K_Events/14TeV_TTBar_100K_Events_1500mjj.root")
wpjets = os.path.join(events, "Delphes_OUTPUT/MG5_v2_2_1_100K_Events/14TeV_Wp3Jets_100K_Events_1500mjj.root")
opsdelphes = os.path.join(events, "Delphes_OUTPUT/VBFNLO_100K_Events/14TeV_201_100K_Events_4.6e-6CWWW_1500mjj")

#Event range allocations
jetTrainStart = 0
jetTrainStop = 1000
eventTrainStart = 1000
eventTrainStop = 20000
experimentStart = 20000
experimentStop = 100000
