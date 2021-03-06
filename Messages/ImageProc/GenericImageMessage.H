// //////////////////////////////////////////////////////////////////// //
// The NRT iLab Neuromorphic Vision C++ Toolkit, Copyright(C) 2000-2011 //
// by the University of Southern California (USC) and the iLab at USC.  //
// See http://iLab.usc.edu for information about this project.          //
// //////////////////////////////////////////////////////////////////// //
// Portions of the NRT iLab Neuromorphic Vision Toolkit are protected   //
// under the U.S. patent ``Computation of Intrinsic Perceptual Saliency //
// in Visual Environments, and Applications'' by Christof Koch and      //
// Laurent Itti, California Institute of Technology, 2001 (patent       //
// pending; application number 09/912,225 filed July 23, 2001; see      //
// http://pair.uspto.gov/cgi-bin/final/home.pl for current status).     //
// //////////////////////////////////////////////////////////////////// //
// This file is part of the NRT iLab Neuromorphic Vision C++ Toolkit.   //
//                                                                      //
// The NRT iLab Neuromorphic Vision C++ Toolkit is free software; you   //
// can redistribute it and/or modify it under the terms of the GNU      //
// General Public License as published by the Free Software Foundation; //
// either version 2, or (at your option) any later version.             //
//                                                                      //
// The NRT iLab Neuromorphic Vision C++ Toolkit is distributed in the   //
// hope that it will be useful, but WITHOUT ANY WARRANTY; without even  //
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  //
// PURPOSE.  See the GNU General Public License for more details.       //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with the NRT iLab Neuromorphic Vision C++ Toolkit; if not,     //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite  //
// 330, Boston, MA 02111-1307 USA.                                      //
// //////////////////////////////////////////////////////////////////// //
//
// Primary maintainer for this file: Laurent Itti <itti@usc.edu>
// $Id: test-MessageMaster.C 9 2011-04-01 06:23:09Z itti $
//

#ifndef MESSAGES_IMAGEPROC_GENERICIMAGEMESSAGE_H
#define MESSAGES_IMAGEPROC_GENERICIMAGEMESSAGE_H

#include <nrt/Core/Blackboard/Message.H>
#include <nrt/Core/Image/GenericImage.H>

NRT_DECLARE_MESSAGE(GenericImageMessage)
{
  public:
    GenericImageMessage() { }
    GenericImageMessage(nrt::GenericImage img_) : img(img_) { }
    template<class T>
      GenericImageMessage(nrt::Image<T> img_) : img(nrt::GenericImage(img_)) {}
    virtual ~GenericImageMessage() { }

    nrt::GenericImage img;

  public:
    template <class Archive> inline void serialize(Archive& ar, unsigned int const version) { ar & NRT_VAR(img); }
};

#endif // MESSAGES_IMAGEPROC_GENERICIMAGEMESSAGE_H

