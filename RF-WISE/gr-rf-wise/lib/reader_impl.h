//Upgraded by: Cui Zhao

#ifndef INCLUDED_RFID_READER_IMPL_H
#define INCLUDED_RFID_READER_IMPL_H

#include <rfid/reader.h>
#include <vector>
#include <queue>
#include <fstream>
namespace gr {
  namespace rfid {

    class reader_impl : public reader
    {
     private:
      int s_rate, d_rate,  n_cwquery_s,  n_cwack_s,n_p_down_s;
      float sample_d, n_data0_s, n_data1_s, n_cw_s, n_pw_s, n_delim_s, n_trcal_s;
      std::vector<float> query_bits, ack_bits, query_adjust_bits;
      std::vector<gr_complex> data_0_zc, data_1_zc, cw_zc, cw_ack_zc, cw_ack_zc1, cw_ack_zc2, cw_query_zc, delim_zc, frame_sync_zc, preamble_zc, preamble_zc2, rtcal_zc, trcal_zc, query_rep_zc, nak_zc, p_down_zc;
      int q_change; // 0-> increment, 1-> unchanged, 2-> decrement
      void gen_query_adjust_bits();
      void crc_append(std::vector<float> & q);
      void gen_query_bits();
      void gen_ack_bits(const float * in);
//      void gen_ack_bits();        real-time ack generation --> default ackbits

    public:
      void print_results();
      reader_impl(int sample_rate, int dac_rate);
      ~reader_impl();


      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace rfid
} // namespace gr

#endif /* INCLUDED_RFID_READER_IMPL_H */

