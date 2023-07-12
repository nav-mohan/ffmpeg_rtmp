#include "listdevice.hpp"

void ListDevice::PrintDevices(){

    pFormatCtx_ = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    avformat_open_input(&pFormatCtx_,"",av_find_input_format("avfoundation"),&options);
    avformat_close_input(&pFormatCtx_);
    av_dict_free(&options);
}

ListDevice::ListDevice() : deviceNameRegex_("\\[(\\d+)\\](.*)\n")
{
    avdevice_register_all();
}

void ListDevice::RefreshDevices()
{
    vDevices_.clear();
    aDevices_.clear();

    FILE* oldStdout = stderr;

    // Create a temporary file to redirect stderr
    FILE* tempFile = tmpfile();
    if (tempFile == NULL) {
        fprintf(stderr, "Failed to create temporary file.\n");
    }

    // Redirect stdout to the temporary file
    stderr = tempFile;

    PrintDevices();

    // Restore the original stderr
    stderr = oldStdout;

    // Seek to the beginning of the temporary file
    rewind(tempFile);

    // Read the contents of the temporary file into a string
    char capturedOutput[1024];
    size_t bytesRead = fread(capturedOutput, sizeof(char), sizeof(capturedOutput) - 1, tempFile);
    capturedOutput[bytesRead] = '\0';

    // Close and delete the temporary file
    fclose(tempFile);

    std::string str(capturedOutput);
    std::sregex_iterator end;
    std::sregex_iterator it(str.begin(), str.end(), deviceNameRegex_);
    while(it!=end)
    {
        const int di = atoi(it->str(1).c_str());
        if(vDevices_.find(di) == vDevices_.end())
            vDevices_.insert(std::pair<int,std::string>(di,it->str(2)));
        else
            aDevices_.insert(std::pair<int,std::string>(di,it->str(2)));
        it++;
    }
}

void ListDevice::QueryForDevices()
{
    vdi_ = -1; adi_ = -1;
    fprintf(stdout, "Choose Video Device:\n");
    std::map<int,std::string>::const_iterator dit = vDevices_.begin();
    while(dit != vDevices_.end())
    {
        fprintf(stdout, "%d: %s\n",dit->first,dit->second.c_str());
        dit++;
    }
    while(vDevices_.find(vdi_) == vDevices_.end())
        scanf("%d",&vdi_);
    fprintf(stdout,"You chose video device %s\n",vDevices_.at(vdi_).c_str());

    fprintf(stdout, "\n---------------\n");
    fprintf(stdout, "Choose Audio Device:\n");
    dit = aDevices_.begin();
    while(dit != aDevices_.end())
    {
        fprintf(stdout, "%d: %s\n",dit->first,dit->second.c_str());
        dit++;
    }
    while(aDevices_.find(adi_) == aDevices_.end())
        scanf("%d",&adi_);
    fprintf(stdout,"You chose audio device %s\n",aDevices_.at(adi_).c_str());
}