/* -*- c++ -*- */
/* 
 * Copyright 2014 <Nikos Kargas (nkargas@isc.tuc.gr)>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_RFID_TAG_DECODER_H
#define INCLUDED_RFID_TAG_DECODER_H

#include <rfid/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace rfid {

    /*!
     * \brief <+description of block+>
     * \ingroup rfid
     *
     */
    class RFID_API tag_decoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<tag_decoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of rfid::tag_decoder.
       *
       * To avoid accidental use of raw pointers, rfid::tag_decoder's
       * constructor is in a private implementation
       * class. rfid::tag_decoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(int sample_rate);
    };

  } // namespace rfid
} // namespace gr

#endif /* INCLUDED_RFID_TAG_DECODER_H */

