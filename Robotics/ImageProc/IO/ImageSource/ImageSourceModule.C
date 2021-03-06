//NRT_HEADER_BEGIN
/*! @file ImageSource/ImageSourceModule.C */

// ////////////////////////////////////////////////////////////////////////
//              The iLab Neuromorphic Robotics Toolkit (NRT)             //
// Copyright 2010 by the University of Southern California (USC) and the //
//                              iLab at USC.                             //
//                                                                       //
//                iLab - University of Southern California               //
//                Hedco Neurociences Building, Room HNB-10               //
//                    Los Angeles, Ca 90089-2520 - USA                   //
//                                                                       //
//      See http://ilab.usc.edu for information about this project.      //
// ////////////////////////////////////////////////////////////////////////
// This file is part of The iLab Neuromorphic Robotics Toolkit.          //
//                                                                       //
// The iLab Neuromorphic Robotics Toolkit is free software: you can      //
// redistribute it and/or modify it under the terms of the GNU General   //
// Public License as published by the Free Software Foundation, either   //
// version 3 of the License, or (at your option) any later version.      //
//                                                                       //
// The iLab Neuromorphic Robotics Toolkit is distributed in the hope     //
// that it will be useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       //
// PURPOSE.  See the GNU General Public License for more details.        //
//                                                                       //
// You should ave received a copy of the GNU General Public License     //
// along with The iLab Neuromorphic Robotics Toolkit.  If not, see       //
// <http://www.gnu.org/licenses/>.                                       //
// ////////////////////////////////////////////////////////////////////////
//
// Primary maintainer for this file: Randolph Voorhies
//
//NRT_HEADER_END

#include "ImageSourceModule.H"
#include <nrt/ImageProc/IO/ImageSource/details/SeekableImageSourceType.H>

#include <chrono>
#include <thread>

// ######################################################################
ImageSourceModule::ImageSourceModule(std::string const& instanceid) :
    nrt::Module(instanceid),
    itsImageSource(new nrt::ImageSource("source")),
    itsFramerateParam(imagesource::ParamDefFrameRate, this),
    itsLoopParam(imagesource::ParamDefLoop, this)
{ 
  addSubComponent(itsImageSource);
}

// ######################################################################
void ImageSourceModule::run()
{
  int frameCount = 0;
  while(running())
  {
    auto startTime = std::chrono::monotonic_clock::now();
    float const framerate = itsFramerateParam.getVal();
    if(framerate == 0) { usleep(100000); continue; }
    auto delayTime = std::chrono::milliseconds(int(1000.0F/framerate));
    auto endTime = startTime + delayTime;

    if (itsImageSource->ok())
    {
      // First, post a clock tick:
      std::unique_ptr<nrt::TriggerMessage> tickMsg(new nrt::TriggerMessage);
      post<imagesource::Tick>(tickMsg); // this will block until completion of all callbacks

      // Get the next frame:
      nrt::GenericImage const image = itsImageSource->in();

      // Post the frame count:
      std::unique_ptr<nrt::Message<int> > frameMsg(new nrt::Message<int>(frameCount));
      post<imagesource::FrameCount>(frameMsg); // this will block until completion of all callbacks

      // Post the dims of our frame:
      std::unique_ptr<nrt::Message<nrt::GenericImage::DimsType> >
        dimsMsg(new nrt::Message<nrt::GenericImage::DimsType>(image.dims()));
      post<imagesource::Dims>(dimsMsg); // this will block until completion of all callbacks

      // Post the frame itself:
      std::unique_ptr<GenericImageMessage> imgMsg(new GenericImageMessage(image));
      post<imagesource::Image>(imgMsg); // this will block until completion of all callbacks

      // First, post a clock tock:
      std::unique_ptr<nrt::TriggerMessage> tockMsg(new nrt::TriggerMessage);
      post<imagesource::Tock>(tockMsg); // this will block until completion of all callbacks

      // Increment the frame count:
      ++frameCount;
    }
    else
    {
      // Get a reference to the real image source
      std::shared_ptr<nrt::SeekableImageSourceType> fileSource = 
        std::dynamic_pointer_cast<nrt::SeekableImageSourceType>(itsImageSource->actualSource());

      // If it is a file source, then either seek back to the first frame, or break:
      if (fileSource)
      {
        if (itsLoopParam.getVal())
        {
          NRT_INFO("Looping back to frame #" << fileSource->frameRange().first);
          fileSource->seek(fileSource->frameRange().first);
        }
        else break; // If not looping, then stop when we've reached the end
      }
      else
      {
        break; // Our stream has been broken, let's get out of here.
      }
    }

    // Maintain the framerate
    if(framerate > 0)
    {
      if (std::chrono::monotonic_clock::now() > startTime + delayTime) 
        NRT_WARNING("Cannot maintain framerate"); 
      else std::this_thread::sleep_until(endTime);
    }
  }

  std::unique_ptr<nrt::TriggerMessage> donemsg(new nrt::TriggerMessage);
  post<imagesource::Done>(donemsg);
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(ImageSourceModule);

