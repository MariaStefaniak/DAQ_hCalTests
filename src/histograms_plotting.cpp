void histograms_plotting(){
    
    TFile *fin_volOff = new TFile("/Users/stefaniak.9/OneDriveOSU/hCal/OSU_DAQ/run/adc_output_16dec_OFF.root","READ");
    TFile *fin_volOn = new TFile("/Users/stefaniak.9/OneDriveOSU/hCal/OSU_DAQ/run/adc_output_16dec_ON.root","READ");
    TFile *fin_EmptyChannel = new TFile("/Users/stefaniak.9/OneDriveOSU/hCal/OSU_DAQ/run/adc_output_16dec_noChannelConnection.root","READ");
    
    TTree *tree_volOff       = (TTree*)fin_volOff->Get("T");
    TTree *tree_volOn        = (TTree*)fin_volOn->Get("T");
    TTree *tree_EmptyChannel = (TTree*)fin_EmptyChannel->Get("T");
    
    int nrEntries_Off          = tree_volOff->GetEntries();
    int nrEntries_On           = tree_volOn->GetEntries();
    int nrEntries_EmptyChannel = tree_EmptyChannel->GetEntries();
    
 //   int adc_volOff[nrEntries_Off], adc_volOn[nrEntries_On], adc_EmptyChannel[nrEntries_EmptyChannel];
    int adc_volOff, adc_volOn, adc_EmptyChannel;
   
    tree_volOff->SetBranchAddress("ch6", &adc_volOff);
    tree_volOn->SetBranchAddress("ch6", &adc_volOn);
    tree_EmptyChannel->SetBranchAddress("ch6", &adc_EmptyChannel);
    
    TH1D *hAdc_volOff       = new TH1D("hAdc_volOff","hAdc_volOff; adc", 1000, 0, 1000);
    TH1D *hAdc_volOn        = new TH1D("hAdc_volOn","hAdc_volOn; adc", 1000, 0, 1000);
    TH1D *hAdc_EmptyChannel = new TH1D("hAdc_EmptyChannel","hAdc_EmptyChannel; adc", 1000, 0, 1000);
    
    for(int iEntry = 0; iEntry < tree_volOff->GetEntries(); iEntry++){
        tree_volOff->GetEntry(iEntry);
        hAdc_volOff->Fill(adc_volOff);
    }
    for(int iEntry = 0; iEntry < tree_volOn->GetEntries(); iEntry++){
        tree_volOn->GetEntry(iEntry);
        hAdc_volOn->Fill(adc_volOn);
    }
    for(int iEntry = 0; iEntry < tree_EmptyChannel->GetEntries(); iEntry++){
        tree_EmptyChannel->GetEntry(iEntry);
        hAdc_EmptyChannel->Fill(adc_EmptyChannel);
    }
    
    hAdc_volOn->GetXaxis()->SetRangeUser(0,200);
   // hAdc_volOn->GetYaxis()->SetRangeUser(0,80);
    hAdc_volOff->SetLineColor(kGreen);
    hAdc_volOn->SetLineColor(kRed);
    hAdc_EmptyChannel ->SetLineColor(kBlue);
    hAdc_volOff->SetLineWidth(5);
    hAdc_volOn->SetLineWidth(5);
    hAdc_EmptyChannel ->SetLineWidth(5);
    
    auto legend = new TLegend(0.1,0.7,0.48,0.9);
    legend->SetHeader("Channel 6","C"); // option "C" allows to center the header
    legend->AddEntry(hAdc_volOff,"High Voltage off","l");
    legend->AddEntry(hAdc_volOn,"High Voltage ON","l");
    legend->AddEntry(hAdc_EmptyChannel,"no connection to the channel","l");
    
    
    TCanvas *c_adc = new TCanvas();
    hAdc_volOn->Draw();
    hAdc_volOff->Draw("same");
    hAdc_EmptyChannel->Draw("same");
    legend->Draw();
    
    c_adc->SaveAs("adc_High_V.pdf");
    
    
}
