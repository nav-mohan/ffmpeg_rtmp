#include "listdevice.hpp"

#include "audiodevice.hpp"
#include "audioresampler.hpp"
#include "audioencoder.hpp"

#include "videodevice.hpp"
#include "videoencoder.hpp"
#include "videorescaler.hpp"

#include "rtmpwriter.hpp"

#include "timer.hpp"
#include <boost/asio.hpp>

#include "filereader.hpp"
#include "remotereader.hpp"
#include "imgoverlay.hpp"
#include "txtoverlay.hpp"

#include <thread>

int main()
{
    av_log_set_level(AV_LOG_DEBUG);
    avdevice_register_all();
    ListDevice *ld = new ListDevice();
    ld->RefreshDevices();
    ld->QueryForDevices();

    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    AudioDevice *adevice = new AudioDevice(ld->adi_);
    AudioResampler *aresampler = new AudioResampler(stereo, 44100, AV_SAMPLE_FMT_FLT, stereo, 44100, AV_SAMPLE_FMT_FLTP);
    AudioEncoder *aencoder = new AudioEncoder();

    VideoDevice *vdevice = new VideoDevice(ld->vdi_);
    VideoRescaler *vrescaler = new VideoRescaler(1920,1080,AV_PIX_FMT_YUVA444P, 1920, 1080, AV_PIX_FMT_YUV444P);
    VideoEncoder *vencoder = new VideoEncoder();

    RtmpWriter *rtmpwriter = new RtmpWriter(vencoder->encCtx_, aencoder->encCtx_);
    // RtmpWriter *rtmpwriter = new RtmpWriter(vencoder->encCtx_,nullptr);

    int aret,vret;
    std::atomic_bool isRunning = ATOMIC_VAR_INIT(false);

    std::thread audioThread([&](){
        while(!isRunning.load()){} // wait for the trigger
        while(1)
        {
            if(!isRunning.load()) break;
            adevice->ReadDevice();
            aresampler->Resample(adevice->doubleFrame_);
            aret = aencoder->EncodeFrame(aresampler->outFrame_);
            if(aret) rtmpwriter->SendAudioPacket(aencoder->outPkt_);
            
            // adevice->Flush() // need to call this in the adevice ReadDevice() code because of doubleFrame_
            // aencoder->Flush(); // no need to call this for AAC

        }
    });


    ImgOverlay *wm = new ImgOverlay("logo.png",306,158);
    wm->InitFilterGraph(vdevice->decCtx_,1600,920,0.8);
    TxtOverlay *np = new TxtOverlay();
    np->InitFilterGraph(vdevice->decCtx_,28,"white",50,50);
    std::thread videoThread([&](){
        while(!isRunning.load()){} // wait for the trigger
        while(1)
        {
            if(!isRunning.load()) break;
            vdevice->ReadDevice();
            np->OverlayText(vdevice->outFrame_);
            wm->OverlayImage(np->outFrame_);
            vrescaler->Rescale(wm->outFrame_);
            vret = vencoder->EncodeFrame(vrescaler->outFrame_);
            if(vret) rtmpwriter->SendVideoPacket(vencoder->encPkt_);
            
            vdevice->Flush();
            // vencoder->Flush(); // no need to call this for x264
            np->Flush();
            wm->Flush();
        }
    });

    // boost::asio::io_context ioContext;
    // Timer* npTimer = new Timer(ioContext, 1); // now playing timer
    // FileReader *filereader = new FileReader("hello.txt");
    // RemoteReader *remotereader = new RemoteReader(ioContext);
    // npTimer->OnTimeout = [&](){
    //     printf("NOW PLAYING REFRESH!\n");
    //     if(filereader->Refresh())
    //     {
    //         printf("NEW CONTENT %s\n",filereader->content_.c_str());
    //         std::string content = filereader->content_;
    //         np->content_ = std::move(content);
    //         np->ReInitFilterGraph();
    //     }
    //     remotereader->Run("949fm.ca","80","/",10);
    // };
    // npTimer->StartTimer();

    // std::thread helperThread([&](){
    //    while(!isRunning.load()) {}
    //     ioContext.run();
    // });


    isRunning.store(1);
    audioThread.join();
    videoThread.join();
    // helperThread.join();

}